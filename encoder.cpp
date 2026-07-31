// 原打算用ffmpeg实现
//  extern "C"
//  {
//  #include <libavcodec/avcodec.h>
//  #include <libavformat/avformat.h>
//  #include <libswscale/swscale.h>
//  }
#include "encoder.h"
#include "font_array.h"
#include "rk_venc_cmd.h"
#include "camera_source.h"
#include "unistd.h"
#include <string.h>
#include <stdlib.h>
#include <cerrno>

#include "free_type_osd.h"
FreeTypeOSD g_ftosd;

#define msleep(x) usleep((x) * 1000)
#define MPP_FRAME_FMT_IS_FBC(fmt) (fmt & MPP_FRAME_FBC_MASK)
#define SZ_4K (4096)
#define VIDEO_FRAME_AUD_LEN 6

static uint8_t frameAudInfo[VIDEO_FRAME_AUD_LEN] = {0x00, 0x00, 0x00, 0x01, 0x09, 0x10};
static void fill_half_char(char *dst, unsigned char (*src)[32], unsigned int index, unsigned int total_size, unsigned int color = 1);
static unsigned int fill_int_string(char *dst, unsigned int value, unsigned int index, unsigned int total_size);
static void fill_one_char(char *dst, const unsigned char (*src)[32], unsigned int index, unsigned int total_size, unsigned int color = 1);
static unsigned int fill_float_string(char *dst, unsigned int value, unsigned int index, unsigned int total_size);
bool encoder::isCN = true;
RK_S32 os_get_env_u32(const char *name, RK_U32 *value, RK_U32 default_value);

MPP_RET mpi_enc_gen_smart_gop_ref_cfg(MppEncRefCfg ref, RK_S32 gop_len, RK_S32 vi_len)
{
    MppEncRefLtFrmCfg lt_ref[4];
    MppEncRefStFrmCfg st_ref[16];
    RK_S32 lt_cnt = 1;
    RK_S32 st_cnt = 8;
    RK_S32 pos = 0;
    MPP_RET ret = MPP_OK;
    memset(&lt_ref, 0, sizeof(lt_ref));
    memset(&st_ref, 0, sizeof(st_ref));

    ret = mpp_enc_ref_cfg_set_cfg_cnt(ref, lt_cnt, st_cnt);

    /* set 8 frame lt-ref gap */
    lt_ref[0].lt_idx = 0;
    lt_ref[0].temporal_id = 0;
    lt_ref[0].ref_mode = REF_TO_PREV_LT_REF;
    lt_ref[0].lt_gap = gop_len;
    lt_ref[0].lt_delay = 0;

    ret = mpp_enc_ref_cfg_add_lt_cfg(ref, 1, lt_ref);

    /* st 0 layer 0 - ref */
    st_ref[pos].is_non_ref = 0;
    st_ref[pos].temporal_id = 0;
    st_ref[pos].ref_mode = REF_TO_PREV_INTRA;
    st_ref[pos].ref_arg = 0;
    st_ref[pos].repeat = 0;
    pos++;

    /* st 1 layer 1 - non-ref */
    if (vi_len > 1)
    {
        st_ref[pos].is_non_ref = 0;
        st_ref[pos].temporal_id = 1;
        st_ref[pos].ref_mode = REF_TO_PREV_REF_FRM;
        st_ref[pos].ref_arg = 0;
        st_ref[pos].repeat = vi_len - 2;
        pos++;
    }

    st_ref[pos].is_non_ref = 0;
    st_ref[pos].temporal_id = 0;
    st_ref[pos].ref_mode = REF_TO_PREV_INTRA;
    st_ref[pos].ref_arg = 0;
    st_ref[pos].repeat = 0;
    pos++;

    ret = mpp_enc_ref_cfg_add_st_cfg(ref, pos, st_ref);

    /* check and get dpb size */
    ret = mpp_enc_ref_cfg_check(ref);

    return ret;
}
MPP_RET mpi_enc_gen_ref_cfg(MppEncRefCfg ref, RK_S32 gop_mode)
{
    MppEncRefLtFrmCfg lt_ref[4];
    MppEncRefStFrmCfg st_ref[16];
    RK_S32 lt_cnt = 0;
    RK_S32 st_cnt = 0;
    MPP_RET ret = MPP_OK;

    memset(&lt_ref, 0, sizeof(lt_ref));
    memset(&st_ref, 0, sizeof(st_ref));

    switch (gop_mode)
    {
    case 3:
    {
        // tsvc4
        //      /-> P1      /-> P3        /-> P5      /-> P7
        //     /           /             /           /
        //    //--------> P2            //--------> P6
        //   //                        //
        //  ///---------------------> P4
        // ///
        // P0 ------------------------------------------------> P8
        lt_cnt = 1;

        /* set 8 frame lt-ref gap */
        lt_ref[0].lt_idx = 0;
        lt_ref[0].temporal_id = 0;
        lt_ref[0].ref_mode = REF_TO_PREV_LT_REF;
        lt_ref[0].lt_gap = 8;
        lt_ref[0].lt_delay = 0;

        st_cnt = 9;
        /* set tsvc4 st-ref struct */
        /* st 0 layer 0 - ref */
        st_ref[0].is_non_ref = 0;
        st_ref[0].temporal_id = 0;
        st_ref[0].ref_mode = REF_TO_TEMPORAL_LAYER;
        st_ref[0].ref_arg = 0;
        st_ref[0].repeat = 0;
        /* st 1 layer 3 - non-ref */
        st_ref[1].is_non_ref = 1;
        st_ref[1].temporal_id = 3;
        st_ref[1].ref_mode = REF_TO_PREV_REF_FRM;
        st_ref[1].ref_arg = 0;
        st_ref[1].repeat = 0;
        /* st 2 layer 2 - ref */
        st_ref[2].is_non_ref = 0;
        st_ref[2].temporal_id = 2;
        st_ref[2].ref_mode = REF_TO_PREV_REF_FRM;
        st_ref[2].ref_arg = 0;
        st_ref[2].repeat = 0;
        /* st 3 layer 3 - non-ref */
        st_ref[3].is_non_ref = 1;
        st_ref[3].temporal_id = 3;
        st_ref[3].ref_mode = REF_TO_PREV_REF_FRM;
        st_ref[3].ref_arg = 0;
        st_ref[3].repeat = 0;
        /* st 4 layer 1 - ref */
        st_ref[4].is_non_ref = 0;
        st_ref[4].temporal_id = 1;
        st_ref[4].ref_mode = REF_TO_PREV_LT_REF;
        st_ref[4].ref_arg = 0;
        st_ref[4].repeat = 0;
        /* st 5 layer 3 - non-ref */
        st_ref[5].is_non_ref = 1;
        st_ref[5].temporal_id = 3;
        st_ref[5].ref_mode = REF_TO_PREV_REF_FRM;
        st_ref[5].ref_arg = 0;
        st_ref[5].repeat = 0;
        /* st 6 layer 2 - ref */
        st_ref[6].is_non_ref = 0;
        st_ref[6].temporal_id = 2;
        st_ref[6].ref_mode = REF_TO_PREV_REF_FRM;
        st_ref[6].ref_arg = 0;
        st_ref[6].repeat = 0;
        /* st 7 layer 3 - non-ref */
        st_ref[7].is_non_ref = 1;
        st_ref[7].temporal_id = 3;
        st_ref[7].ref_mode = REF_TO_PREV_REF_FRM;
        st_ref[7].ref_arg = 0;
        st_ref[7].repeat = 0;
        /* st 8 layer 0 - ref */
        st_ref[8].is_non_ref = 0;
        st_ref[8].temporal_id = 0;
        st_ref[8].ref_mode = REF_TO_TEMPORAL_LAYER;
        st_ref[8].ref_arg = 0;
        st_ref[8].repeat = 0;
    }
    break;
    case 2:
    {
        // tsvc3
        //     /-> P1      /-> P3
        //    /           /
        //   //--------> P2
        //  //
        // P0/---------------------> P4
        lt_cnt = 0;

        st_cnt = 5;
        /* set tsvc4 st-ref struct */
        /* st 0 layer 0 - ref */
        st_ref[0].is_non_ref = 0;
        st_ref[0].temporal_id = 0;
        st_ref[0].ref_mode = REF_TO_TEMPORAL_LAYER;
        st_ref[0].ref_arg = 0;
        st_ref[0].repeat = 0;
        /* st 1 layer 2 - non-ref */
        st_ref[1].is_non_ref = 1;
        st_ref[1].temporal_id = 2;
        st_ref[1].ref_mode = REF_TO_PREV_REF_FRM;
        st_ref[1].ref_arg = 0;
        st_ref[1].repeat = 0;
        /* st 2 layer 1 - ref */
        st_ref[2].is_non_ref = 0;
        st_ref[2].temporal_id = 1;
        st_ref[2].ref_mode = REF_TO_PREV_REF_FRM;
        st_ref[2].ref_arg = 0;
        st_ref[2].repeat = 0;
        /* st 3 layer 2 - non-ref */
        st_ref[3].is_non_ref = 1;
        st_ref[3].temporal_id = 2;
        st_ref[3].ref_mode = REF_TO_PREV_REF_FRM;
        st_ref[3].ref_arg = 0;
        st_ref[3].repeat = 0;
        /* st 4 layer 0 - ref */
        st_ref[4].is_non_ref = 0;
        st_ref[4].temporal_id = 0;
        st_ref[4].ref_mode = REF_TO_TEMPORAL_LAYER;
        st_ref[4].ref_arg = 0;
        st_ref[4].repeat = 0;
    }
    break;
    case 1:
    {
        // tsvc2
        //   /-> P1
        //  /
        // P0--------> P2
        lt_cnt = 0;

        st_cnt = 3;
        /* set tsvc4 st-ref struct */
        /* st 0 layer 0 - ref */
        st_ref[0].is_non_ref = 0;
        st_ref[0].temporal_id = 0;
        st_ref[0].ref_mode = REF_TO_TEMPORAL_LAYER;
        st_ref[0].ref_arg = 0;
        st_ref[0].repeat = 0;
        /* st 1 layer 2 - non-ref */
        st_ref[1].is_non_ref = 1;
        st_ref[1].temporal_id = 1;
        st_ref[1].ref_mode = REF_TO_PREV_REF_FRM;
        st_ref[1].ref_arg = 0;
        st_ref[1].repeat = 0;
        /* st 2 layer 1 - ref */
        st_ref[2].is_non_ref = 0;
        st_ref[2].temporal_id = 0;
        st_ref[2].ref_mode = REF_TO_PREV_REF_FRM;
        st_ref[2].ref_arg = 0;
        st_ref[2].repeat = 0;
    }
    break;
    default:
    {
        printf("unsupport gop mode %d\n", gop_mode);
    }
    break;
    }

    if (lt_cnt || st_cnt)
    {
        ret = mpp_enc_ref_cfg_set_cfg_cnt(ref, lt_cnt, st_cnt);

        if (lt_cnt)
            ret = mpp_enc_ref_cfg_add_lt_cfg(ref, lt_cnt, lt_ref);

        if (st_cnt)
            ret = mpp_enc_ref_cfg_add_st_cfg(ref, st_cnt, st_ref);

        /* check and get dpb size */
        ret = mpp_enc_ref_cfg_check(ref);
    }

    return ret;
}
MPP_RET mpi_enc_gen_osd_plt(MppEncOSDPlt *osd_plt, RK_U32 frame_cnt)
{
    /*
     * osd idx size range from 16x16 bytes(pixels) to hor_stride*ver_stride(bytes).
     * for general use, 1/8 Y buffer is enough.
     */
    static int plt_table[8] = {
        MPP_ENC_OSD_PLT_TRANS,
        MPP_ENC_OSD_PLT_WHITE,
        MPP_ENC_OSD_PLT_GREEN,
        MPP_ENC_OSD_PLT_BLUE,
        MPP_ENC_OSD_PLT_YELLOW,
        MPP_ENC_OSD_PLT_RED,
        MPP_ENC_OSD_PLT_CYAN,
        MPP_ENC_OSD_PLT_BLACK,
    };

    if (osd_plt)
    {
        RK_U32 k = 0;
        RK_U32 base = frame_cnt & 7;

        for (k = 0; k < 256; k++)
            osd_plt->data[k].val = plt_table[(base + k) % 8];
    }
    return MPP_OK;
}
static void fill_one_char(char *dst, const unsigned char (*src)[32], unsigned int index, unsigned int total_size, unsigned int color)
{
    char (*buf)[total_size * 32] = (char (*)[total_size * 32]) dst;
    for (int m = 0; m < 4; m++)
    {
        for (int n = 0; n < 32; n++)
        {
            buf[n][index] = src[m][n] >> 7 & 0x01 ? color : 0;
            buf[n][index + 1] = src[m][n] >> 6 & 0x01 ? color : 0;
            buf[n][index + 2] = src[m][n] >> 5 & 0x01 ? color : 0;
            buf[n][index + 3] = src[m][n] >> 4 & 0x01 ? color : 0;
            buf[n][index + 4] = src[m][n] >> 3 & 0x01 ? color : 0;
            buf[n][index + 5] = src[m][n] >> 2 & 0x01 ? color : 0;
            buf[n][index + 6] = src[m][n] >> 1 & 0x01 ? color : 0;
            buf[n][index + 7] = src[m][n] & 0x01 ? color : 0;
        }
        index += 8;
    }
}
// total_size 一行有几个dw。
static void fill_half_char(char *dst, unsigned char (*src)[32], unsigned int index, unsigned int total_size, unsigned int color)
{

    if (!dst || !src) {
        printf("fill_half_char: NULL pointer, dst=%p, src=%p\n", dst, src);
        return;
    }

    // 关键：检查 index 是否会导致越界
    // total_size 是每行的 DW (32字节) 数量
    // 每行有 32 行高，所以总列数 = total_size * 32
    unsigned int max_index_per_row = total_size * 32;

    // fill_half_char 会写入 16 列宽 (2段 x 8列)
    if (index + 8 > max_index_per_row) {
        printf("fill_half_char: index out of bounds! index=%u, max=%u, line_width=%u\n",
               index, max_index_per_row, total_size);
        return;
    }

    char (*buf)[total_size * 32] = (char (*)[total_size * 32]) dst;
    for (int m = 0; m < 2; m++)
    {
        // 再次检查（因为 index 可能增加了）
        if (index + 8 > max_index_per_row) {
            printf("fill_half_char: overflow in loop m=%d\n", m);
            return;
        }

        for (int n = 0; n < 32; n++)
        {
            buf[n][index] = src[m][n] >> 7 & 0x01 ? color : 0;
            buf[n][index + 1] = src[m][n] >> 6 & 0x01 ? color : 0;
            buf[n][index + 2] = src[m][n] >> 5 & 0x01 ? color : 0;
            buf[n][index + 3] = src[m][n] >> 4 & 0x01 ? color : 0;
            buf[n][index + 4] = src[m][n] >> 3 & 0x01 ? color : 0;
            buf[n][index + 5] = src[m][n] >> 2 & 0x01 ? color : 0;
            buf[n][index + 6] = src[m][n] >> 1 & 0x01 ? color : 0;
            buf[n][index + 7] = src[m][n] & 0x01 ? color : 0;
        }
        index += 8;
    }
}
static void fill_string(char *dst, const char *str, unsigned int index, unsigned int line_width = 8, unsigned int color = 1)
{
    unsigned int str_len = strlen(str);
    for (int i = 0; i < str_len; i++, index += 16)
    {
        fill_half_char(dst, &Alpha_bet[(str[i] - 'a') * 2], index, line_width, color);
    }
}
/// @brief 在某行中指定位置填充整数数字字符串,返回数字的位数
/// @param dst osd缓冲区
/// @param value 数字
/// @param index 指定填充位置
/// @param total_size 缓冲区当前行宽
static unsigned int fill_int_string(char *dst, unsigned int value, unsigned int index, unsigned int total_size)
{
    unsigned int ret;
    unsigned int every_num[6] = {0};
    int i = 0;
    for (i = 0; value > 9; i++)
    {
        every_num[i] = value % 10;
        value /= 10;
    }
    ret = i + 1;
    every_num[i] = value;
    for (; i >= 0; i--, index += 16)
    {
        fill_half_char(dst, (unsigned char (*)[32]) & NUM[2 * every_num[i]][0], index, total_size);
    }
    return ret;
}

/// @brief 在某行中指定位置填充一位小数浮点数字符串,返回数字的位数（小数点算一位）
/// @param dst osd缓冲区
/// @param value 数字*10
/// @param index 指定填充位置（从开头起第几个像素）
/// @param total_size 缓冲区当前行宽
static unsigned int fill_float_string(char *dst, unsigned int value, unsigned int index, unsigned int total_size)
{
    unsigned int every_num[6] = {0};
    int ret;
    int i;
    for (i = 0; value > 9; i++)
    {
        every_num[i] = value % 10;
        value /= 10;
    }
    every_num[i] = value;

    if (!i)
    {
        fill_half_char(dst, (unsigned char (*)[32]) & NUM[0][0], index, total_size); // 补0
        index += 16;
    }

    ret = i + 3;
    for (; i >= 0; i--, index += 16)
    {
        if (i)
            fill_half_char(dst, (unsigned char (*)[32]) & NUM[2 * every_num[i]][0], index, total_size); // 整数
        else
        {
            fill_half_char(dst, (unsigned char (*)[32]) & NUM[2 * 10][0], index, total_size); //.
            index += 16;
            fill_half_char(dst, (unsigned char (*)[32]) & NUM[2 * every_num[i]][0], index, total_size); // 小数
        }
    }
    return ret;
}
void encoder::draw_en_osd(MppEncOSDData *osd_data)
{
    int DW_per_line = osd_data->region[0].num_mb_x/2;
    char *ptr = (char *)mpp_buffer_get_ptr(osd_data->buf);
    
    int buf_width = osd_data->region[0].num_mb_x * 16;
    int buf_height = osd_data->region[0].num_mb_y * 16;
    int buf_stride = buf_width;
    unsigned char color = 1;
    g_ftosd.render_text("Gain: 0%", (unsigned char *)ptr, buf_width, buf_height / 2+2, buf_stride, 0, 32, color);
    g_ftosd.render_text("Dist: 0m", (unsigned char *)ptr, buf_width, buf_height / 2+2, buf_stride, 300, 32, color);
    g_ftosd.render_text("UVPT: 0/M", (unsigned char *)ptr, buf_width, buf_height / 2+2, buf_stride, 580, 32, color);

    unsigned char *ptr2 = (unsigned char *)ptr + buf_width * (buf_height / 2);
    g_ftosd.render_text("Mode: Fuse", (unsigned char *)ptr2, buf_width, buf_height / 2+2, buf_stride, 0, 32, color);
    g_ftosd.render_text("T&H: 0°C/0%", (unsigned char *)ptr2, buf_width, buf_height / 2+2, buf_stride, 300, 32, color);
    
    // region[1]: 公司logo "紫红光电 UVIRSYS"
    int logo_buf_width = osd_data->region[1].num_mb_x * 16;
    int logo_buf_height = osd_data->region[1].num_mb_y * 16;
    unsigned char *logo_ptr = (unsigned char *)ptr + osd_data->region[1].buf_offset;
    g_ftosd.render_text("紫红光电 UVIRSYS", logo_ptr, logo_buf_width, logo_buf_height, logo_buf_width, 0, 32, color);

    // region[2]: GPS信息
    int gps_buf_width = osd_data->region[2].num_mb_x * 16;
    int gps_buf_height = osd_data->region[2].num_mb_y * 16;
    unsigned char *gps_ptr = (unsigned char *)ptr + osd_data->region[2].buf_offset;
    g_ftosd.render_text("N:0.00 E:0.00", gps_ptr, gps_buf_width, gps_buf_height, gps_buf_width, 0, 32, color);
}
void encoder::modify_osd_T_and_H_data(MppEncOSDData *osd_data, unsigned int T_value, unsigned int H_value)
{
    MppEncOSDRegion *region = osd_data->region;
    char *ptr = (char *)mpp_buffer_get_ptr(osd_data->buf);
    if (!ptr)
        return;
    int buf_width = region->num_mb_x * 16;
    int buf_height = region->num_mb_y * 16;
    int buf_stride = buf_width;
    unsigned char color = 1;
    ptr += buf_width * (buf_height / 2);

    // 更新温度
    if (T_value > 99)
        T_value = 99;
    else if (T_value < 0)
        T_value = 0;
    // 更新湿度
    if (H_value > 99)
        H_value = 99;
    else if (H_value < 0)
        H_value = 0;

    char gain_text[32];
    snprintf(gain_text, sizeof(gain_text), "%d°C/%d%%", H_value, T_value);
    if (isCN)
    {
        clear_area((unsigned char *)ptr, buf_width, buf_height / 2, 300+32*4, 0, 160, buf_height / 2, buf_stride);
        g_ftosd.render_text(gain_text, (unsigned char *)ptr, buf_width, buf_height / 2, buf_stride, 300+32*4, 32, color);
    }else{
        clear_area((unsigned char *)ptr, buf_width, buf_height / 2, 300+32*3-16, 0, 160, buf_height / 2, buf_stride);
        g_ftosd.render_text(gain_text, (unsigned char *)ptr, buf_width, buf_height / 2, buf_stride, 300+32*3-16, 32, color);
    }
    
}

void encoder::modify_osd_gps_data(MppEncOSDData *osd_data, const char *gps_str)
{
    if (!osd_data || !osd_data->buf || !gps_str)
        return;

    MppEncOSDRegion *region = &osd_data->region[2];
    char *ptr = (char *)mpp_buffer_get_ptr(osd_data->buf);
    if (!ptr)
        return;

    int gps_buf_width = region->num_mb_x * 16;
    int gps_buf_height = region->num_mb_y * 16;
    unsigned char *gps_ptr = (unsigned char *)ptr + region->buf_offset;
    unsigned char color = 1;

    clear_area(gps_ptr, gps_buf_width, gps_buf_height, 0, 0, gps_buf_width, gps_buf_height, gps_buf_width);
    g_ftosd.render_text(gps_str, gps_ptr, gps_buf_width, gps_buf_height, gps_buf_width, 0, 32, color);
}

void encoder::draw_cn_osd(MppEncOSDData *osd_data)
{
    int DW_per_line = osd_data->region[0].num_mb_x/2;
    char *ptr = (char *)mpp_buffer_get_ptr(osd_data->buf);
    
    int buf_width = osd_data->region[0].num_mb_x * 16;
    int buf_height = osd_data->region[0].num_mb_y * 16;
    int buf_stride = buf_width;
    unsigned char color = 1;

    g_ftosd.render_text("增益：0", (unsigned char *)ptr, buf_width, buf_height / 2 +2, buf_stride, 0, 32, color);
    g_ftosd.render_text("距离：0m", (unsigned char *)ptr, buf_width, buf_height / 2+2, buf_stride, 300, 32, color);
    g_ftosd.render_text("计数：0/M", (unsigned char *)ptr, buf_width, buf_height / 2+2, buf_stride, 580, 32, color);

    unsigned char *ptr2 = (unsigned char *)ptr + buf_width * (buf_height / 2);
    g_ftosd.render_text("模式：融合", (unsigned char *)ptr2, buf_width, buf_height / 2+2, buf_stride, 0, 32, color);
    g_ftosd.render_text("温湿度：0°C/0%", (unsigned char *)ptr2, buf_width, buf_height / 2+2, buf_stride, 300, 32, color);
    
    // region[1]: 公司logo "紫红光电 UVIRSYS"
    int logo_buf_width = osd_data->region[1].num_mb_x * 16;
    int logo_buf_height = osd_data->region[1].num_mb_y * 16;
    unsigned char *logo_ptr = (unsigned char *)ptr + osd_data->region[1].buf_offset;
    g_ftosd.render_text("紫红光电 UVIRSYS", logo_ptr, logo_buf_width, logo_buf_height, logo_buf_width, 0, 32, color);

    // region[2]: GPS信息
    int gps_buf_width = osd_data->region[2].num_mb_x * 16;
    int gps_buf_height = osd_data->region[2].num_mb_y * 16;
    unsigned char *gps_ptr = (unsigned char *)ptr + osd_data->region[2].buf_offset;
    g_ftosd.render_text("N:0.00 E:0.00", gps_ptr, gps_buf_width, gps_buf_height, gps_buf_width, 0, 32, color);
}

RK_S32 mpi_enc_width_default_stride(RK_S32 width, MppFrameFormat fmt)
{
    RK_S32 stride = 0;

    switch (fmt & MPP_FRAME_FMT_MASK)
    {
    case MPP_FMT_YUV420SP:
    case MPP_FMT_YUV420SP_VU:
    {
        stride = MPP_ALIGN(width, 8);
    }
    break;
    case MPP_FMT_YUV420P:
    {
        /* NOTE: 420P need to align to 16 so chroma can align to 8 */
        stride = MPP_ALIGN(width, 16);
    }
    break;
    case MPP_FMT_YUV422P:
    case MPP_FMT_YUV422SP:
    case MPP_FMT_YUV422SP_VU:
    {
        /* NOTE: 422 need to align to 8 so chroma can align to 16 */
        stride = MPP_ALIGN(width, 8);
    }
    break;
    case MPP_FMT_YUV444SP:
    case MPP_FMT_YUV444P:
    {
        stride = MPP_ALIGN(width, 8);
    }
    break;
    case MPP_FMT_RGB565:
    case MPP_FMT_BGR565:
    case MPP_FMT_RGB555:
    case MPP_FMT_BGR555:
    case MPP_FMT_RGB444:
    case MPP_FMT_BGR444:
    case MPP_FMT_YUV422_YUYV:
    case MPP_FMT_YUV422_YVYU:
    case MPP_FMT_YUV422_UYVY:
    case MPP_FMT_YUV422_VYUY:
    {
        /* NOTE: for vepu limitation */
        stride = MPP_ALIGN(width, 8) * 2;
    }
    break;
    case MPP_FMT_RGB888:
    case MPP_FMT_BGR888:
    {
        /* NOTE: for vepu limitation */
        stride = MPP_ALIGN(width, 8) * 3;
    }
    break;
    case MPP_FMT_RGB101010:
    case MPP_FMT_BGR101010:
    case MPP_FMT_ARGB8888:
    case MPP_FMT_ABGR8888:
    case MPP_FMT_BGRA8888:
    case MPP_FMT_RGBA8888:
    {
        /* NOTE: for vepu limitation */
        stride = MPP_ALIGN(width, 8) * 4;
    }
    break;
    default:
    {
        printf("do not support type %d\n", fmt);
    }
    break;
    }

    return stride;
}
MPP_RET encoder::gen_osd_data(MppEncOSDData *osd_data, MppBufferGroup group)
{
    MppEncOSDRegion *region = osd_data->region;
    MppBuffer buf = osd_data->buf;
    int DW_per_line = 30;
    RK_U32 buf_size = 0;
    osd_data->num_region = 3;
    int count = 0;

    if (buf)
        buf_size = mpp_buffer_get_size(buf);
    // 光子数，距离，增益,等等
    region[0].inverse = 0;
    region[0].start_mb_x = 6;
    region[0].start_mb_y = 62;
    region[0].num_mb_x = DW_per_line * 2;
    region[0].num_mb_y = 2 * 2+1;
    region[0].buf_offset = 0;
    region[0].enable = 1;
    //公司logo "紫红光电 UVIRSYS"
    region[1].inverse = 0;
    region[1].start_mb_x = 0;
    region[1].start_mb_y = 0;
    region[1].num_mb_x = 20;
    region[1].num_mb_y = 4;
    region[1].buf_offset = region[0].num_mb_x * region[0].num_mb_y * 256;
    region[1].enable = 1;
    //GPS信息 右上角
    region[2].inverse = 0;
    region[2].start_mb_x = 96;
    region[2].start_mb_y = 0;
    region[2].num_mb_x = 24;
    region[2].num_mb_y = 4;
    region[2].buf_offset = region[0].num_mb_x * region[0].num_mb_y * 256 + region[1].num_mb_x * region[1].num_mb_y * 256;
    region[2].enable = 1;

    /* create buffer and write osd index data */
    RK_U32 total_buf_size = region[0].num_mb_x * region[0].num_mb_y * 256 + region[1].num_mb_x * region[1].num_mb_y * 256 + region[2].num_mb_x * region[2].num_mb_y * 256;
    if (buf_size < total_buf_size)
    {
        buf_size = total_buf_size;
        if (buf)
            mpp_buffer_put(buf);
        mpp_buffer_get(group, &buf, buf_size);
        if (NULL == buf)
            printf("failed to create osd buffer size %d\n", buf_size);
    }

    if (buf)
    {
        osd_data->buf = buf;
        if (isCN)
            draw_cn_osd(osd_data);
        else
            draw_en_osd(osd_data);
    }
    return MPP_OK;
}

void clear_region0(MppEncOSDData *osd_data)
{
    if (!osd_data || !osd_data->buf || !osd_data->region)
        return;   
        
    MppEncOSDRegion *region = osd_data->region;
    char *ptr = (char *)mpp_buffer_get_ptr(osd_data->buf);
    
    if (!ptr)
        return;
    
    int buf_width = region[0].num_mb_x * 16;
    int buf_height = region[0].num_mb_y * 16;
    int buf_stride = buf_width;
    
    if (buf_width <= 0 || buf_height <= 0)
        return;
    
    clear_area((unsigned char *)ptr, buf_width, buf_height, 0, 0, buf_width, buf_height, buf_stride);
}
void encoder::switch_language(MppEncOSDData *osd_data, bool is_CN)
{
    if (!osd_data)
        return;
        
    clear_region0(osd_data);
    encoder::isCN = is_CN;
    MppBuffer buf = osd_data->buf;
    if (buf)
    {
        if (isCN)
            draw_cn_osd(osd_data);
        else
            draw_en_osd(osd_data);
    }
}

static uint32_t calculate_nal_len(char *ptr, int len)
{

    int head_index = -1;
    int end_index = -1;
    char nal_head[] = {0x00, 0x00, 0x00, 0x01};
    uint32_t i = 0, j = 0;

    for (;;)
    {
        if (ptr[i + j] != nal_head[j])
        {
            i++;
            j = 0;
            continue;
        }
        else
            j++;

        if (j == 4)
        {

            if (head_index < 0)
            {
                head_index = i;
                i += 4;
            }
            else if (end_index < 0)
            {
                end_index = i;
                break;
            }
            j = 0;
        }
    }

    return end_index - head_index;
}
encoder::encoder(RK_U32 width, RK_U32 height, MppFrameFormat frame_fmt, MppCodingType type, MppEncRcMode rc_mode) : args{0}, osd_data{0}, m_filepath{0}, m_record_state(record_idle)
{
    args.width = width;
    args.height = height;
    args.fmt = frame_fmt;
    args.hor_stride = mpi_enc_width_default_stride(width, args.fmt);
    args.ver_stride = MPP_ALIGN(height, 16);
    args.rc_mode = rc_mode;
    args.type = type;
    args.frame_size = calc_buf_size(frame_fmt);
    if (MPP_FRAME_FMT_IS_FBC(args.fmt))
    {
        if ((args.fmt & MPP_FRAME_FBC_MASK) == MPP_FRAME_FBC_AFBC_V1)
            args.header_size = MPP_ALIGN(MPP_ALIGN(args.width, 16) * MPP_ALIGN(args.height, 16) / 16, SZ_4K);
        else
            args.header_size = MPP_ALIGN(args.width, 16) * MPP_ALIGN(args.height, 16) / 16;
    }
    else
    {
        args.header_size = 0;
    }
}

encoder::~encoder()
{
    deinit();
}
MPP_RET encoder::init()
{
    // 尝试在指定目录下寻找常见字库并初始化 FreeType OSD（非致命）
    const char *font_dir = "/usr/share/fonts/source-han-sans-cn";
    const char *font_candidates[] = {
                                     "SourceHanSansCN-Regular.otf",
                                     NULL};
    char font_path[512] = {0};

    MPP_RET ret;
    ret = mpp_buffer_group_get_internal(&buf_grp, MPP_BUFFER_TYPE_DRM);
    if (ret)
    {
        printf("failed to get mpp buffer group ret %d\n", ret);
        goto OUT;
    }
    for (int i = 0; i < 2; i++)
    {
        ret = mpp_buffer_get(buf_grp, &mEnc_buf[i].mpp_buf, args.frame_size + args.header_size);
        if (ret)
        {
            printf("failed to get buffer for input frame ret %d\n", ret);
            goto OUT;
        }
        mEnc_buf[i].ptr = mpp_buffer_get_ptr(mEnc_buf[i].mpp_buf);
        mEnc_buf[i].fd = mpp_buffer_get_fd(mEnc_buf[i].mpp_buf);
        mEnc_buf[i].rga_buf_handle = importbuffer_fd(mEnc_buf[i].fd, (args.frame_size + args.header_size));
        mEnc_buf[i].rga_buf = wrapbuffer_handle(
            mEnc_buf[i].rga_buf_handle,
            args.width,
            args.height,
            change_MppFmt2RkFmt(args.fmt)); // 删除了, args.hor_stride, args.ver_stride
        printf("fd %d, handle %d,buf %p\n", mEnc_buf[i].fd, mEnc_buf[i].rga_buf_handle, &mEnc_buf[i].rga_buf);
    }
    mCurrent_Enc_buf = &mEnc_buf[0];

    ret = mpp_buffer_get(buf_grp, &pkt_buf, args.frame_size + args.header_size);
    if (ret)
    {
        printf("failed to get buffer for output packet ret %d\n", ret);
        goto OUT;
    }

    ret = mpp_create(&ctx, &mpi);
    if (ret)
    {
        printf("mpp_create failed ret %d\n", ret);
        goto OUT;
    }
    ret = mpi->control(ctx, MPP_SET_INPUT_TIMEOUT, &timeout);
    if (ret)
    {
        printf("mpi control set input timeout %d ret %d\n", timeout, ret);
        return ret;
    }

    timeout = MPP_POLL_BLOCK;
    ret = mpi->control(ctx, MPP_SET_OUTPUT_TIMEOUT, &timeout);
    if (MPP_OK != ret)
    {
        printf("mpi control set output timeout %d ret %d\n", timeout, ret);
        goto OUT;
    }
    ret = mpp_init(ctx, MPP_CTX_ENC, args.type);
    if (ret)
    {
        printf("mpp_init failed ret %d\n", ret);
        goto OUT;
    }
    ret = mpp_enc_cfg_init(&cfg);
    if (ret)
    {
        printf("mpp_enc_cfg_init failed ret %d\n", ret);
        goto OUT;
    }
    ret = mpp_frame_init(&frame);
    if (ret)
    {
        printf("mpp_frame_init failed\n");
        goto OUT;
    }


    for (int i = 0; font_candidates[i] != NULL; ++i)
    {
        snprintf(font_path, sizeof(font_path), "%s/%s", font_dir, font_candidates[i]);
        if (access(font_path, F_OK) == 0)
        {
            if (g_ftosd.init(font_path, 32))
            {
                qDebug() << "文件打开成功!" << font_path;
            }
            else
            {
                qDebug() << "无法打开文件!";
            }
            break;
        }
    }

    if (!osd_data.num_region)
        gen_osd_data(&osd_data, buf_grp);
    mpp_frame_set_width(frame, args.width);
    mpp_frame_set_height(frame, args.height);
    mpp_frame_set_hor_stride(frame, args.hor_stride);
    mpp_frame_set_ver_stride(frame, args.ver_stride);
    mpp_frame_set_fmt(frame, args.fmt);
    return ret;
OUT:
    deinit();
    return MPP_NOK;
}
MPP_RET encoder::cfg_setup()
{
    MPP_RET ret;
    if (cfg == NULL || ctx == NULL)
        printf("未初始化\n");
    if (args.fps_in_den == 0)
        args.fps_in_den = 1;
    if (args.fps_in_num == 0)
        args.fps_in_num = 30;
    if (args.fps_out_den == 0)
        args.fps_out_den = 1;
    if (args.fps_out_num == 0)
        args.fps_out_num = 30;
    if (!args.bps)
    {
        args.bps = 4800000; // args.width * args.height / 8 * (args.fps_out_num / args.fps_out_den);
    }
    args.gop_len = 20;
    // GDR刷新配置
    args.refresh_en = true;
    args.refresh_mode = MPP_ENC_RC_INTRA_REFRESH_ROW;
    args.refresh_num = 4;

    ret = mpp_enc_cfg_set_s32(cfg, "prep:width", args.width);
    ret = mpp_enc_cfg_set_s32(cfg, "prep:height", args.height);
    ret = mpp_enc_cfg_set_s32(cfg, "prep:hor_stride", args.hor_stride);
    ret = mpp_enc_cfg_set_s32(cfg, "prep:ver_stride", args.ver_stride);
    ret = mpp_enc_cfg_set_s32(cfg, "prep:format", args.fmt);
    ret = mpp_enc_cfg_set_s32(cfg, "rc:mode", args.rc_mode);
    /* fix input / output frame rate */
    ret = mpp_enc_cfg_set_s32(cfg, "rc:fps_in_flex", args.fps_in_flex);
    ret = mpp_enc_cfg_set_s32(cfg, "rc:fps_in_num", args.fps_in_num);
    ret = mpp_enc_cfg_set_s32(cfg, "rc:fps_in_denorm", args.fps_in_den);
    ret = mpp_enc_cfg_set_s32(cfg, "rc:fps_out_flex", args.fps_out_flex);
    ret = mpp_enc_cfg_set_s32(cfg, "rc:fps_out_num", args.fps_out_num);
    ret = mpp_enc_cfg_set_s32(cfg, "rc:fps_out_denorm", args.fps_out_den);
    ret = mpp_enc_cfg_set_s32(cfg, "rc:gop", args.gop_len ? args.gop_len : args.fps_out_num);

    /* drop frame or not when bitrate overflow */
    mpp_enc_cfg_set_u32(cfg, "rc:drop_mode", MPP_ENC_RC_DROP_FRM_DISABLED);
    mpp_enc_cfg_set_u32(cfg, "rc:drop_thd", 20); /* 20% of max bps */
    mpp_enc_cfg_set_u32(cfg, "rc:drop_gap", 1);  /* Do not continuous drop frame */
    /* setup intra refresh */
    mpp_enc_cfg_set_u32(cfg, "rc:refresh_en", args.refresh_en);
    mpp_enc_cfg_set_u32(cfg, "rc:refresh_mode", args.refresh_mode);
    mpp_enc_cfg_set_u32(cfg, "rc:refresh_num", args.refresh_num);
    /* setup bitrate for different rc_mode */
    ret = mpp_enc_cfg_set_s32(cfg, "rc:bps_target", args.bps);
    switch (args.rc_mode)
    {
    case MPP_ENC_RC_MODE_FIXQP:
    {
        /* do not setup bitrate on FIXQP mode */
    }
    break;
    case MPP_ENC_RC_MODE_CBR:
    {
        /* CBR mode has narrow bound */
        ret = mpp_enc_cfg_set_s32(cfg, "rc:bps_max", args.bps_max ? args.bps_max : args.bps * 17 / 16);
        ret = mpp_enc_cfg_set_s32(cfg, "rc:bps_min", args.bps_min ? args.bps_min : args.bps * 15 / 16);
    }
    break;
    case MPP_ENC_RC_MODE_VBR:
    case MPP_ENC_RC_MODE_AVBR:
    {
        /* VBR mode has wide bound */
        ret = mpp_enc_cfg_set_s32(cfg, "rc:bps_max", args.bps_max ? args.bps_max : args.bps * 17 / 16);
        ret = mpp_enc_cfg_set_s32(cfg, "rc:bps_min", args.bps_min ? args.bps_min : args.bps * 3 / 4);
    }
    break;
    default:
    {
        /* default use CBR mode */
        ret = mpp_enc_cfg_set_s32(cfg, "rc:bps_max", args.bps_max ? args.bps_max : args.bps * 17 / 16);
        ret = mpp_enc_cfg_set_s32(cfg, "rc:bps_min", args.bps_min ? args.bps_min : args.bps * 15 / 16);
    }
    break;
    }
    /* setup qp for different codec and rc_mode */
    switch (args.type)
    {
    case MPP_VIDEO_CodingAVC:
    case MPP_VIDEO_CodingHEVC:
    {
        switch (args.rc_mode)
        {
        case MPP_ENC_RC_MODE_FIXQP:
        {
            RK_S32 fix_qp = args.qp_init;

            ret = mpp_enc_cfg_set_s32(cfg, "rc:qp_init", fix_qp);
            ret = mpp_enc_cfg_set_s32(cfg, "rc:qp_max", fix_qp);
            ret = mpp_enc_cfg_set_s32(cfg, "rc:qp_min", fix_qp);
            ret = mpp_enc_cfg_set_s32(cfg, "rc:qp_max_i", fix_qp);
            ret = mpp_enc_cfg_set_s32(cfg, "rc:qp_min_i", fix_qp);
            ret = mpp_enc_cfg_set_s32(cfg, "rc:qp_ip", 0);
        }
        break;
        case MPP_ENC_RC_MODE_CBR:
        case MPP_ENC_RC_MODE_VBR:
        case MPP_ENC_RC_MODE_AVBR:
        {
            ret = mpp_enc_cfg_set_s32(cfg, "rc:qp_init", -1);
            ret = mpp_enc_cfg_set_s32(cfg, "rc:qp_max", 50);
            ret = mpp_enc_cfg_set_s32(cfg, "rc:qp_min", 10);
            ret = mpp_enc_cfg_set_s32(cfg, "rc:qp_max_i", 50);
            ret = mpp_enc_cfg_set_s32(cfg, "rc:qp_min_i", 10);
            ret = mpp_enc_cfg_set_s32(cfg, "rc:qp_ip", 2);
        }
        break;
        default:
        {
            printf("unsupport encoder rc mode %d\n", args.rc_mode);
        }
        break;
        }
    }
    break;
    case MPP_VIDEO_CodingVP8:
    {
        /* vp8 only setup base qp range */
        ret = mpp_enc_cfg_set_s32(cfg, "rc:qp_init", 40);
        ret = mpp_enc_cfg_set_s32(cfg, "rc:qp_max", 127);
        ret = mpp_enc_cfg_set_s32(cfg, "rc:qp_min", 0);
        ret = mpp_enc_cfg_set_s32(cfg, "rc:qp_max_i", 127);
        ret = mpp_enc_cfg_set_s32(cfg, "rc:qp_min_i", 0);
        ret = mpp_enc_cfg_set_s32(cfg, "rc:qp_ip", 6);
    }
    break;
    case MPP_VIDEO_CodingMJPEG:
    {
        /* jpeg use special codec config to control qtable */
        ret = mpp_enc_cfg_set_s32(cfg, "jpeg:q_factor", 80);
        ret = mpp_enc_cfg_set_s32(cfg, "jpeg:qf_max", 99);
        ret = mpp_enc_cfg_set_s32(cfg, "jpeg:qf_min", 1);
    }
    break;
    default:
    {
    }
    break;
    }

    ret = mpp_enc_cfg_set_s32(cfg, "codec:type", args.type);
    switch (args.type)
    {
    case MPP_VIDEO_CodingAVC:
    {
        RK_U32 constraint_set;

        /*
         * H.264 profile_idc parameter
         * 66  - Baseline profile
         * 77  - Main profile
         * 100 - High profile
         */
        ret = mpp_enc_cfg_set_s32(cfg, "h264:profile", 100);
        /*
         * H.264 level_idc parameter
         * 10 / 11 / 12 / 13    - qcif@15fps / cif@7.5fps / cif@15fps / cif@30fps
         * 20 / 21 / 22         - cif@30fps / half-D1@@25fps / D1@12.5fps
         * 30 / 31 / 32         - D1@25fps / 720p@30fps / 720p@60fps
         * 40 / 41 / 42         - 1080p@30fps / 1080p@30fps / 1080p@60fps
         * 50 / 51 / 52         - 4K@30fps
         */
        ret = mpp_enc_cfg_set_s32(cfg, "h264:level", 41);
        ret = mpp_enc_cfg_set_s32(cfg, "h264:cabac_en", 1);
        ret = mpp_enc_cfg_set_s32(cfg, "h264:cabac_idc", 0);
        ret = mpp_enc_cfg_set_s32(cfg, "h264:trans8x8", 1);
        os_get_env_u32("constraint_set", &constraint_set, 0);
        if (constraint_set & 0x3f0000)
            ret = mpp_enc_cfg_set_s32(cfg, "h264:constraint_set", constraint_set);
    }
    break;
    case MPP_VIDEO_CodingHEVC:
    case MPP_VIDEO_CodingMJPEG:
    case MPP_VIDEO_CodingVP8:
    {
    }
    break;
    default:
    {
        printf("unsupport encoder coding type %d\n", args.type);
    }
    break;
    }

    // mpp_env_get_u32("split_mode", &args.split_mode, MPP_ENC_SPLIT_NONE);
    // mpp_env_get_u32("split_arg", &args.split_arg, 0);
    // mpp_env_get_u32("split_out", &args.split_out, 0);
    args.split_mode = MPP_ENC_SPLIT_BY_BYTE;
    args.split_arg = 60000;
    args.split_out = MPP_ENC_SPLIT_OUT_LOWDELAY;
    if (args.split_mode)
    {
        printf("%p split mode %d arg %d out %d\n", ctx,
               args.split_mode, args.split_arg, args.split_out);
        mpp_enc_cfg_set_s32(cfg, "split:mode", args.split_mode);
        mpp_enc_cfg_set_s32(cfg, "split:arg", args.split_arg);
        mpp_enc_cfg_set_s32(cfg, "split:out", args.split_out);
    }
    ret = mpi->control(ctx, MPP_ENC_SET_CFG, cfg);
    if (ret)
    {
        printf("mpi control enc set cfg failed ret %d\n", ret);
        goto RET;
    }
    {
        RK_U32 sei_mode;
        mpp_env_get_u32("sei_mode", &sei_mode, MPP_ENC_SEI_MODE_DISABLE);
        args.sei_mode = (MppEncSeiMode)sei_mode;
        ret = mpi->control(ctx, MPP_ENC_SET_SEI_CFG, &args.sei_mode);
        if (ret)
        {
            printf("mpi control enc set sei cfg failed ret %d\n", ret);
            goto RET;
        }
    }
    if (args.type == MPP_VIDEO_CodingAVC || args.type == MPP_VIDEO_CodingHEVC)
    {
        args.header_mode = MPP_ENC_HEADER_MODE_EACH_IDR;
        ret = mpi->control(ctx, MPP_ENC_SET_HEADER_MODE, &args.header_mode);
        if (ret)
        {
            printf("mpi control enc set header mode failed ret %d\n", ret);
            goto RET;
        }
    }
    {
        RK_U32 gop_mode = (RK_U32)args.gop_mode;
        mpp_env_get_u32("gop_mode", &gop_mode, gop_mode);
        if (gop_mode)
        {
            MppEncRefCfg ref;
            mpp_enc_ref_cfg_init(&ref);

            if (args.gop_mode < 4)
                mpi_enc_gen_ref_cfg(ref, gop_mode);
            else
                mpi_enc_gen_smart_gop_ref_cfg(ref, args.gop_len, args.vi_len);

            ret = mpi->control(ctx, MPP_ENC_SET_REF_CFG, ref);
            if (ret)
            {
                printf("mpi control enc set ref cfg failed ret %d\n", ret);
                goto RET;
            }
            mpp_enc_ref_cfg_deinit(&ref);
        }
    }
    MppEncOSDPltCfg osd_plt_cfg;
    MppEncOSDPlt osd_plt;
    mpi_enc_gen_osd_plt(&osd_plt, 0);
    osd_plt_cfg.change = MPP_ENC_OSD_PLT_CFG_CHANGE_ALL;
    osd_plt_cfg.type = MPP_ENC_OSD_PLT_TYPE_USERDEF;
    osd_plt_cfg.plt = &osd_plt;
    ret = mpi->control(ctx, MPP_ENC_SET_OSD_PLT_CFG, &osd_plt_cfg);
    if (ret)
    {
        printf("mpi control enc set osd plt failed ret %d\n", ret);
        goto RET;
    }
RET:
    return ret;
}

void encoder::encode_commit_input_buf(int index, rga_buffer_t *rga_buf)
{
    // 如果rga_buf不为空，将rga_buf中图像复制到frame缓冲区中
    if (input_tid < 0)
        return;
    mCurrent_Enc_buf = &mEnc_buf[index];
    pthread_mutex_lock(&frame_lock);
    if (rga_buf)
    {
        imcopy(*rga_buf, mCurrent_Enc_buf->rga_buf);
    }
    pthread_cond_signal(&frame_cond);
    pthread_mutex_unlock(&frame_lock);
}

void encoder::deinit()
{
    if (ctx)
    {
        mpp_destroy(ctx);
        ctx = NULL;
    }
    if (cfg)
    {
        mpp_enc_cfg_deinit(cfg);
        cfg = NULL;
    }
    if (frame)
    {
        mpp_frame_deinit(&frame);
        frame = NULL;
    }
    if (output_tid >= 0)
    {
        pthread_cancel(output_tid);
    }

    for (int i = 0; i < 2; i++)
    {
        if (mEnc_buf[i].mpp_buf)
        {
            releasebuffer_handle(mEnc_buf[i].rga_buf_handle);
            mpp_buffer_put(mEnc_buf[i].mpp_buf);
        }
    }

    if (pkt_buf)
    {
        mpp_buffer_put(pkt_buf);
        pkt_buf = NULL;
    }
    if (buf_grp)
    {
        mpp_buffer_group_put(buf_grp);
        buf_grp = NULL;
    }
    printf("deinit finish\n");
}

void *encode_input_thread(void *arg)
{
    // 提交frame,修改OSD信息等
    // 条件变量
    encoder *encodec = (encoder *)arg;
    static int time_osd_counter = 0;
    for (;;)
    {
        // 修改OSD信息
        if (encodec->osd_infos_ptr.cam_mode_ptr)
        {

            if (encodec->isCN)
            {
                encoder::modify_osd_cn_mode(&encodec->osd_data, *encodec->osd_infos_ptr.cam_mode_ptr);
            }
            else
            {
                encoder::modify_osd_en_mode(&encodec->osd_data, *encodec->osd_infos_ptr.cam_mode_ptr);
            }
            encoder::modify_osd_gain_data(&encodec->osd_data, *encodec->osd_infos_ptr.gain_val_ptr);
            encoder::modify_osd_count_data(&encodec->osd_data, *encodec->osd_infos_ptr.count_val_ptr, *encodec->osd_infos_ptr.count_period_ptr);
            encoder::modify_osd_distance_data(&encodec->osd_data, *encodec->osd_infos_ptr.distance_val_ptr);
            encoder::modify_osd_T_and_H_data(&encodec->osd_data, *encodec->osd_infos_ptr.T_val_ptr, *encodec->osd_infos_ptr.H_val_ptr);
            if (encodec->osd_infos_ptr.gps_str_ptr)
            {
                encoder::modify_osd_gps_data(&encodec->osd_data, encodec->osd_infos_ptr.gps_str_ptr);
            }
            static int time_osd_counter = 0;
            if (++time_osd_counter >= 30)
            {
                encodec->draw_time_osd(&encodec->osd_data);
                time_osd_counter = 0;
            }
        }
        pthread_testcancel();
        pthread_mutex_lock(&encodec->frame_lock);
        pthread_cond_wait(&encodec->frame_cond, &encodec->frame_lock);
        encodec->encode_input();
        pthread_mutex_unlock(&encodec->frame_lock);
    }
    return nullptr;
}

void *encode_output_thread(void *arg)
{
    encoder *encodec = (encoder *)arg;
    MppPacket &packet = encodec->packet;
    char *ptr;
    size_t len;
    char header[40] = {};
    size_t header_len = 0;
    char pps[] = {0x00, 0x00, 0x00, 0x01, 0x68};
    RK_U32 eoi = 0;
    RK_U32 soi = 0;
    int header_cnt = 0;
    int pps_cnt;
    int sps_cnt;

    for (;;)
    {
        pthread_testcancel();
        pthread_mutex_lock(&encodec->packet_lock);
        while (!packet)
        {
            pthread_cond_wait(&encodec->packet_cond, &encodec->packet_lock);
        }

        do
        {
            if (encodec->mpi->encode_get_packet(encodec->ctx, &packet) < 0)
            {
                printf("encode get packet faied\n");
                msleep(1);
                continue;
            }
            if (packet)
            {
                // write packet to file here
                ptr = (char *)mpp_packet_get_pos(packet);
                len = mpp_packet_get_length(packet);
            }
            if (mpp_packet_is_partition(packet))
            {
                eoi = mpp_packet_is_eoi(packet);
                soi = mpp_packet_is_soi(packet);
            }
            else
            {
                eoi = 1;
                soi = 1;
            }
            if (len > 0)
            {
                encodec->write_func(ptr, len, ptr[4], eoi);

                if (encodec->m_record_state == record_start && soi)
                {
                    // 需添加sps和pps信息
                    // if (header_cnt > 0)
                    // {
                    //     encodec->write_func(header, header_len);
                    //     header_cnt--;
                    // }
                    if (ptr[4] == 0x67)
                    {
                        if (header_len == 0)
                        {
                            header_len += calculate_nal_len(ptr, len);              // sps
                            header_len += calculate_nal_len(ptr + header_len, len); // pps
                            // memcpy(header,ptr,header_len);
                        }
                        if (ptr[header_len + 4] == 0x65)
                        {
                            printf("开始录像\n");
                            encodec->m_record_state = record_doing;
                            if (encodec->m_fp)
                                fwrite(ptr, 1, len, encodec->m_fp);
                        }
                    }
                }
                else if (encodec->m_record_state == record_doing || encodec->m_record_state == record_end)
                {
                    if (encodec->m_fp)
                        fwrite(ptr, 1, len, encodec->m_fp);
                }
            }

        } while (!eoi);
        // encodec->write_func(frameAudInfo, VIDEO_FRAME_AUD_LEN);
        eoi = 0;
        ptr = NULL;
        len = 0;
        mpp_packet_deinit(&packet);

        if (encodec->m_record_state == record_end)
        {
            encodec->m_record_state = record_idle;
            fclose(encodec->m_fp);
            printf("停止录像\n");
            // TO DO:开线程转码成mp4
            pthread_cond_signal(&encodec->transcode_cond);
        }
        pthread_mutex_unlock(&encodec->packet_lock);
    }
    return NULL;
}

void *transcode_thread(void *arg)
{
    encoder *encodec = (encoder *)arg;
    char outPath[256] = {0};
    char audioPath[256] = {0};
    char ffmpegCmdStr[1024];
    FILE *fpCommand = NULL;
    for (;;)
    {
        pthread_mutex_lock(&encodec->packet_lock);
        pthread_cond_wait(&encodec->transcode_cond, &encodec->packet_lock);
        // sprintf(outPath, "/mnt/sdcard/IMG/%s", encodec->m_filepath + 17);
        strcpy(outPath, encodec->m_filepath);
        strcpy(outPath + strlen(outPath) - 4, "mp4");
        pthread_mutex_unlock(&encodec->packet_lock);

        // 推导音频路径: /mnt/sdcard/Standard Storage/M0001.h264 → /opt/voice/M0001.h264_clean.wav
        // 注意: this->buf 已被 cam->start_record() 内部 strcat 了 ".h264"，所以 baseName 带 .h264
        const char *lastSlash = strrchr(encodec->m_filepath, '/');
        const char *baseName = lastSlash ? lastSlash + 1 : encodec->m_filepath;

        // 优先用降噪后的音频，没有就用原始音频
        snprintf(audioPath, sizeof(audioPath), "/opt/voice/%s_clean.wav", baseName);
        if (access(audioPath, F_OK) != 0)
            snprintf(audioPath, sizeof(audioPath), "/opt/voice/%s.wav", baseName);

        if (access(audioPath, F_OK) == 0)
        {
            printf("Merging audio: %s\n", audioPath);
            snprintf(ffmpegCmdStr, sizeof(ffmpegCmdStr),
                     "echo \"y\" | ffmpeg -i \"%s\" -i \"%s\" -c:v copy -c:a aac -map 0:v:0 -map 1:a:0 -shortest -f \"mp4\" \"%s\" 1>/dev/null 2>&1",
                     encodec->m_filepath, audioPath, outPath);
        }
        else
        {
            printf("No audio file found, video only\n");
            snprintf(ffmpegCmdStr, sizeof(ffmpegCmdStr),
                     "echo \"y\" | ffmpeg -i \"%s\" -codec copy -f \"mp4\" \"%s\" 1>/dev/null 2>&1",
                     encodec->m_filepath, outPath);
        }

        fpCommand = popen(ffmpegCmdStr, "r");
        if (fpCommand == NULL)
        {
            printf("execute transcode command fail.");
        }
        int ffmpegRet = pclose(fpCommand);

        // 合并成功后删除音频文件
        if (ffmpegRet == 0 && access(audioPath, F_OK) == 0)
        {
            char rawWav[256], cleanWav[256];
            snprintf(rawWav, sizeof(rawWav), "/opt/voice/%s.wav", baseName);
            snprintf(cleanWav, sizeof(cleanWav), "/opt/voice/%s_clean.wav", baseName);
            printf("Removing audio files: %s, %s\n", rawWav, cleanWav);
            remove(rawWav);
            remove(cleanWav);
        }
    }
}
void encoder::launch(pOutput_func p, osd_infos *osd_infos_ptr)
{
    if (write_func != NULL)
        return;
    if (p)
        write_func = p;
    if (osd_infos_ptr)
    {
        this->osd_infos_ptr = *osd_infos_ptr;
    }
    pthread_create(&input_tid, 0, encode_input_thread, this);
    pthread_create(&output_tid, 0, encode_output_thread, this);
    pthread_create(&transcode_tid, 0, transcode_thread, this);
}
MPP_RET encoder::encode_input()
{
    MPP_RET ret = MPP_NOK;
    MppMeta meta = NULL;

    if (pthread_mutex_trylock(&packet_lock))
    {
        printf("encode input lock failed\n");
        return ret;
    }
    // pthread_mutex_lock(&packet_lock);
    if (packet)
    {
        printf("last packet has not processed!\n");
        pthread_mutex_unlock(&packet_lock);
        return ret;
    }
    if (!frame)
    {
        printf("frame wasn\'t initialized correctly!\n");
        pthread_mutex_unlock(&packet_lock);
        return ret;
    }

    // mpp_frame_set_buffer(frame, mEnc_buf.mpp_buf);
    mpp_frame_set_buffer(frame, mCurrent_Enc_buf->mpp_buf);
    mpp_frame_set_eos(frame, eos_flag);
    meta = mpp_frame_get_meta(frame);
    mpp_packet_init_with_buffer(&packet, pkt_buf);
    mpp_packet_set_length(packet, 0);
    mpp_meta_set_packet(meta, KEY_OUTPUT_PACKET, packet);
    mpp_meta_set_ptr(meta, KEY_OSD_DATA, (void *)&osd_data);
    ret = mpi->encode_put_frame(ctx, frame);
    if (ret)
    {
        printf("encode put frame failed\n");
        pthread_mutex_unlock(&packet_lock);
        return ret;
    }
    pthread_cond_signal(&packet_cond);
    pthread_mutex_unlock(&packet_lock);
    return ret;
}
MPP_RET encoder::encode(HyBirdBuffer *input_CamBuffer)
{
    MPP_RET ret = MPP_OK;
    RK_U32 eoi = 1;
    MppMeta meta = NULL;

    if (!frame)
    {
        printf("frame wasn\'t initialized correctly!\n");
        return MPP_NOK;
    }

    // mpp_frame_set_buffer(frame, mEnc_buf.mpp_buf);
    mpp_frame_set_buffer(frame, mCurrent_Enc_buf->mpp_buf);
    mpp_frame_set_eos(frame, eos_flag);
    meta = mpp_frame_get_meta(frame);
    mpp_packet_init_with_buffer(&packet, pkt_buf);
    mpp_packet_set_length(packet, 0);
    mpp_meta_set_packet(meta, KEY_OUTPUT_PACKET, packet);
    ret = mpi->encode_put_frame(ctx, frame);
    if (ret)
    {
        printf("encode put frame failed\n");
        goto RET;
    }

    do
    {
        ret = mpi->encode_get_packet(ctx, &packet);
        if (ret)
        {
            printf("encode get packet faied\n");
            goto RET;
        }
        if (packet)
        {
            // write packet to file here
            void *ptr = mpp_packet_get_pos(packet);
            size_t len = mpp_packet_get_length(packet);
            // write_func(ptr, len);
        }
        if (mpp_packet_is_partition(packet))
        {
            eoi = mpp_packet_is_eoi(packet);
            printf("成功进入\n");
        }
    } while (!eoi);
    // write_func(frameAudInfo,VIDEO_FRAME_AUD_LEN);
RET:
    if (packet)
    {
        mpp_packet_deinit(&packet);
    }
    return ret;
}
size_t encoder::calc_buf_size(MppFrameFormat fmt)
{
    size_t size;
    RK_S32 hor_stride = mpi_enc_width_default_stride(args.width, fmt);
    switch (fmt & MPP_FRAME_FMT_MASK)
    {
    case MPP_FMT_YUV420SP:
    case MPP_FMT_YUV420P:
    {
        size = MPP_ALIGN(hor_stride, 64) * MPP_ALIGN(args.ver_stride, 64) * 3 / 2;
    }
    break;

    case MPP_FMT_YUV422_YUYV:
    case MPP_FMT_YUV422_YVYU:
    case MPP_FMT_YUV422_UYVY:
    case MPP_FMT_YUV422_VYUY:
    case MPP_FMT_YUV422P:
    case MPP_FMT_YUV422SP:
    {
        size = MPP_ALIGN(hor_stride, 64) * MPP_ALIGN(args.ver_stride, 64) * 2;
    }
    break;
    case MPP_FMT_RGB444:
    case MPP_FMT_BGR444:
    case MPP_FMT_RGB555:
    case MPP_FMT_BGR555:
    case MPP_FMT_RGB565:
    case MPP_FMT_BGR565:
    case MPP_FMT_RGB888:
    case MPP_FMT_BGR888:
    case MPP_FMT_RGB101010:
    case MPP_FMT_BGR101010:
    case MPP_FMT_ARGB8888:
    case MPP_FMT_ABGR8888:
    case MPP_FMT_BGRA8888:
    case MPP_FMT_RGBA8888:
    {
        size = MPP_ALIGN(hor_stride, 64) * MPP_ALIGN(args.ver_stride, 64);
    }
    break;

    default:
    {
        size = MPP_ALIGN(hor_stride, 64) * MPP_ALIGN(args.ver_stride, 64) * 4;
    }
    break;
    }
    return size;
}

bool encoder::start_record(char *save_path)
{
    if (m_record_state != record_idle)
        return false;
    if (ctx == NULL || mpi == NULL)
    {
        printf("encoder not properly initialized\n");
        return false;
    }
    m_fp = fopen(save_path, "w+b");
    if (m_fp == NULL)
    {
        printf("failed to open file for recording: %s\n", save_path);
        return false;
    }
    memcpy(m_filepath, save_path, 256);
    pthread_mutex_lock(&packet_lock);
    mpi->control(ctx, MPP_ENC_SET_IDR_FRAME, NULL);
    m_record_state = record_start;
    pthread_mutex_unlock(&packet_lock);
    return true;
}
void encoder::stop_record()
{
    if (m_record_state == record_doing)
    {
        pthread_mutex_lock(&packet_lock);
        m_record_state = record_end;
        pthread_mutex_unlock(&packet_lock);
    }
}

RK_S32 os_get_env_u32(const char *name, RK_U32 *value, RK_U32 default_value)
{
    char *ptr = getenv(name);
    if (NULL == ptr)
    {
        *value = default_value;
    }
    else
    {
        char *endptr;
        int base = (ptr[0] == '0' && ptr[1] == 'x') ? (16) : (10);
        errno = 0;
        *value = strtoul(ptr, &endptr, base);
        if (errno || (ptr == endptr))
        {
            errno = 0;
            *value = default_value;
        }
    }
    return 0;
}

static int os_malloc(void **memptr, size_t alignment, size_t size)
{
    return posix_memalign(memptr, alignment, size);
}

void *mpp_osal_malloc(size_t size)
{
    size_t size_align = MEM_ALIGNED(size);
    size_t size_real = size_align;
    void *ptr;

    os_malloc(&ptr, MEM_ALIGN, size_real);
    return ptr;
}

void *mpp_osal_calloc(size_t size)
{
    void *ptr = mpp_osal_malloc(size);
    if (ptr)
        memset(ptr, 0, size);
    return ptr;
}

void encoder::modify_osd_cn_mode(MppEncOSDData *osd_data, unsigned int mode)
{
    MppEncOSDRegion *region = osd_data->region;
    char *ptr = (char *)mpp_buffer_get_ptr(osd_data->buf);
    
    int buf_width = region->num_mb_x * 16;
    int buf_height = region->num_mb_y * 16;
    int buf_stride = buf_width;
    unsigned char color = 1;
    ptr += buf_width * (buf_height / 2);
    const char *mode_text = "";
    switch (mode)
    {
    case VL_Mode:
        mode_text = "可见";
        break;
    case UV_Mode:
        mode_text = "紫外";
        break;
    case VL_UV_blendMode:
        mode_text = "融合";
        break;
    case IR_Mode:
        mode_text = "红外";
        break;
    case IR_UV_blendMode:
        mode_text = "融合";
        break;  
    default:
        break;
    }
    clear_area((unsigned char *)ptr, buf_width, buf_height / 2+2, 32*3, 0, 160, buf_height / 2+2, buf_stride);
    g_ftosd.render_text(mode_text, (unsigned char *)ptr, buf_width, buf_height / 2+2, buf_stride, 32*3, 32, color);
}

void encoder::modify_osd_en_mode(MppEncOSDData *osd_data, unsigned int mode)
{
    MppEncOSDRegion *region = osd_data->region;
    char *ptr = (char *)mpp_buffer_get_ptr(osd_data->buf);
    
    int buf_width = region->num_mb_x * 16;
    int buf_height = region->num_mb_y * 16;
    int buf_stride = buf_width;
    unsigned char color = 1;
    
    ptr += buf_width * (buf_height / 2);
    const char *mode_text = "";
    switch (mode)
    {
    case VL_Mode:
        mode_text = "VL";
        break;
    case UV_Mode:
        mode_text = "UV";
        break;
    case VL_UV_blendMode:
        mode_text = "Fusion";
        break;
    case IR_Mode:
        mode_text = "IR";
        break;
    case IR_UV_blendMode:
        mode_text = "Fusion";
        break;
    default:
        break;
    }
    clear_area((unsigned char *)ptr, buf_width, buf_height / 2+2, 32*3, 0, 160, buf_height / 2+2, buf_stride);
    g_ftosd.render_text(mode_text, (unsigned char *)ptr, buf_width, buf_height / 2, buf_stride, 32*3, 32, color);
}

void encoder::modify_osd_gain_data(MppEncOSDData *osd_data, unsigned int value)
{
    MppBuffer buf = osd_data->buf;
    MppEncOSDRegion *region = osd_data->region;
    char *ptr = (char *)mpp_buffer_get_ptr(buf);
    
    int buf_width = region->num_mb_x * 16;
    int buf_height = region->num_mb_y * 16;
    int buf_stride = buf_width;
    unsigned char color = 1;
    
    char gain_text[32];
    snprintf(gain_text, sizeof(gain_text), "%u%%", value);
    if (isCN)
    {
        clear_area((unsigned char *)ptr, buf_width, buf_height / 2, 32*3, 0, 160, buf_height / 2, buf_stride);
        g_ftosd.render_text(gain_text, (unsigned char *)ptr, buf_width, buf_height / 2, buf_stride, 32*3, 32, color);
    }
    else
    {
        clear_area((unsigned char *)ptr, buf_width, buf_height / 2, 32*3-10, 0, 160, buf_height / 2, buf_stride);
        g_ftosd.render_text(gain_text, (unsigned char *)ptr, buf_width, buf_height / 2, buf_stride, 32*3-10, 32, color);
    }

}

void encoder::modify_osd_count_data(MppEncOSDData *osd_data, unsigned int value, bool period)
{
    MppEncOSDRegion *region = osd_data->region;
    char *ptr = (char *)mpp_buffer_get_ptr(osd_data->buf);
    
    if (value > 999999)
        value = 999999;
    
    int buf_width = region->num_mb_x * 16;
    int buf_height = region->num_mb_y * 16;
    int buf_stride = buf_width;
    unsigned char color = 1;
    
    char count_text[32];
    snprintf(count_text, sizeof(count_text), "%d/%s", value, period ? "S" : "M");
    clear_area((unsigned char *)ptr, buf_width, buf_height / 2, 580+32*3, 0, 160, buf_height / 2, buf_stride);
    g_ftosd.render_text(count_text, (unsigned char *)ptr, buf_width, buf_height / 2, buf_stride, 580+32*3, 32, color);
}

void encoder::modify_osd_distance_data(MppEncOSDData *osd_data, unsigned int value)
{
    MppEncOSDRegion *region = osd_data->region;
    char *ptr = (char *)mpp_buffer_get_ptr(osd_data->buf);
    
    int buf_width = region->num_mb_x * 16;
    int buf_height = region->num_mb_y * 16;
    int buf_stride = buf_width;
    unsigned char color = 1;
    
    char dist_text[32];
    snprintf(dist_text, sizeof(dist_text), "%.1fm", value / 10.0);
    if(isCN)
    {
        clear_area((unsigned char *)ptr, buf_width, buf_height / 2, 300+32*3, 0, 160, buf_height / 2, buf_stride);
        g_ftosd.render_text(dist_text, (unsigned char *)ptr, buf_width, buf_height / 2, buf_stride, 300+32*3, 32, color);
    }
    else
    {
        clear_area((unsigned char *)ptr, buf_width, buf_height / 2, 300+32*3-20, 0, 160, buf_height / 2, buf_stride);
        g_ftosd.render_text(dist_text, (unsigned char *)ptr, buf_width, buf_height / 2, buf_stride, 300+32*3-20, 32, color);
    }
}

void encoder::draw_time_osd(MppEncOSDData *osd_data)
{
    MppEncOSDRegion *region = osd_data->region;
    char *ptr = (char *)mpp_buffer_get_ptr(osd_data->buf);
    if (!ptr)
    {
        printf("Failed to get buffer pointer");
        return; 
    }
    int buf_width = region->num_mb_x * 16;
    int buf_height = region->num_mb_y * 16;
    int buf_stride = buf_width;
    unsigned char color = 1;
    ptr += buf_width * (buf_height / 2);
    
    // time_t t = time(NULL);
    // struct tm now_time;
    // localtime_r(&t, &now_time);         // 填充 now_time 结构体
    // int year = now_time.tm_year + 1900; // 年份需 +1900
    // int month = now_time.tm_mon + 1;    // 月份从 0 开始，需 +1
    // int day = now_time.tm_mday;
    // int hour = now_time.tm_hour;
    // int minute = now_time.tm_min;
    // int second = now_time.tm_sec;
    // 直接读 RTC 硬件时钟，避免时区偏差（和屏幕显示保持一致）
    int year = 2026, month = 1, day = 1, hour = 0, minute = 0, second = 0;
    FILE *fp = fopen("/sys/class/rtc/rtc0/date", "r");
    if (fp) {
        fscanf(fp, "%d-%d-%d", &year, &month, &day);
        fclose(fp);
    }
    fp = fopen("/sys/class/rtc/rtc0/time", "r");
    if (fp) {
        fscanf(fp, "%d:%d:%d", &hour, &minute, &second);
        fclose(fp);
    }

    char time_text[32];
    snprintf(time_text, sizeof(time_text), "%04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute, second);
    clear_area((unsigned char *)ptr, buf_width, buf_height / 2, 600, 0, buf_width - 600, buf_height / 2, buf_stride);
    g_ftosd.render_text(time_text, (unsigned char *)ptr, buf_width, buf_height / 2, buf_stride, 600, 32, color);
}
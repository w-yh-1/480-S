#ifndef __ENCODER_H__
#define __ENCODER_H__
#include <stdio.h>
#include "rk_mpi.h"
#include "rk_venc_rc.h"
#include "mpp_rc_api.h"
#include "camera_source.h"
#include <pthread.h>
#include <QDebug>
#define CODEC_TYPE MPP_VIDEO_CodingAVC
#define MPP_ALIGN(x, a) (((x) + (a) - 1) & ~((a) - 1))
#define mpp_env_get_u32(name, value, default_value) os_get_env_u32(name, value, default_value)
enum CamMode
{
    VL_Mode=0,
    VL_UV_blendMode,
    UV_Mode,
    IR_Mode,
    IR_UV_blendMode,
    UNINITIALIZED
};
typedef struct osd_infos_t
{
    unsigned int *cam_mode_ptr;
    unsigned int *gain_val_ptr;
    unsigned int *count_val_ptr;
    unsigned int *first_val_ptr;
    unsigned int *second_val_ptr;
    unsigned int *third_val_ptr;
    unsigned int *fourth_val_ptr;
    unsigned int *count_period_ptr;
    unsigned int *distance_val_ptr;
    unsigned int *T_val_ptr;
    unsigned int *H_val_ptr;
    char *gps_str_ptr;
}osd_infos;

typedef struct HyBirdBuffer_t
{
    MppBuffer mpp_buf;
    void *ptr{NULL};
    int fd{-1};
    rga_buffer_handle_t rga_buf_handle{0};
    rga_buffer_t rga_buf{0};
} HyBirdBuffer;

typedef struct EncodeArgs_t
{
    RK_U32 width;
    RK_U32 height;
    RK_U32 hor_stride;
    RK_U32 ver_stride;
    RK_S32 fps_in_flex;
    RK_S32 fps_in_den;
    RK_S32 fps_in_num;
    RK_S32 fps_out_flex;
    RK_S32 fps_out_den;
    RK_S32 fps_out_num;

    RK_S32 bps;
    RK_S32 bps_max;
    RK_S32 bps_min;

    size_t frame_size;
    size_t header_size;
    MppFrameFormat fmt;
    MppCodingType type;
    MppEncRcMode rc_mode;
    RK_S32 qp_init;

    GopMode gop_mode;
    RK_S32 gop_len;
    RK_S32 vi_len;

    MppEncSeiMode sei_mode;
    MppEncHeaderMode header_mode;

    RK_U32 split_mode;
    RK_U32 split_arg;
    RK_U32 split_out;

    RK_U32 refresh_en;
    MppEncRcRefreshMode refresh_mode;
    RK_U32 refresh_num;
    RK_U32 frm_eos;
} EncodeArgs;

enum record_state
{
    record_start,
    record_doing,
    record_end,
    record_idle
};

typedef void (*pOutput_func)(void *, size_t,uint8_t,bool);

class encoder
{
private:
    /* data */
    MppBufferGroup buf_grp;

    MppPacket packet = NULL;
    MppBuffer pkt_buf;
    MppEncCfg cfg;
    MppCtx ctx;
    MppApi *mpi;
    EncodeArgs args;
    MppPollType timeout = MPP_POLL_NON_BLOCK;
    pOutput_func write_func = NULL;

    size_t ext_buf_size = 0;
    MppFrameFormat ext_buf_fmt;
    pthread_t input_tid = -1;
    pthread_t output_tid = -1;
    pthread_t transcode_tid = -1;
    RK_U32 eos_flag = 0;
    bool bIsRunning = 0;
    pthread_mutex_t frame_lock = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t frame_cond = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t packet_lock = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t packet_cond = PTHREAD_COND_INITIALIZER;
    pthread_cond_t transcode_cond = PTHREAD_COND_INITIALIZER;
    record_state m_record_state;
    char m_filepath[256];
    FILE *m_fp;
    size_t calc_buf_size(MppFrameFormat fmt);

public:
    osd_infos osd_infos_ptr = {};
    encoder(RK_U32 width, RK_U32 height, MppFrameFormat frame_fmt = MPP_FMT_BGRA8888, MppCodingType type = MPP_VIDEO_CodingAVC, MppEncRcMode rc_mode = MPP_ENC_RC_MODE_CBR);
    ~encoder();
    MPP_RET encode(HyBirdBuffer *input_CamBuffer);
    MPP_RET init();
    MPP_RET cfg_setup();
    MPP_RET encode_input();
    void encode_commit_input_buf(int index,rga_buffer_t *rga_buf=NULL);
    friend void *encode_input_thread(void *arg);
    friend void *encode_output_thread(void *arg);
    friend void *transcode_thread(void *arg);
    void deinit();
    void launch(pOutput_func p,osd_infos *osd_infos_ptr);
    void stop_record();
    void pause_record();
    bool start_record(char *save_path);
    HyBirdBuffer mEnc_buf[2];
    HyBirdBuffer *mCurrent_Enc_buf;
    MppFrame frame = NULL;
    MppEncOSDData osd_data;
    rga_buffer_t *input_buf;
    static bool isCN;
    static MPP_RET gen_osd_data(MppEncOSDData *osd_data, MppBufferGroup group);
    static void switch_language(MppEncOSDData *osd_data, bool is_CN);
    static void modify_osd_cn_mode(MppEncOSDData *osd_data, unsigned int mode);
    static void modify_osd_en_mode(MppEncOSDData *osd_data, unsigned int mode);
    static void modify_osd_gain_data(MppEncOSDData *osd_data, unsigned int value);
    static void modify_osd_count_data(MppEncOSDData *osd_data, unsigned int value, bool period);
    static void modify_osd_distance_data(MppEncOSDData *osd_data, unsigned int value);
    static void modify_osd_T_and_H_data(MppEncOSDData *osd_data, unsigned int T_value, unsigned int H_value);
    static void modify_osd_gps_data(MppEncOSDData *osd_data, const char *gps_str);
    static void draw_cn_osd(MppEncOSDData *osd_data);
    static void draw_en_osd(MppEncOSDData *osd_data);
    void draw_time_osd(MppEncOSDData *osd_data);
};

#endif
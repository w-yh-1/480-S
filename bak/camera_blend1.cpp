#include "camera_blend.h"
#include "camera_source.h"
#include "select_delay.h"
#include "encoder.h"
#include <chrono>
CamMode tempMode = VL_UV_blendMode;
static void empty_structure(rga_buffer_t *src, rga_buffer_t *dst, rga_buffer_t *pat,
                            im_rect *srect, im_rect *drect, im_rect *prect, im_opt_t *opt);
IM_STATUS RGA_improcess(rga_buffer_t &src, rga_buffer_t &dst, im_rect sRect = {0}, im_rect dRect = {0}, int usage = 0);
IM_STATUS RGA_imcolorkey(const rga_buffer_t src, rga_buffer_t dst, im_rect srect, im_rect drect, im_colorkey_range range, int mode, int sync = 1, int *release_fence_fd = NULL);
IM_STATUS RGA_imcolorkey_3channel(const rga_buffer_t srcA,
                                  const rga_buffer_t srcB,
                                  rga_buffer_t dst,
                                  im_rect srectA,
                                  im_rect srectB,
                                  im_rect drect,
                                  im_colorkey_range range,
                                  int mode,
                                  int sync = 1,
                                  int *release_fence_fd = NULL);
// void camera_blend::change_SrcRect_by_zoomRatio()
// {
//     switch (mCurrentZoomRatio) // 960,540,1920,1080
//     {
//     case 5: // 1.33333  0.88888
//         mVlSrcRect.x = 480;
//         mVlSrcRect.y = 270;
//         mVlSrcRect.width = 3840 - 480 * 2;  // 2880
//         mVlSrcRect.height = 2160 - 270 * 2; // 1620

//         mUvSrcRect.x = 100;
//         mUvSrcRect.y = 56;
//         mUvSrcRect.width = 1280 - 100 * 2; //
//         mUvSrcRect.height = 720 - 56 * 2;
//         break;
//     case 10: // 1.5
//         mVlSrcRect.x = 640;
//         mVlSrcRect.y = 360;
//         mVlSrcRect.width = 2560;
//         mVlSrcRect.height = 1440;

//         mUvSrcRect.x = 160;
//         mUvSrcRect.y = 90;
//         mUvSrcRect.width = 960;
//         mUvSrcRect.height = 540;
//         break;
//     case 15: // 1.71428   1.14285
//         mVlSrcRect.x = 800;
//         mVlSrcRect.y = 450;
//         mVlSrcRect.width = 2240;
//         mVlSrcRect.height = 1260;

//         mUvSrcRect.x = 220;
//         mUvSrcRect.y = 124;
//         mUvSrcRect.width = 840;
//         mUvSrcRect.height = 472;
//         break;
//     case 20: // 2  1.33333
//         mVlSrcRect.x = 960;
//         mVlSrcRect.y = 540;
//         mVlSrcRect.width = 1920;
//         mVlSrcRect.height = 1080;

//         mUvSrcRect.x = 280;
//         mUvSrcRect.y = 158;
//         mUvSrcRect.width = 720;
//         mUvSrcRect.height = 404;
//         break;
//     case 25: // 2.4 1.6
//         mVlSrcRect.x = 1120;
//         mVlSrcRect.y = 630;
//         mVlSrcRect.width = 1600;
//         mVlSrcRect.height = 900;

//         mUvSrcRect.x = 340;
//         mUvSrcRect.y = 190;
//         mUvSrcRect.width = 600;
//         mUvSrcRect.height = 340;
//         break;
//     case 30: // 3 2
//         mVlSrcRect.x = 1280;
//         mVlSrcRect.y = 720;
//         mVlSrcRect.width = 1280;
//         mVlSrcRect.height = 720;

//         mUvSrcRect.x = 400;
//         mUvSrcRect.y = 224;
//         mUvSrcRect.width = 480;
//         mUvSrcRect.height = 272;
//         break;
//     case 35: // 4 2.66667
//         mVlSrcRect.x = 1440;
//         mVlSrcRect.y = 810;
//         mVlSrcRect.width = 960;
//         mVlSrcRect.height = 540;

//         mUvSrcRect.x = 460;
//         mUvSrcRect.y = 260;
//         mUvSrcRect.width = 360;
//         mUvSrcRect.height = 200;
//         break;
//     default:
//         break;
//     }
// }

void camera_blend::change_SrcRect_by_zoomRatio()
{
    switch (mCurrentZoomRatio) // 960,540,1920,1080
    {
    case 5: // 1.33333  0.88888
        mVlSrcRect.x = 480;
        mVlSrcRect.y = 270;
        mVlSrcRect.width = 3840 - 480 * 2;  // 2880
        mVlSrcRect.height = 2160 - 270 * 2; // 1620

        mUvSrcRect.x = 100+x_offset;
        mUvSrcRect.y = 56+y_offset;
        mUvSrcRect.width = 1280 - 100 * 2; //
        mUvSrcRect.height = 720 - 56 * 2;
        break;
    case 10: // 1.5
        mVlSrcRect.x = 640;
        mVlSrcRect.y = 360;
        mVlSrcRect.width = 2560;
        mVlSrcRect.height = 1440;

        mUvSrcRect.x = 160+x_offset*1.12;
        mUvSrcRect.y = 90+y_offset*1.1;
        mUvSrcRect.width = 960;
        mUvSrcRect.height = 540;
        break;
    case 15: // 1.71428   1.14285
        mVlSrcRect.x = 800;
        mVlSrcRect.y = 450;
        mVlSrcRect.width = 2240;
        mVlSrcRect.height = 1260;

        mUvSrcRect.x = 220+x_offset*1.1;
        mUvSrcRect.y = 124+y_offset*1.0;
        mUvSrcRect.width = 840;
        mUvSrcRect.height = 472;
        break;
    case 20: // 2  1.33333
        mVlSrcRect.x = 960;
        mVlSrcRect.y = 540;
        mVlSrcRect.width = 1920;
        mVlSrcRect.height = 1080;

        mUvSrcRect.x = 280+x_offset*1.1;
        mUvSrcRect.y = 158+y_offset*1.0;
        mUvSrcRect.width = 720;
        mUvSrcRect.height = 404;
        break;
    case 25: // 2.4 1.6
        mVlSrcRect.x = 1120;
        mVlSrcRect.y = 630;
        mVlSrcRect.width = 1600;
        mVlSrcRect.height = 900;

        mUvSrcRect.x = 340+x_offset*1.1;
        mUvSrcRect.y = 190+y_offset*1.0;
        mUvSrcRect.width = 600;
        mUvSrcRect.height = 340;
        break;
    case 30: // 3 2
        mVlSrcRect.x = 1280;
        mVlSrcRect.y = 720;
        mVlSrcRect.width = 1280;
        mVlSrcRect.height = 720;

        mUvSrcRect.x = 400+x_offset*1.1;
        mUvSrcRect.y = 224+y_offset*1.0;
        mUvSrcRect.width = 480;
        mUvSrcRect.height = 272;
        break;
    case 35: // 4 2.66667
        mVlSrcRect.x = 1440;
        mVlSrcRect.y = 810;
        mVlSrcRect.width = 960;
        mVlSrcRect.height = 540;

        mUvSrcRect.x = 460+x_offset*1.1;
        mUvSrcRect.y = 260+y_offset*1.0;
        mUvSrcRect.width = 360;
        mUvSrcRect.height = 200;
        break;
    default:
        break;
    }
}
bool camera_blend::camBlendThreadStart(DrmDisplay *displayer,
                                       pOutput_func output_func,
                                       unsigned int *gain_val_ptr,
                                       unsigned int *count_val_ptr,
                                       unsigned int *count_period_ptr,
                                       unsigned int *distance_val_ptr)
{

    int ret = 0;
    // init camera source

    mDisplayer = displayer;

    // init camera source
    mCamSourceVL = camera_source_init(DEV_VL, 4, 3840, 2160, MPP_FMT_YUV420SP);
    if (mCamSourceVL == NULL)
        return false;

    mCamSourceUV = camera_source_init(DEV_UV, 4, 1280, 720, MPP_FMT_BGR888);

    if (mCamSourceUV == NULL)
        return false;

    // init encoder
    do
    {
        enc = new encoder(1920, 1080);
        ret = enc->init();
        if (ret != MPP_OK)
        {
            printf("failed to init encoder\n");
            break;
        }
        ret = enc->cfg_setup();
        if (ret != MPP_OK)
        {
            printf("failed to cfg encoder\n");
            break;
        }
        enc->launch(output_func, gain_val_ptr, count_val_ptr, count_period_ptr, distance_val_ptr);
    } while (0);

    // init drm buffer
    ret = get_drm_buffer(&mFrameBuf0, 1088, 1920, RK_FORMAT_BGR_888);
    if (ret < 0)
    {
        printf("failed to get drm buf0\n");
        return false;
    }

    ret = get_drm_buffer(&mFrameBuf1, 1088, 1920, RK_FORMAT_BGR_888);
    if (ret < 0)
    {
        printf("failed to get drm buf1\n");
        return false;
    }

    ret = get_drm_buffer(&mVlTempBuf, 1920, 1080, RK_FORMAT_BGRA_8888);
    if (ret < 0)
    {
        printf("failed to get drm buf mVlTempBuf\n");
        return false;
    }
    // mVlTempBuf.rga_buf = enc->mEnc_buf.rga_buf;


    ret = get_drm_buffer(&mTempShootBuf, 1920, 1080, RK_FORMAT_RGB_888);
    if (ret < 0)
    {
        printf("failed to get drm buf mTempShootBuf\n");
        return false;
    }

    ret = get_dma_buf(&mPhotonsBuf, 1280, 720, RK_FORMAT_BGRA_8888);
    if (ret < 0)
    {
        printf("failed to get dma buf mPhotonsBuf\n");
        return false;
    }

    mBo0 = mDisplayer->createSpBoFromDrmBuf(&mFrameBuf0, DRM_FORMAT_BGR888);
    mBo1 = mDisplayer->createSpBoFromDrmBuf(&mFrameBuf1, DRM_FORMAT_BGR888);
    ret = pthread_create(&mthread, NULL, camBlendTask, this);
    if (ret < 0)
        return false;
    pthread_detach(mthread);

    return true;
}
// static int i =0;
// void *camera_blend::camBlendTask(void *args)
// {
//     camera_blend *p = (camera_blend *)args;
//     int vl_frame_id;
//     int uv_frame_id;

//     int ret;
//     u_int32_t index = 0;
//     sp_bo *bo = NULL;
//     im_colorkey_range white_range;
//     im_colorkey_range black_range;
//     white_range.max = 0xFFFFFFFF; // ABGR
//     white_range.min = 0xFF000001;
//     black_range.max = 0xFF000001;
//     black_range.min = 0xFF000000;
//     CamFrame *uv_frame_ptr = NULL;
//     DELAY_INIT;
//     while (1)
//     {
//         //DELAY_US(16666, 500); // 16.666ms 误差0.5ms
//         DELAY_US(30000, 500);
//         if (p->mCurrenMode < 2)
//         {
//             vl_frame_id = camera_source_get_frame(p->mCamSourceVL);
//             if (vl_frame_id < 0)
//             {
//                 printf("VL camera failed to get frame\n");
//                 continue;
//             }
//             RGA_improcess(*camera_frame_to_RgaBuffer(p->mCamSourceVL, vl_frame_id), p->mVlTempBuf.rga_buf, p->mVlSrcRect);
//             camera_source_put_frame(p->mCamSourceVL, vl_frame_id);

//         }
//         if (p->mCurrenMode > 0)
//         {
//             // 获取紫外图像
//             uv_frame_id = camera_source_get_frame(p->mCamSourceUV);
//             if (uv_frame_id < 0)
//             {
//                 printf("UV camera failed to get frame\n");
//                 continue;
//             }
//             camera_source_put_frame(p->mCamSourceUV, uv_frame_id);
//             if (p->mCurrenMode == 1)
//             {
//             // 重新获取最新的紫外图像，减少拖尾
//             uv_frame_id = camera_source_get_frame(p->mCamSourceUV);
//             if (uv_frame_id < 0)
//             {
//                 printf("UV camera failed to get frame\n");
//                 continue;
//             }

//                 if (p->mColor != 0xFFFFFFFF)
//                 {
//                     RGA_imcolorkey(*camera_frame_to_RgaBuffer(p->mCamSourceUV, uv_frame_id),
//                                    p->mPhotonsBuf.rga_buf,
//                                    p->mUvSrcRect,
//                                    {0},
//                                    white_range,
//                                    IM_ALPHA_COLORKEY_NORMAL);
//                     camera_source_put_frame(p->mCamSourceUV, uv_frame_id);
//                     RGA_imcolorkey(p->mPhotonsBuf.rga_buf, p->mVlTempBuf.rga_buf,
//                                    {0},
//                                    {0},
//                                    black_range,
//                                    IM_ALPHA_COLORKEY_NORMAL);
//                 }
//                 else
//                 {
//                     RGA_imcolorkey(*camera_frame_to_RgaBuffer(p->mCamSourceUV, uv_frame_id),
//                                    p->mVlTempBuf.rga_buf,
//                                    p->mUvSrcRect,
//                                    {0},
//                                    black_range,
//                                    IM_ALPHA_COLORKEY_NORMAL);
//                     camera_source_put_frame(p->mCamSourceUV, uv_frame_id);
//                 }
//             }
//             else
//             {
//                 RGA_improcess(*camera_frame_to_RgaBuffer(p->mCamSourceUV, uv_frame_id), p->mVlTempBuf.rga_buf, p->mUvSrcRect);
//                 camera_source_put_frame(p->mCamSourceUV, uv_frame_id);
//             }
//         }
//         // 如果处于拍照预览状态则不更新图像显示，否则更新图像显示
//         if (p->isNewFrameReady == false)
//         {
//             // 交换缓冲区
//             if (index++ % 2 == 0)
//             {
//                 RGA_improcess(p->mVlTempBuf.rga_buf, p->mFrameBuf0.rga_buf, {0}, {0}, IM_HAL_TRANSFORM_ROT_90);
//                 bo = p->mBo0;
//             }
//             else
//             {
//                 RGA_improcess(p->mVlTempBuf.rga_buf, p->mFrameBuf1.rga_buf, {0}, {0}, IM_HAL_TRANSFORM_ROT_90);
//                 bo = p->mBo1;
//             }
//             // 显示图像
//             p->mDisplayer->display(bo);
//         }
//         // 录像喂数据
//         p->enc->encode_commit_input_buf();
//         // 更新缩放倍率
//         p->change_SrcRect_by_zoomRatio();
//         // 刷新光子颜色掩模图
//         if (p->mColor != 0xFFFFFFFF)
//             imfill(p->mPhotonsBuf.rga_buf, {0}, p->mColor);
//         // 处理拍照请求
//         if (p->isRequireNewFrame)
//         {
//             RGA_improcess(p->mVlTempBuf.rga_buf, p->mTempShootBuf.rga_buf);
//             p->isNewFrameReady = true;
//             p->isRequireNewFrame = false;
//         }
//         if(p->mCurrenMode!=tempMode)
//         {
//             p->mCurrenMode = tempMode;
//             //usleep(500*1000);
//         }
//     }

//     return nullptr;
// }

void *camera_blend::camBlendTask(void *args)
{
    camera_blend *p = (camera_blend *)args;
    int vl_frame_id;
    int uv_frame_id;

    int ret;
    u_int32_t index = 0;
    sp_bo *bo = NULL;
    im_colorkey_range white_range;
    im_colorkey_range black_range;
    white_range.max = 0xFFFFFFFF; // ABGR
    white_range.min = 0xFF000001;
    black_range.max = 0xFF000001;
    black_range.min = 0xFF000000;
    CamFrame *uv_frame_ptr = NULL;
    DELAY_INIT;
    while (1)
    {
        DELAY_US(16666, 1000); // 16.666ms 误差0.5ms
        switch (p->mCurrenMode)
        {
        case VL_Mode:
            vl_frame_id = camera_source_get_frame(p->mCamSourceVL);
            if (vl_frame_id < 0)
            {
                printf("VL camera failed to get frame\n");
                continue;
            }
            RGA_improcess(*camera_frame_to_RgaBuffer(p->mCamSourceVL, vl_frame_id), p->mVlTempBuf.rga_buf, p->mVlSrcRect);
            camera_source_put_frame(p->mCamSourceVL, vl_frame_id);
            break;
        case UV_Mode:
            uv_frame_id = camera_source_get_frame(p->mCamSourceUV);
            if (uv_frame_id < 0)
            {
                printf("UV camera failed to get frame\n");
                continue;
            }
            RGA_improcess(*camera_frame_to_RgaBuffer(p->mCamSourceUV, uv_frame_id), p->mVlTempBuf.rga_buf, p->mUvSrcRect);
            camera_source_put_frame(p->mCamSourceUV, uv_frame_id);
            break;
        case VL_UV_blendMode:
            // 获取最新的可见光图像
            vl_frame_id = camera_source_get_frame(p->mCamSourceVL);
            if (vl_frame_id < 0)
            {
                printf("VL camera failed to get frame\n");
                continue;
            }
            RGA_improcess(*camera_frame_to_RgaBuffer(p->mCamSourceVL, vl_frame_id), p->mVlTempBuf.rga_buf, p->mVlSrcRect);
            camera_source_put_frame(p->mCamSourceVL, vl_frame_id);
            // 跳过一帧紫外图像
            uv_frame_id = camera_source_get_frame(p->mCamSourceUV);
            if (uv_frame_id < 0)
            {
                printf("UV camera failed to skip frame\n");
            }
            else
                camera_source_put_frame(p->mCamSourceUV, uv_frame_id);
            // 获取最新的紫外图像，减少拖尾
            uv_frame_id = camera_source_get_frame(p->mCamSourceUV);
            if (uv_frame_id < 0)
            {
                printf("UV camera failed to get frame\n");
                continue;
            }
            // 如果光子颜色纯白，选择更简单的算法
            if (p->mColor != 0xFFFFFFFF)
            {
                RGA_imcolorkey(*camera_frame_to_RgaBuffer(p->mCamSourceUV, uv_frame_id),
                               p->mPhotonsBuf.rga_buf,
                               p->mUvSrcRect,
                               {0},
                               white_range,
                               IM_ALPHA_COLORKEY_NORMAL);
                camera_source_put_frame(p->mCamSourceUV, uv_frame_id);
                RGA_imcolorkey(p->mPhotonsBuf.rga_buf, p->mVlTempBuf.rga_buf,
                               {0},
                               {0},
                               black_range,
                               IM_ALPHA_COLORKEY_NORMAL);
            }
            else
            {
                RGA_imcolorkey(*camera_frame_to_RgaBuffer(p->mCamSourceUV, uv_frame_id),
                               p->mVlTempBuf.rga_buf,
                               p->mUvSrcRect,
                               {0},
                               black_range,
                               IM_ALPHA_COLORKEY_NORMAL);
                camera_source_put_frame(p->mCamSourceUV, uv_frame_id);
            }
            break;
        default:
            break;
        }

        // 如果处于拍照预览状态则不更新图像显示，否则更新图像显示
        if (p->isNewFrameReady == false)
        {
            // 交换缓冲区
            if (index++ % 2 == 0)
            {
                RGA_improcess(p->mVlTempBuf.rga_buf, p->mFrameBuf0.rga_buf, {0}, {0}, IM_HAL_TRANSFORM_ROT_90);
                bo = p->mBo0;
            }
            else
            {
                RGA_improcess(p->mVlTempBuf.rga_buf, p->mFrameBuf1.rga_buf, {0}, {0}, IM_HAL_TRANSFORM_ROT_90);
                bo = p->mBo1;
            }
            // 显示图像
            p->mDisplayer->display(bo);
        }
        // 录像喂数据
        p->enc->encode_commit_input_buf(&p->mVlTempBuf.rga_buf);
        //p->enc->encode_input();
        // 更新缩放倍率
        p->change_SrcRect_by_zoomRatio();
        // 刷新光子颜色掩模图
        if (p->mColor != 0xFFFFFFFF)
            imfill(p->mPhotonsBuf.rga_buf, {0}, p->mColor);
        // 处理拍照请求
        if (p->isRequireNewFrame)
        {
            RGA_improcess(p->mVlTempBuf.rga_buf, p->mTempShootBuf.rga_buf);
            p->isNewFrameReady = true;
            p->isRequireNewFrame = false;
        }
        if(p->mCurrenMode != tempMode){
            // if(p->mCurrenMode == UV_Mode){
            //     // imfill(p->mPhotonsBuf.rga_buf, {0}, 0xFF808080);
            //     // p->enc->encode_commit_input_buf(&p->mPhotonsBuf.rga_buf);
            //     // usleep(30*1000);
            //     // vl_frame_id = camera_source_get_frame(p->mCamSourceVL);
            //     // if (vl_frame_id < 0)
            //     // {
            //     //     printf("VL camera failed to get frame\n");
            //     //     continue;
            //     // }
            //     // RGA_improcess(*camera_frame_to_RgaBuffer(p->mCamSourceVL, vl_frame_id), p->mVlTempBuf.rga_buf, p->mVlSrcRect);
            //     // camera_source_put_frame(p->mCamSourceVL, vl_frame_id);
            //     p->enc->encode_commit_input_buf();
            //     printf("-------------466\n");

            // }
            p->mCurrenMode = tempMode;
        }

    }

    return nullptr;
}

void camera_blend::swtich_photons_color(uint32_t color)
{
    mColor = color;
}

void camera_blend::zoom_out()
{
    mCurrentZoomRatio -= 5;
    if (mCurrentZoomRatio < 5)
        mCurrentZoomRatio = 5;
}

void camera_blend::zoom_in()
{
    mCurrentZoomRatio += 5;
    if (mCurrentZoomRatio > 35)
        mCurrentZoomRatio = 35;
}

bool camera_blend::acquire_new_frame_data()
{
    if (isRequireNewFrame)
    {
        return false;
    }
    isRequireNewFrame = true;
    return true;
}

void *camera_blend::try_to_get_new_frame_data()
{
    if (isNewFrameReady)
    {
        isNewFrameReady = false;
        return mTempShootBuf.ptr;
    }
    return nullptr;
}

static void empty_structure(rga_buffer_t *src, rga_buffer_t *dst, rga_buffer_t *pat,
                            im_rect *srect, im_rect *drect, im_rect *prect, im_opt_t *opt)
{
    if (src != NULL)
        memset(src, 0, sizeof(*src));
    if (dst != NULL)
        memset(dst, 0, sizeof(*dst));
    if (pat != NULL)
        memset(pat, 0, sizeof(*pat));
    if (srect != NULL)
        memset(srect, 0, sizeof(*srect));
    if (drect != NULL)
        memset(drect, 0, sizeof(*drect));
    if (prect != NULL)
        memset(prect, 0, sizeof(*prect));
    if (opt != NULL)
        memset(opt, 0, sizeof(*opt));
}
IM_STATUS RGA_improcess(rga_buffer_t &src, rga_buffer_t &dst, im_rect sRect, im_rect dRect, int usage)
{

    IM_STATUS ret = IM_STATUS_NOERROR;

    rga_buffer_t pat = {0};
    im_rect prect = {0};
    usage |= IM_SYNC;
    ret = improcess(src, dst, pat, sRect, dRect, prect, usage);

    return ret;
}
IM_STATUS RGA_imcolorkey(const rga_buffer_t src, rga_buffer_t dst, im_rect srect, im_rect drect, im_colorkey_range range, int mode, int sync, int *release_fence_fd)
{
    int usage = 0;
    IM_STATUS ret = IM_STATUS_NOERROR;

    im_opt_t opt;

    rga_buffer_t pat;

    im_rect prect;

    empty_structure(NULL, NULL, &pat, NULL, NULL, &prect, &opt);

    usage |= mode;

    opt.colorkey_range = range;

    if (sync == 0)
        usage |= IM_ASYNC;
    else if (sync == 1)
        usage |= IM_SYNC;

    ret = improcess(src, dst, pat, srect, drect, prect, -1, release_fence_fd, &opt, usage);

    return ret;
}

// 无效的，第三通道不起作用，直接使用RGA_imcolorkey
IM_STATUS RGA_imcolorkey_3channel(const rga_buffer_t srcA,
                                  const rga_buffer_t srcB,
                                  rga_buffer_t dst,
                                  im_rect srectA,
                                  im_rect srectB,
                                  im_rect drect,
                                  im_colorkey_range range,
                                  int mode,
                                  int sync,
                                  int *release_fence_fd)
{
    int usage = 0;
    IM_STATUS ret = IM_STATUS_NOERROR;
    im_opt_t opt;
    empty_structure(NULL, NULL, NULL, NULL, NULL, NULL, &opt);

    usage |= mode;

    opt.colorkey_range = range;

    if (sync == 0)
        usage |= IM_ASYNC;
    else if (sync == 1)
        usage |= IM_SYNC;

    ret = improcess(srcA, dst, srcB, srectA, drect, srectB, -1, release_fence_fd, &opt, usage);

    return ret;
}

bool camera_blend::start_record(char *filename)
{
    strcat(filename, ".h264");
    // 若文件已存在，返回false
    if (access(filename, F_OK) == 0)
        return false;
    return enc->start_record(filename);
}

void camera_blend::stop_record()
{
    enc->stop_record();
}

void camera_blend::switch_mode(CamMode mode)
{

    tempMode = mode;

}

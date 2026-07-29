#include "camera_blend.h"
#include "select_delay.h"
#include <chrono>
// 标志位，清理缓存区
bool isClearCache = false;

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

void camera_blend::change_SrcRect_by_zoomRatio()
{
    switch (mCurrentZoomRatio) // 960,540,1920,1080
    {
    case 5: // 1.33333  0.88888
        mVlSrcRect.x =240 ;//if vl move more,decrease this val,else inc.
        mVlSrcRect.y = 136;
        mVlSrcRect.width = 3840 - mVlSrcRect.x*2;  // 3840/2160=1.391
        mVlSrcRect.height = 2160 - mVlSrcRect.y*2; // 1620


        mUvSrcRect.x = 100+x_offset;
        mUvSrcRect.y = 56+y_offset;
        mUvSrcRect.width = 1280 - 100 * 2; //1.185
        mUvSrcRect.height = 720 - 56 * 2;
        break;

    case 10: // 1.5
        mVlSrcRect.x = 444;
        mVlSrcRect.y = 250;
        mVlSrcRect.width = 3840 - mVlSrcRect.x*2;//3840/2560=1.5
        mVlSrcRect.height = 2160 - mVlSrcRect.y*2;

        // mUvSrcRect.x = 160+x_offset*1.15;
        // mUvSrcRect.y = 90+y_offset*1.05;
        // mUvSrcRect.width = 960;//1280/960=1.33
        // mUvSrcRect.height = 540;
        break;

    case 15: // 1.71428   1.14285
        mVlSrcRect.x = 620;
        mVlSrcRect.y = 348;
        mVlSrcRect.width = 3840-mVlSrcRect.x*2;//2240
        mVlSrcRect.height = 2160-mVlSrcRect.y*2;//1260

        mUvSrcRect.x = 220+x_offset*1.15;
        mUvSrcRect.y = 124+y_offset*1.05;
        mUvSrcRect.width = 840;
        mUvSrcRect.height = 472;
        break;

    case 20: // 2  1.33333
        mVlSrcRect.x = 796;
        mVlSrcRect.y = 448;
        mVlSrcRect.width = 3840-mVlSrcRect.x*2;
        mVlSrcRect.height = 2160-mVlSrcRect.y*2;

        // mUvSrcRect.x = 280+x_offset*1.11;
        // mUvSrcRect.y = 158+y_offset*1.05;
        // mUvSrcRect.width = 720;
        // mUvSrcRect.height = 404;
        break;

    case 25: // 2.4 1.6
        mVlSrcRect.x = 970;
        mVlSrcRect.y = 546;
        mVlSrcRect.width = 3840-mVlSrcRect.x*2;//1600
        mVlSrcRect.height = 2160-mVlSrcRect.y*2;//900

        mUvSrcRect.x = 340+x_offset*1.11;
        mUvSrcRect.y = 190+y_offset*1.05;
        mUvSrcRect.width = 600;
        mUvSrcRect.height = 340;
        break;

    case 30: // 3 2
        mVlSrcRect.x = 1280;
        mVlSrcRect.y = 720;
        mVlSrcRect.width = 3840-mVlSrcRect.x*2;
        mVlSrcRect.height = 2160-mVlSrcRect.y*2;

        mUvSrcRect.x = 400+x_offset*1.11;
        mUvSrcRect.y = 224+y_offset*1.05;
        mUvSrcRect.width = 480;
        mUvSrcRect.height = 272;
        break;
    case 35: // 4 2.66667
        mVlSrcRect.x = 1440; // -now / 2
        mVlSrcRect.y = 810;// -now / 2
        mVlSrcRect.width = 960; // 16
        mVlSrcRect.height = 540;// 9

        mUvSrcRect.x = 460+x_offset*1.1;
        mUvSrcRect.y = 260+y_offset*1.05;
        mUvSrcRect.width = 360;
        mUvSrcRect.height = 200;
        break;
    case 40: // 4 2.66667
        mVlSrcRect.x = 1440 + 32; // -now / 2
        mVlSrcRect.y = 810 + 18;// -now / 2
        mVlSrcRect.width = 960 - 64; // 16
        mVlSrcRect.height = 540 - 36;// 9
        break;
    case 45: // 4 2.66667
        mVlSrcRect.x = 1440 + 32 * 2; // -now / 2
        mVlSrcRect.y = 810 + 18 * 2;// -now / 2
        mVlSrcRect.width = 960 - 64 * 2; // 16
        mVlSrcRect.height = 540 - 36 * 2;// 9
        break;
    case 50: // 4 2.66667
        mVlSrcRect.x = 1440 + 32 * 3; // -now / 2
        mVlSrcRect.y = 810 + 18 * 3;// -now / 2
        mVlSrcRect.width = 960 - 64 * 3; // 16
        mVlSrcRect.height = 540 - 36 * 3;// 9
        break;
    case 55: // 4 2.66667
        mVlSrcRect.x = 1440 + 32 * 4; // -now / 2
        mVlSrcRect.y = 810 + 18 * 4;// -now / 2
        mVlSrcRect.width = 960 - 64 * 4; // 16
        mVlSrcRect.height = 540 - 36 * 4;// 9
        break;
    case 60: // 4 2.66667
        mVlSrcRect.x = 1440 + 32 * 5; // -now / 2
        mVlSrcRect.y = 810 + 18 * 5;// -now / 2
        mVlSrcRect.width = 960 - 64 * 5; // 16
        mVlSrcRect.height = 540 - 36 * 5;// 9

        break;
    case 65: // 4 2.66667
        mVlSrcRect.x = 1440 + 32 * 6; // -now / 2
        mVlSrcRect.y = 810 + 18 * 6;// -now / 2
        mVlSrcRect.width = 960 - 64 * 6; // 16
        mVlSrcRect.height = 540 - 36 * 6;// 9

        break;
    case 70: // 4 2.66667
        mVlSrcRect.x = 1440 + 32 * 7; // -now / 2
        mVlSrcRect.y = 810 + 18 * 7;// -now / 2
        mVlSrcRect.width = 960 - 64 * 7; // 16
        mVlSrcRect.height = 540 - 36 * 7;// 9

        break;
    case 75: // 4 2.66667
        mVlSrcRect.x = 1440 + 32 * 8; // -now / 2
        mVlSrcRect.y = 810 + 18 * 8;// -now / 2
        mVlSrcRect.width = 960 - 64 * 8; // 16
        mVlSrcRect.height = 540 - 36 * 8;// 9

        break;
    case 80: // 4 2.66667
        mVlSrcRect.x = 1440 + 32 * 9; // -now / 2
        mVlSrcRect.y = 810 + 18 * 9;// -now / 2
        mVlSrcRect.width = 960 - 64 * 9; // 16
        mVlSrcRect.height = 540 - 36 * 9;// 9

        break;
    case 85: // 4 2.66667
        mVlSrcRect.x = 1440 + 32 * 10; // -now / 2
        mVlSrcRect.y = 810 + 18 * 10;// -now / 2
        mVlSrcRect.width = 960 - 64 * 10; // 16
        mVlSrcRect.height = 540 - 36 * 10;// 9

        break;
    // case 90: // 4 2.66667
    //     mVlSrcRect.x = 1440; // -now / 2
    //     mVlSrcRect.y = 810;// -now / 2
    //     mVlSrcRect.width = 960; // 16
    //     mVlSrcRect.height = 540;// 9

    //     break;
    // case 95: // 4 2.66667
    //     mVlSrcRect.x = 1440; // -now / 2
    //     mVlSrcRect.y = 810;// -now / 2
    //     mVlSrcRect.width = 960; // 16
    //     mVlSrcRect.height = 540;// 9

    //     break;
    // case 100: // 4 2.66667
    //     mVlSrcRect.x = 1440; // -now / 2
    //     mVlSrcRect.y = 810;// -now / 2
    //     mVlSrcRect.width = 960; // 16
    //     mVlSrcRect.height = 540;// 9

    //     break;
    // case 105: // 4 2.66667
    //     mVlSrcRect.x = 1440; // -now / 2
    //     mVlSrcRect.y = 810;// -now / 2
    //     mVlSrcRect.width = 960; // 16
    //     mVlSrcRect.height = 540;// 9

    //     break;
    // case 110: // 4 2.66667
    //     mVlSrcRect.x = 1440; // -now / 2
    //     mVlSrcRect.y = 810;// -now / 2
    //     mVlSrcRect.width = 960; // 16
    //     mVlSrcRect.height = 540;// 9

    //     break;

    default:
        break;
    }
}
int camera_blend::setxy(int x, int y){
    // 文件路径
    QString filePath = "/mnt/sdcard/config.ini";  // 请根据实际路径修改

    // 打开文件
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件!";
        return -1;
    }

    // 读取文件内容
    QTextStream in(&file);
    QString fileContent = in.readAll();
    file.close();

    // 定义正则表达式，匹配形如 ip: xx 或 port: xx 的格式
    QRegularExpression regex("(ip|port):\\s*(\\S+)");

    // 查找所有匹配项
    QRegularExpressionMatchIterator i = regex.globalMatch(fileContent);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString variable = match.captured(1);  // 获取 ip 或 port
        QString value = match.captured(2);     // 获取对应的值

        // 打印变量名和对应的值
        qDebug() << variable << ":" << value;

        // 根据变量名分别赋值
        if (variable == "ip") {
            this->ip = value;  // ip 是字符串
        } else if (variable == "port") {
            this->port = value.toInt();  // port 是整数
        }
    }


    return 0;
}


bool camera_blend::camBlendThreadStart(DrmDisplay *displayer,
                                       pOutput_func output_func,
                                       osd_infos osd_data)
{

    int ret = 0;
    mDisplayer = displayer;

    // init camera source
    mCamSourceVL = isp_camera_source_init(DEV_VL, 4, 3840, 2160, MPP_FMT_YUV420SP);
    //    mCamSourceVL = camera_source_init(DEV_VL, 4, 3840, 2160, MPP_FMT_YUV420SP);

    if (mCamSourceVL == NULL)
        printf("Warning: VL camera source init failed, VL mode will be unavailable\n");

    mCamSourceUV = camera_source_init(DEV_UV, 4, 1280, 720, MPP_FMT_BGR888);

    if (mCamSourceUV == NULL)
        printf("Warning: UV camera source init failed, UV mode will be unavailable\n");

    // init IR camera USB
    mIRCameraUSB = new IRCameraUSB();
    if (!mIRCameraUSB->IR_init()) {
        printf("Warning: Failed to init IR camera USB, IR mode will be unavailable\n");
        delete mIRCameraUSB;
        mIRCameraUSB = NULL;
    }


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
        enc->launch(output_func, &osd_data);
    } while (0);

    // 如果编码器初始化失败，清理并置空
    if (ret != MPP_OK && enc != NULL)
    {
        printf("Warning: encoder init failed, recording and encoding will be unavailable\n");
        delete enc;
        enc = NULL;
    }

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

    ret = get_drm_buffer(&mTempShootBuf, 1920, 1080, RK_FORMAT_RGBA_8888);
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

    // 至少需要一路摄像头才能启动显示线程
    if (mCamSourceVL == NULL && mCamSourceUV == NULL && mIRCameraUSB == NULL)
    {
        printf("Error: no camera source available\n");
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
    black_range.max = 0xFF000015;//black==15   no_black==01
    black_range.min = 0xFF000000;
    CamFrame *uv_frame_ptr = NULL;
    int i = 0;
    int frame_count = 0;

    auto last_time = std::chrono::steady_clock::now();
    float current_fps = 0.0f;
    DELAY_INIT;

    unsigned char* yuv_data = NULL;
    int yuv_size = 0;
    unsigned char* raw_data = NULL;
    int raw_size = 0;
    int irWidth = 0;
    int irHeight = 0;
    
    while (1)
    {
        DELAY_US(30000, 1000); // 16.666ms 误差0.5ms
        switch (p->mCurrenMode)
        {
        case VL_Mode:
            if (p->mCamSourceVL == NULL)
            {
                printf("VL camera not available\n");
                continue;
            }
            vl_frame_id = camera_source_get_frame(p->mCamSourceVL);
            if (vl_frame_id < 0)
            {
                printf("VL camera failed to get frame\n");
                continue;
            }
            if (p->enc)
                RGA_improcess(*camera_frame_to_RgaBuffer(p->mCamSourceVL, vl_frame_id), p->enc->mEnc_buf[index].rga_buf, p->mVlSrcRect);
            camera_source_put_frame(p->mCamSourceVL, vl_frame_id);
            break;
        case IR_Mode:
            {
                if (p->mIRCameraUSB == NULL)
                {
                    printf("IR camera not available\n");
                    continue;
                }
                rga_buffer_t ir_rga_buf{0};
                rga_buffer_handle_t ir_rga_handle{0};
                if (p->mIRCameraUSB->IR_getFrame(&yuv_data, &yuv_size, &raw_data, &raw_size, &irWidth, &irHeight)) {
                    if (yuv_data && yuv_size > 0 && p->enc) {
                        ir_rga_handle = importbuffer_virtualaddr(yuv_data, irWidth, irHeight, RK_FORMAT_UYVY_422);
                        ir_rga_buf = wrapbuffer_handle(ir_rga_handle, irWidth, irHeight, RK_FORMAT_UYVY_422);
                        RGA_improcess(ir_rga_buf, p->enc->mEnc_buf[index].rga_buf, p->mIrSrcRect, p->IRrect);
                        releasebuffer_handle(ir_rga_handle);
                    }
                }
            }
            break;
        case UV_Mode:
            if (p->mCamSourceUV == NULL)
            {
                printf("UV camera not available\n");
                continue;
            }
            uv_frame_id = camera_source_get_frame(p->mCamSourceUV);
            if (uv_frame_id < 0)
            {
                printf("UV camera failed to get frame\n");
                continue;
            }
            if (p->enc)
                RGA_improcess(*camera_frame_to_RgaBuffer(p->mCamSourceUV, uv_frame_id),p->enc->mEnc_buf[index].rga_buf, p->mUvSrcRect);
            camera_source_put_frame(p->mCamSourceUV, uv_frame_id);
            break;
        case VL_UV_blendMode:
            // 检查所需摄像头是否可用
            if (p->mCamSourceVL == NULL || p->mCamSourceUV == NULL)
            {
                printf("VL_UV_blendMode: VL or UV camera not available\n");
                continue;
            }
            // 获取最新的可见光图像
            vl_frame_id = camera_source_get_frame(p->mCamSourceVL);
            if (vl_frame_id < 0)
            {
                printf("VL camera failed to get frame\n");
                continue;
            }
            if (p->enc)
                RGA_improcess(*camera_frame_to_RgaBuffer(p->mCamSourceVL, vl_frame_id), p->enc->mEnc_buf[index].rga_buf, p->mVlSrcRect);
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
                if (p->enc)
                    RGA_imcolorkey(p->mPhotonsBuf.rga_buf, p->enc->mEnc_buf[index].rga_buf,
                                   {0},
                                   {0},
                                   black_range,
                               IM_ALPHA_COLORKEY_NORMAL);
            }
            else
            {
                if (p->enc)
                    RGA_imcolorkey(*camera_frame_to_RgaBuffer(p->mCamSourceUV, uv_frame_id),
                                   p->enc->mEnc_buf[index].rga_buf,
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
        if (p->isNewFrameReady == false && p->enc)
        {
            // 交换缓冲区
            if (index)
            {
                RGA_improcess(p->enc->mEnc_buf[index].rga_buf, p->mFrameBuf0.rga_buf, {0}, {0}, IM_HAL_TRANSFORM_ROT_90);
                bo = p->mBo0;
            }
            else
            {
                RGA_improcess(p->enc->mEnc_buf[index].rga_buf, p->mFrameBuf1.rga_buf, {0}, {0}, IM_HAL_TRANSFORM_ROT_90);
                bo = p->mBo1;
            }
            // 显示图像
            p->mDisplayer->display(bo);
        }
        // 录像喂数据（仅在编码器有效时）
        if (p->enc)
            p->enc->encode_commit_input_buf(index);
        // 更新缩放倍率
        p->change_SrcRect_by_zoomRatio();
        // 刷新光子颜色掩模图
        if (p->mCurrenMode == VL_UV_blendMode && p->mColor != 0xFFFFFFFF)
            imfill(p->mPhotonsBuf.rga_buf, {0}, p->mColor);
            
        // 处理拍照请求
        if (p->isRequireNewFrame && p->enc)
        {
            RGA_improcess(p->enc->mEnc_buf[index].rga_buf, p->mTempShootBuf.rga_buf);
            if ((p->mCurrenMode == IR_Mode || p->mCurrenMode == SoundWave_Mode) && raw_data && raw_size > 0) {
                p->mRawData = raw_data;
                p->mRawSize = raw_size;
                p->mRawWidth = irWidth;
                p->mRawHeight = irHeight;
                p->mIRYuvData = yuv_data;
                p->mIRYuvSize = yuv_size;
            } else {
                p->mRawData = nullptr;
                p->mRawSize = 0;
                p->mIRYuvData = nullptr;
                p->mIRYuvSize = 0;
            }
            p->isNewFrameReady = true;
            p->isRequireNewFrame = false;
        }
        index = !index;

        if(p->enc && isClearCache){
            memset(p->enc->mEnc_buf[0].ptr, 0, 1920*1080*4);
            memset(p->enc->mEnc_buf[1].ptr, 0, 1920*1080*4);
            isClearCache = false;
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
    if (mCurrentZoomRatio > 85)
        mCurrentZoomRatio = 85;
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

bool camera_blend::get_raw_data(unsigned char **data, int *size, int *width, int *height)
{
    if (mRawData && mRawSize > 0) {
        if (data) *data = mRawData;
        if (size) *size = mRawSize;
        if (width) *width = mRawWidth;
        if (height) *height = mRawHeight;
        return true;
    }
    return false;
}

bool camera_blend::get_ir_frame_data(unsigned char **yuv_data, int *yuv_size, unsigned char **raw_data, int *raw_size, int *width, int *height)
{
    if (mRawData && mRawSize > 0 && mIRYuvData && mIRYuvSize > 0) {
        if (yuv_data) *yuv_data = mIRYuvData;
        if (yuv_size) *yuv_size = mIRYuvSize;
        if (raw_data) *raw_data = mRawData;
        if (raw_size) *raw_size = mRawSize;
        if (width) *width = mRawWidth;
        if (height) *height = mRawHeight;
        return true;
    }
    return false;
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
    if (enc == NULL)
    {
        printf("encoder not initialized, cannot start record\n");
        return false;
    }
    strcat(filename, ".h264");
    // 若文件已存在，返回false
    if (access(filename, F_OK) == 0)
        return false;
    return enc->start_record(filename);
}

void camera_blend::stop_record()
{
    if (enc == NULL)
    {
        printf("encoder not initialized, cannot stop record\n");
        return;
    }
    enc->stop_record();
}

void camera_blend::switch_mode(CamMode mode)
{
    mCurrenMode = mode;
    // 设置红外数据标志
    if (mode == IR_Mode) {
        isClearCache = true;
        IR_DataFlag = true;
    } else {
        IR_DataFlag = false;
    }
}

void camera_blend::switch_osd_language(int language)
{
    if (enc && language != enc->isCN)
    {
        enc->switch_language(&enc->osd_data, language);
    }
}

camera_blend::~camera_blend()
{
    if (mIRCameraUSB) {
        delete mIRCameraUSB;
        mIRCameraUSB = NULL;
    }
}
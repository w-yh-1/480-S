#ifndef _CAMERA_BLEND_H_
#define _CAMERA_BLEND_H_

#include <pthread.h>
#include "drm_display.h"
#include "drm_buf.h"
#include "dma_buf.h"
#include "camera_source.h"
#include "encoder.h"
#define DEV_VL "/dev/video11"
#define DEV_UV "/dev/video20"
// 作用:每编码好一帧，编码器会自动调用此函数。并传入编码后的帧数据地址和帧数据大小，具体的处理逻辑由用户根据需求实现。
typedef void (*pOutput_func)(void *, size_t);

enum CamMode
{
    VL_Mode=0,
    VL_UV_blendMode,
    UV_Mode,
    UNINITIALIZED
};
class camera_blend
{
    // -55,56
public:
    int x_offset = -55;
    int y_offset = 35;
private:
    encoder *enc = NULL;
    CamSource *mCamSourceVL = NULL;
    CamSource *mCamSourceUV = NULL;
    pthread_t mthread;
    DRM_Buffer mFrameBuf0;
    DRM_Buffer mFrameBuf1;
    DRM_Buffer mVlTempBuf;
    DMA_Buffer mPhotonsBuf;
    DRM_Buffer mTempShootBuf;
    DrmDisplay *mDisplayer = NULL;
    im_rect mVlSrcRect = {640, 360, 2560, 1440};
    im_rect mUvSrcRect = {160, 90, 960, 540};
    struct sp_bo *mBo0 = NULL;
    struct sp_bo *mBo1 = NULL;
    int mCurrentZoomRatio = 5;    // 当前缩放倍率 单位：0.1倍 10代表1倍。
    uint32_t mColor = 0xFFFFFFFF; // 默认为白色
    bool isColorChanged = true;
    bool isRequireNewFrame = false;
    bool isNewFrameReady = false;
    // func
    void change_SrcRect_by_zoomRatio();
    static void *camBlendTask(void *args);
    CamMode mCurrenMode = VL_UV_blendMode;
public:
    camera_blend(/* args */) {};
    ~camera_blend() {};
    bool camBlendThreadStart(DrmDisplay *displayer,
                                           pOutput_func output_func,
                                           unsigned int *gain_val_ptr,
                                           unsigned int *count_val_ptr,
                                           unsigned int *count_period_ptr,
                                           unsigned int *distance_val_ptr);
    void swtich_photons_color(uint32_t color); // color:BGRA
    void zoom_out();                           // 缩小
    void zoom_in();                            // 放大
    // 要求底层准备新的帧数据(格式1920*1080 BGRA),成功返回true，否则返回false(上一帧还没处理完，不能再提供新的帧数据)
    bool acquire_new_frame_data();
    // 尝试获取新的帧数据,获取成功返回帧数据指针,否则返回NULL。(根据acquire_new_frame_data的调用时机不同，最长不超过20ms一定能拿到)
    void *try_to_get_new_frame_data();
    bool start_record(char *filename);
    void stop_record();
    void switch_mode(CamMode mode);
};

#endif

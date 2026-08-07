#ifndef _CAMERA_BLEND_H_
#define _CAMERA_BLEND_H_

#include <pthread.h>
#include "drm_display.h"
#include "drm_buf.h"
#include "dma_buf.h"
#include "camera_source.h"
#include "encoder.h"
#include "RtpSendPs.h"

#include "USBLib.h"
#include "ir_camera_usb.h"

#include <QFile>        // 用于文件操作（QFile类）
#include <QTextStream>  // 用于文本流操作（QTextStream类）
#include <QIODevice>    // 用于文件和设备的读取、写入操作（QIODevice类）
#include <QDebug>       // 用于调试输出（qWarning函数）

#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>


#define DEV_VL "/dev/video11"
#define DEV_UV "/dev/video20"


// 作用:每编码好一帧，编码器会自动调用此函数。并传入编码后的帧数据地址和帧数据大小，具体的处理逻辑由用户根据需求实现。
typedef void (*pOutput_func)(void *, size_t,uint8_t,bool);

class camera_blend
{
public:
private:
    encoder *enc = NULL;
    CamSource *mCamSourceVL = NULL;
    CamSource *mCamSourceUV = NULL;
    CamSource *mCamSourceIR = NULL;

    IRCameraUSB *mIRCameraUSB = NULL;

    pthread_t mthread;
    DRM_Buffer mFrameBuf0;
    DRM_Buffer mFrameBuf1;
    DMA_Buffer mPhotonsBuf;
    DRM_Buffer mTempShootBuf;
    DrmDisplay *mDisplayer = NULL;
    im_rect mVlSrcRect = {640, 360, 2560, 1440};
    im_rect mUvSrcRect = {160, 90, 960, 540};
    im_rect mIrSrcRect = {0, 0, 640, 512};
    im_rect IRrect = {285, 0, 1350, 1080};
    im_rect mIruvDstRect = {285, 0, 1350, 1080}; // IR-UV融合: UV叠加目标矩形(经偏移矫正)
    im_rect mIruvDstRectClipped = {285, 0, 1350, 1080}; // 裁剪到IRrect范围内的UV目标矩形
    im_rect mUvSrcRectForBlend = {160, 90, 960, 540}; // 融合模式UV源矩形(含红外变焦裁剪)
    // IR-UV 偏移矫正参数（直接在代码中修改）
    // mIruvOverlayRect: IR图像上UV基准覆盖区域（1.0倍缩放时的UV目标区域）
    im_rect mIruvOverlayRect = {200, 150, 950, 780};
    float mIruvHorzScaleFactor = 0.75f;  // UV水平缩放因子（基准比例）
    float mIruvVertScaleFactor = 0.5f;  // UV垂直缩放因子（基准比例，可独立调整）
    int mIruvHorzDistCorrectionFactor = 0; // 水平距离矫正因子
    int mIruvVertDistCorrectionFactor = 0; // 垂直距离矫正因子
    int mIruvHorzAngCorrectionFactor = 15;  // 水平角度矫正因子
    int mIruvVertAngCorrectionFactor = 25;  // 垂直角度矫正因子
    struct sp_bo *mBo0 = NULL;
    struct sp_bo *mBo1 = NULL;
    int mCurrentZoomRatio = 5;    // 当前缩放倍率 单位：0.1倍 10代表1倍。
    int mIrZoomLevel = 1;         // 红外数字变焦倍数：1=正常，2=2倍
    uint32_t mColor = 0xFFFFFFFF; // 默认为白色
    bool isColorChanged = true;
    bool isRequireNewFrame = false;
    bool isNewFrameReady = false;
    unsigned char *mRawData = nullptr;
    int mRawSize = 0;
    int mRawWidth = 0;
    int mRawHeight = 0;
    unsigned char *mIRYuvData = nullptr;
    int mIRYuvSize = 0;
    // func
    void change_SrcRect_by_zoomRatio();
    void update_ir_uv_rects(int distance = 0);
    static void *camBlendTask(void *args);
    CamMode mCurrenMode = VL_UV_blendMode;


public:
    camera_blend(/* args */) {};
    ~camera_blend();
    void setIrZoom(int level);
    // UV 范围调试接口（供 QML 调用）
    int getIruvDstX() { return mIruvDstRect.x; }
    int getIruvDstY() { return mIruvDstRect.y; }
    int getIruvDstWidth() { return mIruvDstRect.width; }
    int getIruvDstHeight() { return mIruvDstRect.height; }
    int getIruvDstClippedX() { return mIruvDstRectClipped.x; }
    int getIruvDstClippedY() { return mIruvDstRectClipped.y; }
    int getIruvDstClippedWidth() { return mIruvDstRectClipped.width; }
    int getIruvDstClippedHeight() { return mIruvDstRectClipped.height; }

    // 因子矫正接口（供 QML 调用）
    int getHorzDistCorrectionFactor() { return mIruvHorzDistCorrectionFactor; }
    void setHorzDistCorrectionFactor(int v) {
        mIruvHorzDistCorrectionFactor = v;
        qDebug() << "HorzDistFactor: " << v << endl;
    }
    int getVertDistCorrectionFactor() { return mIruvVertDistCorrectionFactor; }
    void setVertDistCorrectionFactor(int v) {
        mIruvVertDistCorrectionFactor = v;
        qDebug() << "VertDistFactor: " << v << endl;
    }
    int getHorzAngCorrectionFactor() { return mIruvHorzAngCorrectionFactor; }
    void setHorzAngCorrectionFactor(int v) {
        mIruvHorzAngCorrectionFactor = v;
        qDebug() << "HorzAngFactor: " << v << endl;
    }
    int getVertAngCorrectionFactor() { return mIruvVertAngCorrectionFactor; }
    void setVertAngCorrectionFactor(int v) {
        mIruvVertAngCorrectionFactor = v;
        qDebug() << "VertAngFactor: " << v << endl;
    }
    float getHorzScaleFactor() { return mIruvHorzScaleFactor; }
    void setHorzScaleFactor(float v) {
        mIruvHorzScaleFactor = v;
        qDebug() << "HorzScaleFactor: " << v << endl;
    }
    float getVertScaleFactor() { return mIruvVertScaleFactor; }
    void setVertScaleFactor(float v) {
        mIruvVertScaleFactor = v;
        qDebug() << "VertScaleFactor: " << v << endl;
    }

    bool camBlendThreadStart(DrmDisplay *displayer,pOutput_func output_func,osd_infos osd_data);
    void swtich_photons_color(uint32_t color); // color:BGRA
    void zoom_out();                           // 缩小
    void zoom_in();                            // 放大
    // 要求底层准备新的帧数据(格式1920*1080 BGRA),成功返回true，否则返回false(上一帧还没处理完，不能再提供新的帧数据)
    bool acquire_new_frame_data();
    // 尝试获取新的帧数据,获取成功返回帧数据指针,否则返回NULL。(根据acquire_new_frame_data的调用时机不同，最长不超过20ms一定能拿到)
    void *try_to_get_new_frame_data();
    bool get_raw_data(unsigned char **data, int *size, int *width, int *height);
    bool get_ir_frame_data(unsigned char **yuv_data, int *yuv_size, unsigned char **raw_data, int *raw_size, int *width, int *height);
    bool start_record(char *filename);
    void stop_record();
    void switch_mode(CamMode mode);
    void switch_osd_language(int language);
    int setxy(int x,int y);
    QString ip;
    int port;
    int x_offset = -15;//move right:-,move left:+
    int y_offset = -30;//move down:-,move up:+

};

#endif
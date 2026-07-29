#ifndef __IR_CAMERA_USB_H__
#define __IR_CAMERA_USB_H__

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include "USBLib.h"
extern bool IR_DataFlag;

class IRCameraUSB
{
public:
    IRCameraUSB();
    ~IRCameraUSB();
    bool IR_init();
    void IR_deinit();
    bool IR_getFrame(unsigned char** yuv_data, int* yuv_size, unsigned char** raw_data, int* raw_size, int* width, int* height);
    
    bool IR_PointTemperature(int x, int y, float* temperature);
    bool IR_LineTemperature(int x1, int y1, int x2, int y2, USBSDK_RuleTemper* temp);
    bool IR_RectangleTemperature(int x1, int y1, int x2, int y2, USBSDK_RuleTemper* temp);
    bool IR_getHotColdPoint(USBSDK_HotAndColdInfo* temp);
    void screenCoord2IRCoord(int screenX, int screenY, int* irX, int* irY);
    void irCoordToScreenCoord(int irX, int irY, int* screenX, int* screenY);
private:
    USBSDK_DeviceInfo devInfoList[12];
    unsigned char* yuvBuffer;
    unsigned char* rawBuffer;
    int yuvBufferSize;
    int rawBufferSize;
    int width, height;
    pthread_mutex_t IR_mutex;
    static void CALLBACK IR_dataCallback(unsigned char* yuv, int yuvSize,
                                   unsigned char* raw, int rawSize,
                                   int w, int h, DWORD user);
};

#endif
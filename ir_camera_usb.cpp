#include "ir_camera_usb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QDebug>
bool IR_DataFlag = false; 
int irScreenX = 285, irScreenY = 0, irScreenWidth = 1350, irScreenHeight = 1080;
IRCameraUSB::IRCameraUSB()
{
    memset(devInfoList, 0, sizeof(devInfoList));
    yuvBuffer = NULL;
    rawBuffer = NULL;
    yuvBufferSize = 0;
    rawBufferSize = 0;
    width = height = 0;
    pthread_mutex_init(&IR_mutex, NULL);
}

IRCameraUSB::~IRCameraUSB()
{
    IR_deinit();
    pthread_mutex_destroy(&IR_mutex);
}
//摄像头初始化
bool IRCameraUSB::IR_init()
{
    if (!USBSDK_Init()) {
        printf("USBSDK_Init failed: %d\n", USBSDK_GetLastError());
        return false;
    }
    
    if (USBSDK_GetDevInfoList(devInfoList) <= 0) {
        printf("USBSDK_GetDevInfoList failed: %d\n", USBSDK_GetLastError());
        return false;
    }
    
    if (!USBSDK_Open(&devInfoList[0])) {
        printf("USBSDK_Open failed: %d\n", USBSDK_GetLastError());
        return false;
    }

    if (!USBSDK_OpenThermalFunction()) {
        printf("USBSDK_OpenThermalFunction failed: %d\n", USBSDK_GetLastError());
        return false;
    }


    if (!USBSDK_StartRealPlay((fUSBSDKDataCallBack)IR_dataCallback, (DWORD)this)) {
        printf("USBSDK_StartRealPlay failed: %d\n", USBSDK_GetLastError());
        return false;
    }
    USBSDK_SetColor(6);
    printf("IR camera started\n");
    return true;
}

void IRCameraUSB::IR_deinit()
{
    USBSDK_CloseThermalFunction();
    USBSDK_CloseRealPlay();
    printf("IR camera stopped\n");
    if (yuvBuffer) {
        free(yuvBuffer);
        yuvBuffer = NULL;
    }
    
    if (rawBuffer) {
        free(rawBuffer);
        rawBuffer = NULL;
    }
    
    USBSDK_Close();
    printf("IR camera deinitialized\n");
}


bool IRCameraUSB::IR_getFrame(unsigned char** yuv_data, int* yuv_size, unsigned char** raw_data, int* raw_size, int* width, int* height)
{
    pthread_mutex_lock(&IR_mutex);
    if (yuv_data) *yuv_data = yuvBuffer;
    if (yuv_size) *yuv_size = yuvBufferSize;
    if (raw_data) *raw_data = rawBuffer;
    if (raw_size) *raw_size = rawBufferSize;    
    if (width) *width = this->width;
    if (height) *height = this->height;
    
    pthread_mutex_unlock(&IR_mutex);
    return true;
}

//红外获取数据回调函数
void CALLBACK IRCameraUSB::IR_dataCallback(unsigned char* yuv, int yuvSize,
                                       unsigned char* raw, int rawSize,
                                       int w, int h, DWORD user)
{    
    if (IR_DataFlag == false) {
        return;
    }
    IRCameraUSB* cam = (IRCameraUSB*)user;
    if (!cam) return;
    pthread_mutex_lock(&cam->IR_mutex);
    
    cam->width = w;
    cam->height = h;
    
    if (yuvSize > cam->yuvBufferSize) {
        if (cam->yuvBuffer) free(cam->yuvBuffer);
        cam->yuvBuffer = (unsigned char*)malloc(yuvSize);
        cam->yuvBufferSize = yuvSize;
    }
    
    if (rawSize > cam->rawBufferSize) {
        if (cam->rawBuffer) free(cam->rawBuffer);
        cam->rawBuffer = (unsigned char*)malloc(rawSize);
        cam->rawBufferSize = rawSize;
    }
    
    if (yuv && cam->yuvBuffer) {
        memcpy(cam->yuvBuffer, yuv, yuvSize);
    }
    
    if (raw && cam->rawBuffer) {
        memcpy(cam->rawBuffer, raw, rawSize);
    }
    
    pthread_mutex_unlock(&cam->IR_mutex);
}

//获取点温度 x, y 坐标，temperature 输出温度值
bool IRCameraUSB::IR_PointTemperature(int x, int y, float* temperature)
{
    if (!temperature) {
        printf("Invalid temperature pointer\n");
        return false;
    }
    screenCoord2IRCoord(x, y, &x, &y);
    USBSDK_Rules rules;
    memset(&rules, 0, sizeof(rules));
    rules.count = 1;
    rules.Rules[0].ruleId = 1;
    rules.Rules[0].enable = 1;
    rules.Rules[0].meterType = USBSDK_RADIOMETERY_SPOTMETER;
    rules.Rules[0].localParam.enable = 0;
    rules.Rules[0].regionConfig.spotPoint.x = x;
    rules.Rules[0].regionConfig.spotPoint.y = y;
    strcpy(rules.Rules[0].ruleName, "PointMeasure");
    
    USBSDK_SetThermalRules(rules);
    USBSDK_RuleTemper temperInfo;
    memset(&temperInfo, 0, sizeof(temperInfo));
    USBSDK_GetThermalRules(1, temperInfo);
    
    *temperature = temperInfo.temperatureAve;
    return true;
}
//获取线温度 x1, y1, x2, y2 线段坐标
bool IRCameraUSB::IR_LineTemperature(int x1, int y1, int x2, int y2, USBSDK_RuleTemper* temp)
{
    if (!temp) {
        printf("Invalid temperature pointer\n");
        return false;
    }
    
    screenCoord2IRCoord(x1, y1, &x1, &y1);
    screenCoord2IRCoord(x2, y2, &x2, &y2);
    
    USBSDK_Rules rules;
    memset(&rules, 0, sizeof(rules));
    rules.count = 1;
    
    rules.Rules[0].ruleId = 3;
    rules.Rules[0].enable = 1;
    rules.Rules[0].meterType = USBSDK_RADIOMETERY_LINEMETER;
    rules.Rules[0].localParam.enable = 0;
    
    rules.Rules[0].regionConfig.linePostion.start.x = x1;
    rules.Rules[0].regionConfig.linePostion.start.y = y1;
    rules.Rules[0].regionConfig.linePostion.end.x = x2;
    rules.Rules[0].regionConfig.linePostion.end.y = y2;
    strcpy(rules.Rules[0].ruleName, "LineMeasure");
    
    USBSDK_SetThermalRules(rules);
    
    memset(temp, 0, sizeof(USBSDK_RuleTemper));
    USBSDK_GetThermalRules(3, *temp);
    
    irCoordToScreenCoord(temp->hotPoint.x, temp->hotPoint.y, &temp->hotPoint.x, &temp->hotPoint.y);
    irCoordToScreenCoord(temp->coldPoint.x, temp->coldPoint.y, &temp->coldPoint.x, &temp->coldPoint.y);

    return true;
}
//获取矩形温度 x1, y1, x2, y2 矩形对角坐标
bool IRCameraUSB::IR_RectangleTemperature(int x1, int y1, int x2, int y2, USBSDK_RuleTemper* temp)
{
    if (!temp) {
        printf("Invalid temperature pointer\n");
        return false;
    }
    screenCoord2IRCoord(x1, y1, &x1, &y1);
    screenCoord2IRCoord(x2, y2, &x2, &y2);
    USBSDK_Rules rules;
    memset(&rules, 0, sizeof(rules));
    rules.count = 1;
    
    rules.Rules[0].ruleId = 2;
    rules.Rules[0].enable = 1;
    rules.Rules[0].meterType = USBSDK_RADIOMETERY_AREAMETER;
    rules.Rules[0].subType = USBSDK_RADIOMETERY_AREARECT;
    rules.Rules[0].localParam.enable = 0;
    
    rules.Rules[0].regionConfig.areaRegion.pointNumber = 4;
    rules.Rules[0].regionConfig.areaRegion.points[0].x = x1;
    rules.Rules[0].regionConfig.areaRegion.points[0].y = y1;
    rules.Rules[0].regionConfig.areaRegion.points[1].x = x2;
    rules.Rules[0].regionConfig.areaRegion.points[1].y = y1;
    rules.Rules[0].regionConfig.areaRegion.points[2].x = x2;
    rules.Rules[0].regionConfig.areaRegion.points[2].y = y2;
    rules.Rules[0].regionConfig.areaRegion.points[3].x = x1;
    rules.Rules[0].regionConfig.areaRegion.points[3].y = y2;
    strcpy(rules.Rules[0].ruleName, "RectMeasure");
    
    USBSDK_SetThermalRules(rules);
    memset(temp, 0, sizeof(USBSDK_RuleTemper));
    USBSDK_GetThermalRules(2, *temp);
    
    irCoordToScreenCoord(temp->hotPoint.x, temp->hotPoint.y, &temp->hotPoint.x, &temp->hotPoint.y);
    irCoordToScreenCoord(temp->coldPoint.x, temp->coldPoint.y, &temp->coldPoint.x, &temp->coldPoint.y);
    return true;
}

//获取热点和冷点温度
bool IRCameraUSB::IR_getHotColdPoint(USBSDK_HotAndColdInfo* temp)
{
    if (!temp) {
        printf("Invalid temperature pointer\n");
        return false;
    }
    
    USBSDK_HotAndColdInfo hotColdInfo;
    memset(&hotColdInfo, 0, sizeof(hotColdInfo));
    
    if (!USBSDK_GetThermalHotColdPoint(&hotColdInfo)) {
        printf("USBSDK_GetThermalHotColdPoint failed: %d\n", USBSDK_GetLastError());
        return false;
    }

    *temp = hotColdInfo;
    
    irCoordToScreenCoord(temp->hotPoint.x, temp->hotPoint.y, &temp->hotPoint.x, &temp->hotPoint.y);
    irCoordToScreenCoord(temp->coldPoint.x, temp->coldPoint.y, &temp->coldPoint.x, &temp->coldPoint.y);
    return true;
}

//screenX点击的屏幕X坐标，irScreenX红外图像在屏幕上的起始X坐标，irScreenWidth红外图像在屏幕上的宽度
void IRCameraUSB::screenCoord2IRCoord(int screenX, int screenY, int* irX, int* irY)
{
    if (!irX || !irY) {
        return;
    }
    
    float ratioX = (screenX - irScreenX) / (float)irScreenWidth;
    float ratioY = (screenY - irScreenY) / (float)irScreenHeight;
    
    if (ratioX < 0) ratioX = 0;
    if (ratioX > 1) ratioX = 1;
    if (ratioY < 0) ratioY = 0;
    if (ratioY > 1) ratioY = 1;
    
    *irX = (int)(ratioX * 8192);
    *irY = (int)(ratioY * 8192);
}

void IRCameraUSB::irCoordToScreenCoord(int irX, int irY, int* screenX, int* screenY)
{
    if (!screenX || !screenY) {
        return;
    }

    float ratioX = irX / 8192.0f;
    float ratioY = irY / 8192.0f;

    if (ratioX < 0) ratioX = 0;
    if (ratioX > 1) ratioX = 1;
    if (ratioY < 0) ratioY = 0; 
    if (ratioY > 1) ratioY = 1;

    *screenX = (int)(irScreenX + ratioX * irScreenWidth);
    *screenY = (int)(irScreenY + ratioY * irScreenHeight);
}
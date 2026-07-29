TARGET = 480-S
QT += quick
QT += network
# QT += virtualkeyboard
# //QT += quick quickcontrols2 virtualkeyboard
static {
    QT += svg
    QTPLUGIN += qtvirtualkeyboardplugin
}
CONFIG += c++11
CONFIG += QT_QPA_PLATFORM_PLUGIN_PATH=~/Qt/5.15.2/gcc_64/plugins/platforms
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
LIBS += -L/home/wyh/SDK_DIR/rk-linux5.10-SDK-20260122/buildroot/output/rockchip_rk3588/host/aarch64-buildroot-linux-gnu/sysroot/usr/lib -lrockchip_mpp


SOURCES += \
        E_button.cpp \
        FileModel.cpp \
        GPS.cpp \
        ID_R_service.cpp \
        RtpSendPs.cpp \
        Serial.cpp \
        Stream.cpp \
        UV_information_service.cpp \
        dma_buf.cpp \
        encoder.cpp \
        free_type_osd.cpp \
        main.cpp \
        camera_blend.cpp \
        drm_display.cpp\
        drm_buf.cpp\
        camera_source.cpp \
        ir_camera_usb.cpp \
        IRTemperatureWrapper.cpp

INCLUDEPATH += ./include
INCLUDEPATH += /home/wyh/SDK_DIR/rk-linux5.10-SDK-20260122/buildroot/output/rockchip_rk3588/host/aarch64-buildroot-linux-gnu/sysroot/usr/include/rockchip
INCLUDEPATH += /home/wyh/SDK_DIR/rk-linux5.10-SDK-20260122/buildroot/output/rockchip_rk3588/host/aarch64-buildroot-linux-gnu/sysroot/usr/include/rga
INCLUDEPATH += /home/wyh/SDK_DIR/rk-linux5.10-SDK-20260122/buildroot/output/rockchip_rk3588/host/aarch64-buildroot-linux-gnu/sysroot/usr/include/libdrm
INCLUDEPATH += /home/wyh/SDK_DIR/rk-linux5.10-SDK-20260122/buildroot/output/rockchip_rk3588/host/aarch64-buildroot-linux-gnu/sysroot/usr/include/rkaiq
INCLUDEPATH += /home/wyh/SDK_DIR/rk-linux5.10-SDK-20260122/buildroot/output/rockchip_rk3588/host/aarch64-buildroot-linux-gnu/sysroot/usr/include/rkaiq/algos
INCLUDEPATH += /home/wyh/SDK_DIR/rk-linux5.10-SDK-20260122/buildroot/output/rockchip_rk3588/host/aarch64-buildroot-linux-gnu/sysroot/usr/include/rkaiq/common
INCLUDEPATH += /home/wyh/SDK_DIR/rk-linux5.10-SDK-20260122/buildroot/output/rockchip_rk3588/host/aarch64-buildroot-linux-gnu/sysroot/usr/include/rkaiq/ipc_server
INCLUDEPATH += /home/wyh/SDK_DIR/rk-linux5.10-SDK-20260122/buildroot/output/rockchip_rk3588/host/aarch64-buildroot-linux-gnu/sysroot/usr/include/rkaiq/iq_parser
INCLUDEPATH += /home/wyh/SDK_DIR/rk-linux5.10-SDK-20260122/buildroot/output/rockchip_rk3588/host/aarch64-buildroot-linux-gnu/sysroot/usr/include/rkaiq/iq_parser_v2
INCLUDEPATH += /home/wyh/SDK_DIR/rk-linux5.10-SDK-20260122/buildroot/output/rockchip_rk3588/host/aarch64-buildroot-linux-gnu/sysroot/usr/include/rkaiq/smartIr
INCLUDEPATH += /home/wyh/SDK_DIR/rk-linux5.10-SDK-20260122/buildroot/output/rockchip_rk3588/host/aarch64-buildroot-linux-gnu/sysroot/usr/include/rkaiq/uAPI2
INCLUDEPATH += /home/wyh/SDK_DIR/rk-linux5.10-SDK-20260122/buildroot/output/rockchip_rk3588/host/aarch64-buildroot-linux-gnu/sysroot/usr/include/rkaiq/xcore
INCLUDEPATH += /home/wyh/SDK_DIR/rk-linux5.10-SDK-20260122/buildroot/output/rockchip_rk3588/host/aarch64-buildroot-linux-gnu/sysroot/usr/include/freetype2
# INCLUDEPATH += /usr/include/x86_64-linux-gnu/qt5

QT += core gui widgets

LIBS += -L$$PWD/lib -ldrm -lrga -lrockchip_mpp -lrkaiq -lthermal -lusb-1.0 -lUSBLib -lfreetype#-lrockit
RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =
include (src/xlsx/qtxlsx.pri)
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    E_button.h \
    FileModel.h \
    GPS.h \
    ID_R_service.h \
    Serial.h \
    Stream.h \
    UV_information_service.h \
    common.h \
    camera_blend.h \
    dma_buf.h \
    drm_display.h\
    drm_buf.h\
    camera_source.h \
    encoder.h \
    font_array.h \
    include/Log.h \
    include/Utils.h \
    select_delay.h \
    include/USBLib.h \
    ir_camera_usb.h \
    free_type_osd.h \
    IRTemperatureWrapper.h

DISTFILES += \
    photo/baojing.png \
    photo/paizhao.jpg
QT += core gui
RC_ICONS = title.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    QxtSpanSlider.cpp \
    main.cpp \
    mainwindow.cpp \
    myglwidget.cpp \
    roi.cpp

HEADERS += \
    QxtSpanSlider.h \
    QxtSpanSlider_p.h \
    include/guidemt.h \
    mainwindow.h \
    myglwidget.h \
    roi.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    IR_Analyst_zh_CN.ts
CONFIG += lMTSDK
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    Res.qrc

win32: LIBS += -L$$PWD/dll/x64/ -lMTSDK

INCLUDEPATH += $$PWD/dll/x64
DEPENDPATH += $$PWD/dll/x64

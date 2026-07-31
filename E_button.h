#ifndef E_BUTTON_H
#define E_BUTTON_H

#include <QObject>
#include "camera_blend.h"
#include "UV_information_service.h"
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "ID_R_service.h"
#include "GPS.h"
#include <QProcess>
#include <QFileDialog>
#include <QQuickImageProvider>
#include <QImage>
#include <QPixmap>
#include <QLabel>
#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkInterface>
#include "encoder.h"
#include "RtpSendPs.h"
#include "main.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QTimer>
#include <QWindow>
#include <iostream>
#include <fstream>
static unsigned int aa1[10] = {1, 1, 1, 1, 1, 1, 4, 3, 2, 1};

class E_button : public QObject
{
    Q_OBJECT
public:

    DrmDisplay *drm;
    QFile *filew;
    QFile *files;
    explicit E_button(QObject *parent = nullptr);
    QString  zy = "增益：";
    QString  zy1;
    QString gz;
    QString wd;
    QString mm;
    QString timeshijian;
    int w;
    int s;
    ID_R_service* is = NULL;
    GPS_service* gps = NULL;
    QImage image;
    QObject *rootObject;
    QLabel *imageLabel = NULL;
    int shijiankaiguan = 0;
    char buf[128] = {};

    int nnnn = 2024;
    int yy = 11;
    int dd = 15;
    int ss = 10;
    int ff = 00;
    int miao = 00;
    int sizejpg = 0;
    int sizemp4 = 0;
    // int biaozhiwei_moshi = 0;

    int quezhi = 10000;
    int biaozhiwei_quezhi_sw = 0;
    int biaozhiwei_jf = 0;

    int biaozhiwei_jiguang = 0;
    int biaozhiwei_jishu = 0;
    int biaozhiwei_jishu_1 = 0;
    int biaozhiwei_yuyan = 0;
    int guangzishu = 0;
    int haomi = 0;
    //int mssz = 1;

    Q_PROPERTY(int mssz READ getmoshi WRITE setmoshi NOTIFY msszChanged)
    Q_PROPERTY(int yuyan READ getyuyan WRITE setyuyan NOTIFY yuyanChanged)

    int m_mssz = 1;

    Q_INVOKABLE void setmoshi(int value);

    // int biaozhiwei_moshi = 0;
    Q_INVOKABLE int getguagzishu();
    Q_INVOKABLE void sdcard();
    Q_INVOKABLE void setjf(int);
    Q_INVOKABLE int getmoshi();
    Q_INVOKABLE void myFunction();
    Q_INVOKABLE void myFunction1();

    Q_INVOKABLE void anotherFunction();
    Q_INVOKABLE void anotherFunction1();
    Q_INVOKABLE camera_blend *cam= NULL;
    Q_INVOKABLE UV_information_service* uv = NULL;
    Q_INVOKABLE void openWifiSettings();
    Q_INVOKABLE QString wifiScan();
    Q_INVOKABLE QString wifiConnect(const QString &ssid, const QString &password);
    Q_INVOKABLE QString wifiStatus();

    // 蓝牙
    Q_INVOKABLE QString bluetoothCheck();
    Q_INVOKABLE QString bluetoothScan();
    Q_INVOKABLE QString bluetoothPair(const QString &addr);
    Q_INVOKABLE QString bluetoothConnect(const QString &addr);
    Q_INVOKABLE QString bluetoothIsPaired(const QString &addr);
    Q_INVOKABLE QString bluetoothDisconnect(const QString &addr);
    Q_INVOKABLE QString startFileServer();
    Q_INVOKABLE void stopFileServer();
    Q_INVOKABLE QString getWlanIP();
    Q_INVOKABLE QString getzy();
    Q_INVOKABLE QString getgz();
    Q_INVOKABLE int setjs();
    Q_INVOKABLE QString getws();
    Q_INVOKABLE QString getmm();
    Q_INVOKABLE QString getgps();
    Q_INVOKABLE QString getgps_te();
    Q_INVOKABLE void setmsqh(uint32_t color);
    Q_INVOKABLE void bgra8888();
    Q_INVOKABLE bool startluzhi();
    Q_INVOKABLE bool overluzhi();
    Q_INVOKABLE QString readBatteryLevel();
    Q_INVOKABLE QString gettime();
    Q_INVOKABLE QString settjiguang(int i);
    Q_INVOKABLE QString xxxxxjishu(int i);
    Q_INVOKABLE bool settime(QString,QString,QString,QString,QString,QString);
    Q_INVOKABLE bool setquezhi(QString);
    Q_INVOKABLE void setquezhi_sw(int onoff);
    Q_INVOKABLE int getquezhi_sw();

    Q_INVOKABLE int moshiqiehuan();
    Q_INVOKABLE void bofangshipin(QString st);
    Q_INVOKABLE void taizhangdaoru(QString st);
    Q_INVOKABLE QString get1();
    Q_INVOKABLE QString get2();
    Q_INVOKABLE QString get3();
    Q_INVOKABLE QString get4();
    Q_INVOKABLE void setyuyan(int);
    Q_INVOKABLE int getyuyan();
    Q_INVOKABLE void getrtc();
    Q_INVOKABLE int getnnnn();
    Q_INVOKABLE int getyy();
    Q_INVOKABLE int getday();
    Q_INVOKABLE int gets();
    Q_INVOKABLE int getf();
    Q_INVOKABLE int getm();


    Q_INVOKABLE void pushstream();
    Q_INVOKABLE void setzywen(int i);
    Q_INVOKABLE void switch_l(int i);
    Q_INVOKABLE void setIRColor(int index);

    // UV 范围调试接口
    Q_INVOKABLE int getIruvDstX() { return cam ? cam->getIruvDstX() : 0; }
    Q_INVOKABLE int getIruvDstY() { return cam ? cam->getIruvDstY() : 0; }
    Q_INVOKABLE int getIruvDstWidth() { return cam ? cam->getIruvDstWidth() : 0; }
    Q_INVOKABLE int getIruvDstHeight() { return cam ? cam->getIruvDstHeight() : 0; }

    // 因子矫正接口（供 QML 调用）
    Q_INVOKABLE int getHorzDistCorrectionFactor() { return cam ? cam->getHorzDistCorrectionFactor() : 0; }
    Q_INVOKABLE void setHorzDistCorrectionFactor(int v) { if (cam) cam->setHorzDistCorrectionFactor(v); }
    Q_INVOKABLE int getVertDistCorrectionFactor() { return cam ? cam->getVertDistCorrectionFactor() : 0; }
    Q_INVOKABLE void setVertDistCorrectionFactor(int v) { if (cam) cam->setVertDistCorrectionFactor(v); }
    Q_INVOKABLE int getHorzAngCorrectionFactor() { return cam ? cam->getHorzAngCorrectionFactor() : 0; }
    Q_INVOKABLE void setHorzAngCorrectionFactor(int v) { if (cam) cam->setHorzAngCorrectionFactor(v); }
    Q_INVOKABLE int getVertAngCorrectionFactor() { return cam ? cam->getVertAngCorrectionFactor() : 0; }
    Q_INVOKABLE void setVertAngCorrectionFactor(int v) { if (cam) cam->setVertAngCorrectionFactor(v); }

    //gps重连
    Q_INVOKABLE bool gps_reinit();

    // 蜂鸣器报警音播放
    Q_INVOKABLE void playAlarm();
    Q_INVOKABLE void stopAlarm();

    // 获取视频缩略图
    Q_INVOKABLE QString getVideoThumbnail(const QString &videoPath);

    // 红外测量数据JSON（由QML调用写入，拍照时读取）
    Q_INVOKABLE void setIrMeasurementData(const QString &json) { m_irMeasurementDataJson = json; }
    QString m_irMeasurementDataJson;

    // 实现写入数字到文件的函数
    Q_INVOKABLE void writeDigitToFile(QString filename, int number) {
        std::ofstream outFile(filename.toStdString(), std::ios::trunc); // 使用 trunc 标志覆盖文件内容
        if (!outFile) {
            std::cerr << "无法打开文件进行写入。" << std::endl;
            exit(1); // 返回非零值表示错误
        }
        outFile << number; // 写入数字
        outFile.close(); // 关闭文件
    }

    // 实现从文件中读取数字的函数
    Q_INVOKABLE int readDigitFromFile(QString filename) {
        int number;
        std::ifstream inFile(filename.toStdString());
        if (!inFile) {
            std::cerr << "无法打开文件进行读取。" << std::endl;
            exit(1); // 返回非零值表示错误
        }
        inFile >> number; // 读取数字
        inFile.close(); // 关闭文件
        return number; // 返回读取的数字
    }

private:
    QProcess *m_process;
    QProcess *m_alarmProcess = nullptr;
    QTcpServer *m_fileServer = nullptr;
    QString m_mediaPath = "/mnt/sdcard/Standard Storage";

private slots:
    void onFileServerNewConnection();

signals:
    void msszChanged();
    void yuyanChanged();
};

#endif // E_BUTTON_H

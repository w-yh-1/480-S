#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "E_button.h"
#include <QQmlContext>
#include "drm_display.h"
// #include "myrk_decode.h"
#include "camera_blend.h"
#include "RtpSendPs.h"
#include "UV_information_service.h"


#include <QTextCodec>
#include <QCoreApplication>
#include <QFileSystemWatcher>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QDir>
#include <QMessageBox>



#include <QDebug>
#include "main.h"
#include "qtkeyboard.h"
//#include "mainwindow.h"
#include <iostream>
#include <fstream>
#include <QProcess>


#include "IRTemperatureWrapper.h"





void executeCommand(const QString &command, const QStringList &arguments) {
    QProcess process;

    // 启动命令
    process.start(command, arguments);

    // 等待命令执行完成
    if (!process.waitForFinished()) {
        qDebug() << "命令执行失败：" << process.errorString();
        return;
    }

    // 读取命令输出
    QString output = process.readAllStandardOutput();
    QString errorOutput = process.readAllStandardError();

    // 输出结果
    qDebug() << "命令输出:\n" << output;
    if (!errorOutput.isEmpty()) {
        qDebug() << "命令错误输出:\n" << errorOutput;
    }
}
void checkAndCreateFile(const QString& directory, const QString& filename) {
    // 创建目标目录对象
    QDir dir(directory);

    // 棘检查目录是否存在
    if (!dir.exists()) {
        qDebug() << "目录不存在:" << directory;
        return;
    }

    // 检查文件是否存在
    QFile file(dir.filePath(filename));
    if (file.exists()) {
        qDebug() << "文件已存在:" << file.fileName();
    } else {
        // 文件不存在，尝试创建并写入数据
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "1"; // 写入数字1
            file.close();
            qDebug() << "文件已创建并写入:" << file.fileName();
        } else {
            qDebug() << "无法创建文件:" << file.fileName();
        }
    }
}
// 实现写入数字到文件的函数
void writeDigitToFile(const std::string& filename, int number) {
    std::ofstream outFile(filename, std::ios::trunc); // 使用 trunc 标志覆盖文件内容
    if (!outFile) {
        std::cerr << "无法打开文件进行写入。" << std::endl;
        exit(1); // 返回非零值表示错误
    }
    outFile << number; // 写入数字
    outFile.close(); // 关闭文件
}

// 实现从文件中读取数字的函数
int readDigitFromFile(const std::string& filename) {
    int number;
    std::ifstream inFile(filename);
    if (!inFile) {
        return 0;
    }
    inFile >> number; // 读取数字
    inFile.close(); // 关闭文件
    return number; // 返回读取的数字
}
static unsigned int aa[12] = {1, 1, 1, 1, 1, 1, 4, 3, 2, 1,1,1};
static RtpSendPs *pRtpSendPs=NULL;
extern char gps_jw[128];
osd_infos osd_date = {
    .cam_mode_ptr = aa + 4,
    .gain_val_ptr = aa,
    .count_val_ptr = aa + 1,
    .first_val_ptr = aa + 6,
    .second_val_ptr = aa + 7,
    .third_val_ptr = aa + 8,
    .fourth_val_ptr = aa + 9,
    .count_period_ptr = aa + 5,
    .distance_val_ptr = aa + 3,
    .T_val_ptr = aa+10,
    .H_val_ptr = aa+11,
    .gps_str_ptr = gps_jw
};
QMap<QString, QString> lastContents; // 存储每个文件的上次内容
int n_button = 1;
int n_ebutton = 0;
void saveDataToFile(int val1, int val2) {
    QString filePath = "/opt/xy";  // 替换为文件实际路径
    QFile file(filePath);

    // 尝试打开文件进行写操作，文件不存在时会被创建
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        // 将数据写入文件并覆盖原内容
        out << "Value 1: " << val1 << ", Value 2: " << val2 << "\n";
        file.close();
        qDebug() << "文件已覆盖并写入数据";
    } else {
        qDebug() << "无法打开文件进行写入！";
    }
}
void readDataFromFile(E_button *ebutton) {
    QString filePath = "/opt/xy";  // 替换为实际文件路径
    QFile file(filePath);

    // 打开文件进行读取
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString line;

        // 使用 QRegularExpression 来进行正则匹配，支持负数
        QRegularExpression regex("Value 1: (-?\\d+), Value 2: (-?\\d+)");
        QRegularExpressionMatch match;

        // 逐行读取文件内容
        while (!in.atEnd()) {
            line = in.readLine();
            qDebug() << "读取的行：" << line;  // 输出读取的行

            // 使用正则表达式匹配并提取数据
            match = regex.match(line);
            if (match.hasMatch()) {
                ebutton->cam->x_offset = match.captured(1).toInt();  // 提取第一个值
                ebutton->cam->y_offset = match.captured(2).toInt();  // 提取第二个值
                qDebug() << "读取到的值: x_offset =" << ebutton->cam->x_offset << ", y_offset =" << ebutton->cam->y_offset;
            }
        }
        file.close();
    } else {
        qDebug() << "无法打开文件进行读取！";
    }
}

void change(const QString &filePath, QObject *rootObject, E_button *ebutton){
    aa[0] = ebutton->uv->m_gain_factor;
    if ((aa[2] = ebutton->biaozhiwei_jishu) == 1)
        aa[1] = ebutton->uv->m_photons;
    else
        aa[1] = ebutton->uv->m_photons;
    if(aa[1]>99999)
        aa[1] = 99990;
    aa[3] = ebutton->haomi;
    aa[5] = ebutton->biaozhiwei_jishu;
    aa[4] = ebutton->m_mssz;
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        QString content = in.readAll().trimmed(); // 读取并去除空白
        file.close();

        // 检查内容是否为0，并与上次内容比较
        if (content == "0" && lastContents[filePath] != content)
        {
            lastContents[filePath] = content; // 更新最后的内容
            if (filePath == "/proc/rp_gpio/gpio3a2")
            {
            }
            else if (filePath == "/proc/rp_gpio/gpio3b3")
            {
                if(ebutton->cam->y_offset<55)
                    ebutton->cam->y_offset++;
                saveDataToFile(ebutton->cam->x_offset,ebutton->cam->y_offset);
            }
            else if (filePath == "/proc/rp_gpio/gpio3b4")
            {
                if(ebutton->cam->y_offset>-55)
                    ebutton->cam->y_offset--;
                saveDataToFile(ebutton->cam->x_offset,ebutton->cam->y_offset);
            }
            else if (filePath == "/proc/rp_gpio/gpio3a4")
            {
                if(ebutton->cam->x_offset>-55)
                    ebutton->cam->x_offset--;
                saveDataToFile(ebutton->cam->x_offset,ebutton->cam->y_offset);
            }
            else if (filePath == "/proc/rp_gpio/gpio3a3")//you
            {
                if(ebutton->cam->x_offset<55)ebutton->cam->x_offset++;
                saveDataToFile(ebutton->cam->x_offset,ebutton->cam->y_offset);
            }
            else if (filePath == "/proc/rp_gpio/gpio3b5")
            {
            }
            else if (filePath == "/proc/rp_gpio/gpio3a1")
            {

            }
            else if (filePath == "/proc/rp_gpio/gpio3a0")
            {

            }
        }
        else if (content != "0")
        {
            lastContents[filePath] = content; // 更新最后的内容
        }
    }
    else
    {
        qDebug() << "Failed to open file:" << file.errorString();
    }
}
void printFileContentIfZero(const QString &filePath, QObject *rootObject, E_button *ebutton)
{
    if (!rootObject || !ebutton || !ebutton->uv) return;

    aa[0] = ebutton->uv->m_gain_factor;
    if ((aa[2] = ebutton->biaozhiwei_jishu) == 1)
        aa[1] = ebutton->uv->m_photons;
    else
        aa[1] = ebutton->uv->m_photons;
    if(aa[1]>99999)
        aa[1] = 99990;
    aa[3] = ebutton->haomi;
    aa[5] = ebutton->biaozhiwei_jishu;
    aa[4] = ebutton->m_mssz;
    aa[10] = ebutton->w;
    aa[11] = ebutton->s;
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        QString content = in.readAll().trimmed(); // 读取并去除空白
        file.close();

        // 检查内容是否为0，并与上次内容比较
        if (content == "0" && lastContents[filePath] != content)
        {
            lastContents[filePath] = content; // 更新最后的内容
            if (filePath == "/proc/rp_gpio/gpio3a2")
            {
                qDebug() << "File" << filePath << "content is zero.";
                if (QObject *dialog = rootObject->findChild<QObject *>("dialog"))
                {
                    if (QMetaObject::invokeMethod(dialog, "opened"))
                        QMetaObject::invokeMethod(dialog, "close");
                }
                if (QObject *taizhang = rootObject->findChild<QObject *>("taizhang"))
                {
                    if (QMetaObject::invokeMethod(taizhang, "opened"))
                        QMetaObject::invokeMethod(taizhang, "close");
                }
                if (QObject *settingsDialog2222 = rootObject->findChild<QObject *>("shezhi"))
                {
                    settingsDialog2222->setProperty("visible",false);
                }
                if (QObject *yanse = rootObject->findChild<QObject *>("yanse"))
                {
                    yanse->setProperty("visible", false);
                }
                if (QObject *xiao_1 = rootObject->findChild<QObject *>("1"))
                {
                    xiao_1->setProperty("visible", false);
                }
                if (QObject *xiao_2 = rootObject->findChild<QObject *>("2"))
                {
                    xiao_2->setProperty("visible", false);
                }
                if (QObject *xiao_3 = rootObject->findChild<QObject *>("3"))
                {
                    xiao_3->setProperty("visible", false);
                }
                if (QObject *xiao_4 = rootObject->findChild<QObject *>("4"))
                {
                    xiao_4->setProperty("visible", false);
                }
                if (QObject *xiao_5 = rootObject->findChild<QObject *>("5"))
                {
                    xiao_5->setProperty("visible", false);
                }
                if (QObject *taizhangwenjian = rootObject->findChild<QObject *>("taizhangwenjian"))
                {
                    if (QMetaObject::invokeMethod(taizhangwenjian, "opened"))
                        QMetaObject::invokeMethod(taizhangwenjian, "close");
                }
                system("killall qplayer");
            }
            else if (filePath == "/proc/rp_gpio/gpio3b3")
            {
                if (QObject *drawer = rootObject->findChild<QObject *>("drawer"))
                    QMetaObject::invokeMethod(drawer, "open");
            }
            else if (filePath == "/proc/rp_gpio/gpio3b4")
            {
                if (QObject *drawer = rootObject->findChild<QObject *>("drawer"))
                    QMetaObject::invokeMethod(drawer, "close");
            }
            else if (filePath == "/proc/rp_gpio/gpio3a3")
            {
                QObject *border1, *border2, *border3, *border4, *border5, *border6, *border7, *border8, *border9;
                switch (n_button)
                {
                case 1:
                    border1 = rootObject->findChild<QObject *>("border1");
                    if (border1) border1->setProperty("visible", true);
                    break;
                case 2:
                    border1 = rootObject->findChild<QObject *>("border1");
                    border2 = rootObject->findChild<QObject *>("border2");
                    if (border1) border1->setProperty("visible", true);
                    if (border2) border2->setProperty("visible", false);
                    n_button = 1;
                    break;
                case 3:
                    border2 = rootObject->findChild<QObject *>("border2");
                    border3 = rootObject->findChild<QObject *>("border3");
                    if (border2) border2->setProperty("visible", true);
                    if (border3) border3->setProperty("visible", false);
                    n_button = 2;
                    break;
                case 4:
                    border3 = rootObject->findChild<QObject *>("border3");
                    border4 = rootObject->findChild<QObject *>("border4");
                    if (border3) border3->setProperty("visible", true);
                    if (border4) border4->setProperty("visible", false);
                    n_button = 3;
                    break;
                case 5:
                    border4 = rootObject->findChild<QObject *>("border4");
                    border5 = rootObject->findChild<QObject *>("border5");
                    if (border4) border4->setProperty("visible", true);
                    if (border5) border5->setProperty("visible", false);
                    n_button = 4;
                    break;
                case 6:
                    border5 = rootObject->findChild<QObject *>("border5");
                    border6 = rootObject->findChild<QObject *>("border6");
                    if (border5) border5->setProperty("visible", true);
                    if (border6) border6->setProperty("visible", false);
                    n_button = 5;
                    break;
                case 7:
                    border6 = rootObject->findChild<QObject *>("border6");
                    border7 = rootObject->findChild<QObject *>("border7");
                    if (border6) border6->setProperty("visible", true);
                    if (border7) border7->setProperty("visible", false);
                    n_button = 6;
                    break;
                case 8:
                    border7 = rootObject->findChild<QObject *>("border7");
                    border8 = rootObject->findChild<QObject *>("border8");
                    if (border7) border7->setProperty("visible", true);
                    if (border8) border8->setProperty("visible", false);
                    n_button = 7;
                    break;
                case 9:
                    border8 = rootObject->findChild<QObject *>("border8");
                    border9 = rootObject->findChild<QObject *>("border9");
                    if (border8) border8->setProperty("visible", true);
                    if (border9) border9->setProperty("visible", false);
                    n_button = 8;
                    break;
                }
            }
            else if (filePath == "/proc/rp_gpio/gpio3a4")
            {
                QObject *border1, *border2, *border3, *border4, *border5, *border6, *border7, *border8, *border9;
                switch (n_button)
                {
                case 1:
                    border1 = rootObject->findChild<QObject *>("border1");
                    border2 = rootObject->findChild<QObject *>("border2");
                    if (border1) border1->setProperty("visible", false);
                    if (border2) border2->setProperty("visible", true);
                    n_button = 2;
                    break;
                case 2:
                    border2 = rootObject->findChild<QObject *>("border2");
                    border3 = rootObject->findChild<QObject *>("border3");
                    if (border2) border2->setProperty("visible", false);
                    if (border3) border3->setProperty("visible", true);
                    n_button = 3;
                    break;
                case 3:
                    border3 = rootObject->findChild<QObject *>("border3");
                    border4 = rootObject->findChild<QObject *>("border4");
                    if (border3) border3->setProperty("visible", false);
                    if (border4) border4->setProperty("visible", true);
                    n_button = 4;
                    break;
                case 4:
                    border4 = rootObject->findChild<QObject *>("border4");
                    border5 = rootObject->findChild<QObject *>("border5");
                    if (border4) border4->setProperty("visible", false);
                    if (border5) border5->setProperty("visible", true);
                    n_button = 5;
                    break;
                case 5:
                    border5 = rootObject->findChild<QObject *>("border5");
                    border6 = rootObject->findChild<QObject *>("border6");
                    if (border5) border5->setProperty("visible", false);
                    if (border6) border6->setProperty("visible", true);
                    n_button = 6;
                    break;
                case 6:
                    border6 = rootObject->findChild<QObject *>("border6");
                    border7 = rootObject->findChild<QObject *>("border7");
                    if (border6) border6->setProperty("visible", false);
                    if (border7) border7->setProperty("visible", true);
                    n_button = 7;
                    break;
                case 7:
                    border7 = rootObject->findChild<QObject *>("border7");
                    border8 = rootObject->findChild<QObject *>("border8");
                    if (border7) border7->setProperty("visible", false);
                    if (border8) border8->setProperty("visible", true);
                    n_button = 8;
                    break;
                case 8:
                    border8 = rootObject->findChild<QObject *>("border8");
                    border9 = rootObject->findChild<QObject *>("border9");
                    if (border8) border8->setProperty("visible", false);
                    if (border9) border9->setProperty("visible", true);
                    n_button = 9;
                    break;
                case 9:
                    border9 = rootObject->findChild<QObject *>("border9");
                    if (border9) border9->setProperty("visible", true);
                    break;
                }
            }
            else if (filePath == "/proc/rp_gpio/gpio3b5")
            {
                QObject *button1, *button2, *button3, *button4, *button5, *button6, *button7, *button8, *button9;
                switch (n_button)
                {
                case 1:
                    button1 = rootObject->findChild<QObject *>("button1");
                    if (button1) QMetaObject::invokeMethod(button1, "clicked");
                    break;
                case 2:
                    button2 = rootObject->findChild<QObject *>("button2");
                    if (button2) QMetaObject::invokeMethod(button2, "clicked");
                    break;
                case 3:
                    button3 = rootObject->findChild<QObject *>("button3");
                    if (button3) QMetaObject::invokeMethod(button3, "clicked");
                    break;
                case 4:
                    button4 = rootObject->findChild<QObject *>("button4");
                    if (button4) QMetaObject::invokeMethod(button4, "clicked");
                    break;
                case 5:
                    button5 = rootObject->findChild<QObject *>("button5");
                    if (button5) QMetaObject::invokeMethod(button5, "clicked");
                    break;
                case 6:
                    button6 = rootObject->findChild<QObject *>("button6");
                    if (button6) QMetaObject::invokeMethod(button6, "clicked");
                    break;
                case 7:
                    button7 = rootObject->findChild<QObject *>("button7");
                    if (button7) QMetaObject::invokeMethod(button7, "clicked");
                    break;
                case 8:
                    button8 = rootObject->findChild<QObject *>("button8");
                    if (button8) QMetaObject::invokeMethod(button8, "clicked");
                    break;
                case 9:
                    button9 = rootObject->findChild<QObject *>("button9");
                    if (button9) QMetaObject::invokeMethod(button9, "clicked");
                    break;
                }
            }
            else if (filePath == "/proc/rp_gpio/gpio3a1")
            {
                QDir dir("/mnt/sdcard");
                if (dir.count() == 2)
                {
                    QObject *windowsss = rootObject->findChild<QObject *>("windowsss");
                    if (windowsss) windowsss->setProperty("visible", true);
                    return;
                }
                else
                    qDebug() << dir.count();

                QObject *flashScreen = rootObject->findChild<QObject *>("flashScreen");
                QObject *flashTimer = rootObject->findChild<QObject *>("flashTimer");
                flashScreen->setProperty("visible", true);
                QMetaObject::invokeMethod(flashTimer, "start");
                ebutton->bgra8888();
            }
            else if (filePath == "/proc/rp_gpio/gpio3a0")
            {
                QDir dir("/mnt/sdcard");
                if (dir.count() == 2)
                {
                    QObject *windowsss = rootObject->findChild<QObject *>("windowsss");
                    if (windowsss) windowsss->setProperty("visible", true);
                    return;
                }
                else
                    qDebug() << dir.count();
                QObject *ruxianti = rootObject->findChild<QObject *>("ruxianti");
                QObject *l_l = rootObject->findChild<QObject *>("l_l");
                QObject *ji_s = rootObject->findChild<QObject *>("ji_s");

                if (n_ebutton == 0)
                {
                    if (ebutton->startluzhi())
                    {
                        if (ruxianti) QMetaObject::invokeMethod(ruxianti, "start");
                        if (ji_s) QMetaObject::invokeMethod(ji_s, "start");
                        if (l_l) l_l->setProperty("visible", true);
                        n_ebutton = 1;
                    }
                }
                if (n_ebutton == 1)
                {
                    n_ebutton = 2;
                }
                else if (n_ebutton == 2)
                {
                    ebutton->overluzhi();
                    if (ruxianti) QMetaObject::invokeMethod(ruxianti, "stop");
                    if (ji_s) QMetaObject::invokeMethod(ji_s, "stop");
                    QObject *ruxian = rootObject->findChild<QObject *>("ruxian");
                    if (ruxian) ruxian->setProperty("visible", false);
                    if (l_l) {
                        l_l->setProperty("visible", false);
                        l_l->setProperty("text", "00:00:00");
                    }
                    n_ebutton = 0;
                }
            }
        }
        else if (content != "0")
        {
            lastContents[filePath] = content; // 更新最后的内容
        }
    }
    else
    {
        qDebug() << "Failed to open file:" << file.errorString();
    }
}

int main(int argc, char *argv[])
{

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    // qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    QGuiApplication app(argc, argv);

    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);

    qmlRegisterType<HotColdInfo>("IRSDK", 1, 0, "HotColdInfo");
    qmlRegisterType<RuleTemper>("IRSDK", 1, 0, "RuleTemper");
    qmlRegisterType<IRTool>("IRSDK", 1, 0, "IRTool");
    // QString command = "mv /mnt/sdcard/bofang.jpg /opt/"; // 或者在 Windows 上使用 "dir"
    // QStringList arguments;   // 可根据需要添加参数，例如：arguments << "-l"
    //system("mv /mnt/sdcard/bofang.jpg /opt/");

    //executeCommand(command, arguments);


    camera_blend *cam = new camera_blend();
    //cam->setxy(0, 0);

    // MyrkDecode *decode = MyrkDecode::getInstance();
    // if(decode)
    //     decode->initDecode();
    // decode->sendStreamThread();
    // decode->getFrameThread();

    DrmDisplay *drm = DrmDisplay::getInstance();
    if (drm != NULL)
        drm->initDrm();
    // drm->showFrameThread();
    UV_information_service *uv = new UV_information_service();
    if (uv != NULL)
    {
        uv->init();
    }

    bool camInitOk = cam->camBlendThreadStart(drm, outPutFunc, osd_date);
    if (!camInitOk)
    {
        qWarning() << "camera_blend initialization failed! Camera and recording features will be unavailable.";
    }

    E_button *ebutton = new E_button();
    ebutton->sdcard();
    unsigned int *aaa = &ebutton->uv->m_photons;


    qmlRegisterType<E_button>("com.external", 1, 0, "E_button");
    ebutton->drm = drm;
    ebutton->cam = cam;
    readDataFromFile(ebutton);
    ebutton->cam->swtich_photons_color(0xFFFFFFFF);
    ebutton->uv = uv;
    ebutton->is = new ID_R_service();
    if (ebutton->is->init() == true)
    {
        qDebug() << "init succes -------51";
    }
    else
    {
        qDebug() << "init error ------56";
    }
    ebutton->gps = new GPS_service();
    if (ebutton->gps->GPS_init() == true)
    {
    }

    ebutton->cam->switch_osd_language(1);

    QQmlApplicationEngine engine;

    QString path = "/usr/local/bin/widget_file/cn_big_screen"; // 替换为你的指定路径
    engine.rootContext()->setContextProperty("imagePath", path);

    engine.rootContext()->setContextProperty("ebutton", ebutton);

    const QUrl url(QStringLiteral("qrc:/main.qml"));


    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject *obj, const QUrl &objUrl)
                     {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1); }, Qt::QueuedConnection);

    engine.load(url);


    checkAndCreateFile("/opt/","c_mv");
    // 读取上一次语言设置
    checkAndCreateFile("/opt/","l_mv");
    checkAndCreateFile("/opt/","jf_mv");
    checkAndCreateFile("/opt/","jishu_mv");
    checkAndCreateFile("/opt/","ceju_mv");
    checkAndCreateFile("/opt/","quezhi_mv");
    checkAndCreateFile("/opt/","quezhi_sw_mv");
    checkAndCreateFile("/opt/","gps_mv");
    int c_mv = readDigitFromFile("/opt/c_mv");
    int l_mv = readDigitFromFile("/opt/l_mv");
    int jf_mv = readDigitFromFile("/opt/jf_mv");
    int jishu_mv = readDigitFromFile("/opt/jishu_mv");
    int ceju_mv = readDigitFromFile("/opt/ceju_mv");
    int quezhi_mv = readDigitFromFile("/opt/quezhi_mv");
    int quezhi_sw_mv = readDigitFromFile("/opt/quezhi_sw_mv");
    int gps_mv = readDigitFromFile("/opt/gps_mv");
    ebutton->setmsqh(c_mv);
    ebutton->setyuyan(l_mv);
    ebutton->setjf(jf_mv);
    ebutton->xxxxxjishu(jishu_mv);
    ebutton->settjiguang(ceju_mv);
    ebutton->setquezhi(QString::number(quezhi_mv));
    ebutton->setquezhi_sw(quezhi_sw_mv);
    // gps_mv 在 QML 端处理

    QObject *rootObject = engine.rootObjects().isEmpty() ? nullptr : engine.rootObjects().first();
    ebutton->rootObject = rootObject;


    QStringList filePaths = {
        "/proc/rp_gpio/gpio3a0",
        "/proc/rp_gpio/gpio3a1",
        "/proc/rp_gpio/gpio3a2",
        "/proc/rp_gpio/gpio3a3",
        "/proc/rp_gpio/gpio3a4",
        "/proc/rp_gpio/gpio3b3",
        "/proc/rp_gpio/gpio3b4",
        "/proc/rp_gpio/gpio3b5",
        // 添加更多文件路径
    };

    // 初始化每个文件的上次内容
    for (const QString &filePath : filePaths)
    {
        lastContents[filePath] = ""; // 初始化为空
    }

    QTimer timer;
    QString changefile = "/mnt/sdcard/change";
    QObject::connect(&timer, &QTimer::timeout, [&]()
                     {
        for (const QString &filePath : filePaths) {
            if (QFile::exists(changefile)) {

                change(filePath,rootObject,ebutton);
            } else {

                printFileContentIfZero(filePath,rootObject,ebutton);
            }

        } });
    timer.start(1);
    QDir dir("/mnt/sdcard");
    if (dir.count() == 2)
    {
        QObject *windowsss = rootObject->findChild<QObject *>("windowsss");
        windowsss->setProperty("visible", true);
    }
    else
        qDebug() << dir.count();
    // 设置过滤器，只获取 .jpg 文件
    QStringList filters;
    filters << "*.jpg";
    QStringList filtersmp4;
    filtersmp4 << "*.h264";
    QDir dirjpg("/mnt/sdcard/Standard Storage");
    // 找到以J开头的jpg文件的最大编号
    int jpgCount = 0;
    QStringList jpgFiles = dirjpg.entryList(filters, QDir::Files);
    for (const QString &file : jpgFiles) {
        if (file.startsWith("J") && file.endsWith(".jpg")) {
            QString numStr = file.mid(1, file.length() - 5); // 去掉"J"和".jpg"
            bool ok;
            int num = numStr.toInt(&ok);
            if (ok && num > jpgCount) {
                jpgCount = num;
            }
        }
    }
    int jpgCountmp4 = dirjpg.entryList(filtersmp4, QDir::Files).size();
    ebutton->sizejpg = jpgCount;
    ebutton->sizemp4 = jpgCountmp4;



    // app.exec();

    // QApplication a(argc, argv);
    // MainWindow w;
    // w.show();




    return app.exec();
}
void outPutFunc(void *data, size_t len, uint8_t type, bool isEnd)
{
    if (pRtpSendPs != nullptr)
    {
        pRtpSendPs->SendData(data, len, type, isEnd);
    }
}
//注意：开始前先关掉录像功能,可以只传入remoteRtpIp,remoteRtpPort,localRtpPort
void start_rtp_send(const char *remoteRtpIp, int remoteRtpPort, int localRtpPort, bool isTcp, uint32_t ssrc)
{
    qDebug()<<remoteRtpIp<<remoteRtpPort<<localRtpPort;
    if (pRtpSendPs)
    {
        pRtpSendPs->stop();
        delete pRtpSendPs;
        pRtpSendPs = nullptr;
    }
    pRtpSendPs = new RtpSendPs(remoteRtpIp,
                               remoteRtpPort,
                               localRtpPort,
                               isTcp,
                               ssrc);
    pRtpSendPs->start();
}

void stop_rtp_send()
{
    if (pRtpSendPs)
    {
        pRtpSendPs->stop();
        delete pRtpSendPs;
        pRtpSendPs = nullptr;
    }
}
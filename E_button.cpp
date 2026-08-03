#include "E_button.h"
#include "USBLib.h"
#include <QDebug>
#include <QProcess>
#include <QGuiApplication>
#include <QCoreApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QImage>
#include <QBuffer>
#include <QThread>
#include <QDir>
#include <QUrl>
#include <QElapsedTimer>
#include <QString>
#include <QByteArray>
#include <QPainter>
#include <QDateTime>
#include <unistd.h>
#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"
#include <QDebug>
#include <QStringList>
#include <QDir>
#include <stdlib.h>
#include <QTimeZone>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

static int biaozhiwei_cishu = 0;
static int biaozhiwei_cishu_vl = 0;
static int biaozhiwei_cishu_sc = 0;




E_button::E_button(QObject *parent)
    : QObject{parent}
{
    this->filew = new QFile("/sys/class/hwmon/hwmon10/humidity1_input");
    this->files = new QFile("/sys/class/hwmon/hwmon10/temp1_input");

    if (!filew->open(QIODevice::ReadOnly)) {
        qWarning() << "无法打开文件";
    }
    if (!files->open(QIODevice::ReadOnly)) {
        qWarning() << "无法打开文件-------36";
    }




}
void E_button::switch_l(int i){
    this->cam->switch_osd_language(i);
}
void E_button::setIRColor(int index){
    USBSDK_SetColor(index);
}
static int stream = 0;
char test_ipbuffer[16];
void E_button::pushstream(){
    //qDebug()<<this->cam->ip<<"----"<<this->cam->port;
    if(stream == 0){
        strncpy(test_ipbuffer, this->cam->ip.toUtf8().constData(), sizeof(test_ipbuffer) - 1); // 安全地复制字符串，避免溢出
        test_ipbuffer[sizeof(test_ipbuffer) - 1] = '\0'; // 确保字符串以 null 结尾

        start_rtp_send(test_ipbuffer, this->cam->port, 10011, 0, 100);

        stream = 1;
    }else{
        stop_rtp_send();
        stream = 0;
    }
}
int E_button::getguagzishu(){
    return this->guangzishu;
}
int E_button::getyy(){
    return this->yy;
}
int E_button::getday(){
    return this->dd;
}
int E_button::getnnnn(){
    return this->nnnn;
}
int E_button::gets(){
    return this->ss;
}
int E_button::getf(){
    return this->ff;
}
int E_button::getm(){
    return this->miao;
}
void E_button::sdcard(){


    // 检查设备是否存在
    if (system("test -b /dev/mmcblk1p1") == 0)
    {
        printf("/dev/mmcblk1p1 设备存在。\n");

        // 检查设备是否已挂载
        if (system("mount | grep -q '/dev/mmcblk1p1'") == 0)
        {
            if (system("mount | grep '/dev/mmcblk1p1' | grep -q '/mnt/sdcard'") == 0)
            {
                QDir dir;

                // 你想创建的目录路径
                QString pt_path = "/mnt/sdcard/Standard Storage";

                // 检查目录是否存在
                if (!dir.exists(pt_path)) {
                    // 如果目录不存在，创建它
                    if (dir.mkpath(pt_path)) {
                        qDebug() << "目录创建成功：" << pt_path;
                    } else {
                        qDebug() << "创建目录失败：" << pt_path;
                    }
                } else {
                    qDebug() << "目录已经存在：" << pt_path;
                }
                // 你想创建的目录路径
                QString tz_path = "/mnt/sdcard/Ledger storage";

                // 检查目录是否存在
                if (!dir.exists(tz_path)) {
                    // 如果目录不存在，创建它
                    if (dir.mkpath(tz_path)) {
                        qDebug() << "目录创建成功：" << tz_path;
                    } else {
                        qDebug() << "创建目录失败：" << tz_path;
                    }
                } else {
                    qDebug() << "目录已经存在：" << tz_path;
                }
                printf("设备已挂载到 /mnt/sdcard/。\n");
                return;
            }
            else
            {
                printf("设备未挂载到 /mnt/sdcard/，正在处理...\n");
                // 备份数据到当前挂载点
                system("cp -r /mnt/sdcard/* $(mount | grep '/dev/mmcblk1p1' | awk '{print $3}')/");
            }
        }
        else
        {
            printf("设备未挂载，正在创建临时目录并挂载...\n");
            // 创建临时挂载点
            system("mkdir -p /tmp/sdcard_temp");
            // 将SD卡挂载到临时目录
            system("mount /dev/mmcblk1p1 /tmp/sdcard_temp");
            // 备份数据到临时目录
            system("cp -r /mnt/sdcard/* /tmp/sdcard_temp/");
        }
        // 卸载SD卡
        system("umount /dev/mmcblk1p1");
        // 清空/mnt/sdcard/目录下的内容
        system("rm -rf /mnt/sdcard/*");

        // 将SD卡挂载到/mnt/sdcard/
        system("mount /dev/mmcblk1p1 /mnt/sdcard/");

        printf("SD卡已挂载到 /mnt/sdcard/ 并且原始数据已保留。\n");
    }
    else
    {
        printf("/dev/mmcblk1p1 设备不存在。\n");
    }

}



int E_button::getmoshi(){
    return this->m_mssz;
}


// void E_button::setmoshi(int value){
//     if (this->m_mssz != value) {
//         this->m_mssz = value;
//         emit msszChanged();
//     }
// }


void E_button::setmoshi(int value){
    if (this->m_mssz != value) {
        qDebug() << "Setting mssz from" << this->m_mssz << "to" << value;
        this->m_mssz = value;
        emit msszChanged();
        qDebug() << "Signal emitted";
    }
}

int E_button::getyuyan(){
    return this->biaozhiwei_yuyan;
}


void E_button::setjf(int js){
    this->uv->m_integration = js;
    this->biaozhiwei_jf = js;
    writeDigitToFile("/opt/jf_mv", js);
}
void E_button::setyuyan(int i){
    if(i == 0){
        this->cam->switch_osd_language(1);
        std::ofstream outFile("/opt/l_mv", std::ios::trunc); // 使用 trunc 标志覆盖文件内容
        if (!outFile) {
            std::cerr << "无法打开文件进行写入。" << std::endl;
            exit(1); // 返回非零值表示错误
        }
        outFile << i; // 写入数字
        outFile.close(); // 关闭文件
    }
    else if(i == 1){
        this->cam->switch_osd_language(0);
        std::ofstream outFile("/opt/l_mv", std::ios::trunc); // 使用 trunc 标志覆盖文件内容
        if (!outFile) {
            std::cerr << "无法打开文件进行写入。" << std::endl;
            exit(1); // 返回非零值表示错误
        }
        outFile << i; // 写入数字
        outFile.close(); // 关闭文件
    }
    else{
        this->cam->switch_osd_language(0);
        std::ofstream outFile("/opt/l_mv", std::ios::trunc); // 使用 trunc 标志覆盖文件内容
        if (!outFile) {
            std::cerr << "无法打开文件进行写入。" << std::endl;
            exit(1); // 返回非零值表示错误
        }
        outFile << i; // 写入数字
        outFile.close(); // 关闭文件
    }
    this->biaozhiwei_yuyan = i;
    emit yuyanChanged();

}

bool E_button::settime(QString n,QString y,QString r,QString s,QString f,QString m){

    this->nnnn = n.toInt();
    this->yy = y.toInt();
    this->dd = r.toInt();
    this->ss = s.toInt();
    this->ff = f.toInt();
    this->miao = m.toInt();

    // 通过i2ctransfer写入RTC寄存器 0x02~0x08
    int year  = this->nnnn;
    int month = this->yy;
    int day   = this->dd;
    int hour  = this->ss;
    int min   = this->ff;
    int sec   = this->miao;

    // 十进制转BCD
    auto dec2bcd = [](int val) -> int {
        return ((val / 10) << 4) | (val % 10);
    };

    int sec_bcd  = dec2bcd(sec);
    int min_bcd  = dec2bcd(min);
    int hour_bcd = dec2bcd(hour);
    int day_bcd  = dec2bcd(day);
    int year_bcd = dec2bcd(year % 100);
    int month_bcd = dec2bcd(month);

    // 计算星期几 (0=周日, RTC中周日=0)
    QDate qdate(year, month, day);
    int weekday = qdate.dayOfWeek() % 7; // Qt: 1=周一...7=周日 → 转换为0=周日

    // 通过i2ctransfer写入RTC寄存器 0x02~0x08
    char i2c_cmd[256];
    snprintf(i2c_cmd, sizeof(i2c_cmd),
             "i2ctransfer -f -y 4 w8@0x51 0x02 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
             sec_bcd, min_bcd, hour_bcd, day_bcd, weekday, month_bcd, year_bcd);
    system(i2c_cmd);

    this->shijiankaiguan =1;
    return true;
}

bool E_button::setquezhi(QString quzhi){
    this->quezhi = quzhi.toInt();
    writeDigitToFile("/opt/quezhi_mv", this->quezhi);
    return true;
}

Q_INVOKABLE void E_button::setquezhi_sw(int onoff)
{
    this->biaozhiwei_quezhi_sw = onoff;
    writeDigitToFile("/opt/quezhi_sw_mv", onoff);
}

Q_INVOKABLE int E_button::getquezhi_sw()
{
    return this->biaozhiwei_quezhi_sw;
}
QString E_button::get1(){
    return QString::number(this->uv->m_first_photons);
}
QString E_button::get2(){
    return QString::number(this->uv->m_second_photons);
}
QString E_button::get3(){

    return QString::number(this->uv->m_third_photons);
}
QString E_button::get4(){
    return QString::number(this->uv->m_fourth_photons);
}

void E_button::taizhangdaoru(QString st1){
    if (st1.endsWith(".xlsx", Qt::CaseInsensitive)) {
        qDebug() << "文件名以 .xlsx 结尾";
    } else {
        return;
    }
    QString st =  st1;
    qDebug() << st;
    QXlsx::Document xlsx(st);
    QObject *contentModel = this->rootObject->findChild<QObject*>("contentModel");
    QMetaObject::invokeMethod(contentModel,"clear");
    QXlsx::CellRange dimensions = xlsx.dimension();
    int rowCount = dimensions.rowCount(); // 获取总行数
    int colCount = dimensions.columnCount(); // 获取总列数
    QString name;
    for (int row = 1; row <= rowCount; ++row) {
        name.clear();
        for (int col = 1; col <= colCount; ++col) {
            name = name+xlsx.read(row, col).toString();
        }
        QMetaObject::invokeMethod(this->rootObject, "addItem", Q_ARG(QVariant, name));
    }
}

void E_button::bofangshipin(QString st){
    // 先杀掉之前可能残留的播放器进程，避免冲突闪屏
    // system("pkill -9 qplayer 2>/dev/null");
    // usleep(200000); // 等 0.2s 确保旧进程退出、显示资源释放

    QProcess *process = new QProcess(this);

    // 构造命令
    QString program = "/usr/bin/qplayer";

    // 启动进程
    process->start(program, QStringList() << st);

    // 检查进程是否成功启动
    if (!process->waitForStarted()) {
        qDebug() << "Failed to start process:" << process->errorString();
    }
}
int biaozhiwei_ms = 0;
int E_button::moshiqiehuan(){

    //qDebug() << this->mssz;

    qDebug() << this->m_mssz;

    biaozhiwei_cishu_vl = 0;
    biaozhiwei_cishu = 0;
    biaozhiwei_cishu_sc = 0;

    if(this->m_irZoomLevel != 1)
    {
        this->cam->setIrZoom(1);
        this->m_irZoomLevel = 1;
    }

    //this->mssz = (this->mssz + 1) % 4;

    //this->m_mssz = (this->m_mssz + 1) % 4;
    int newMode = (this->m_mssz + 1) % 5;
    this->setmoshi(newMode);


    // if(this->mssz == 1 && biaozhiwei_ms == 0){
    //     this->mssz = 2;
    //     biaozhiwei_ms = 1;
    // }else if(this->mssz == 2){
    //     this->mssz = 1;
    // }else if(this->mssz == 1 && biaozhiwei_ms == 1){
    //     this->mssz = 0;
    //     biaozhiwei_ms = 0;
    // }else if(this->mssz == 0){
    //     this->mssz = 1;
    // }
    switch (newMode) {
    case 0:
        this->cam->switch_mode(VL_Mode);
        biaozhiwei_cishu_vl = 0;
        for(int i =0;i<6; i++)
            this->cam->zoom_out();
        break;
    case 1:
        this->cam->switch_mode(VL_UV_blendMode);
        for(int i =0;i<6; i++)
            this->cam->zoom_out();
        break;
    case 2:
        this->cam->switch_mode(UV_Mode);
        for(int i =0;i<6; i++)
            this->cam->zoom_out();
        break;
    case 3: //add IR mode
        this->cam->switch_mode(IR_Mode);
        for(int i =0;i<6; i++)
            this->cam->zoom_out();
        break;
    case 4: //add IR_UV_blend mode
        this->cam->switch_mode(IR_UV_blendMode);
        for(int i =0;i<6; i++)
            this->cam->zoom_out();
        break;


    default:
        break;
    }
    // if(this->mssz == 1){
    //     this->cam->switch_mode(VL_Mode);
    //     this->mssz = 0;
    // }else{
    //     this->cam->switch_mode(VL_UV_blendMode);
    //     this->mssz = 1;
    // }

    return this->m_mssz;
    //return this->mssz;
}

void E_button::setzywen(int i){
    this->biaozhiwei_jishu_1 = i;
    if(i==2||i==1){
        this->cam->switch_osd_language(1);

    }
    else{
        this->cam->switch_osd_language(0);
    }

}

QString E_button::xxxxxjishu(int i){
    // if(this->biaozhiwei_jishu == 0){
    //     this->biaozhiwei_jishu = 1;
    //     if(this->biaozhiwei_jishu_1==0)
    //         return "计数模式(/S)";
    //     else if(this->biaozhiwei_jishu_1==1)
    //         return "Counting mode(/S)";
    //     else
    //         return "Единица измерения (/S)";
    // }
    // else{
    //     this->biaozhiwei_jishu = 0;
    //     if(this->biaozhiwei_jishu_1==0)
    //         return "计数模式(/M)";
    //     else if(this->biaozhiwei_jishu_1==1)
    //         return "Counting mode(/M)";
    //     else
    //         return "Единица измерения (/M)";
    // }
    this->biaozhiwei_jishu = i;
    if(i == 0)
        uv->m_count_period = 1;
    else
        uv->m_count_period = 0;
    qDebug() << i;
    writeDigitToFile("/opt/jishu_mv", i);
    return "计数模式(/S)";
}

QString E_button::settjiguang(int i){
    if(i == 0){
        this->is->turn_on_laser();
        this->is->start_continue_measure();
        this->biaozhiwei_jiguang = 1;
        writeDigitToFile("/opt/ceju_mv", 0);
        if(this->biaozhiwei_jishu_1==0)
            return "开";
        else
            return "on";
    }else{
        this->is->turn_off_laser();
        this->biaozhiwei_jiguang = 0;
        writeDigitToFile("/opt/ceju_mv", 1);
        if(this->biaozhiwei_jishu_1==0)
            return "关";
        else
            return "off";
    }
}


// RTC BCD 转十进制（必须用这个！）
int bcd2dec(int bcd)
{
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}
QString E_button::gettime(){


    // 1. 执行 i2ctransfer 读取 RTC 7个字节
    QProcess process;
    process.start("i2ctransfer -f -y 4 w1@0x51 0x02 r7@0x51");
    process.waitForFinished();
    QString output = process.readAllStandardOutput().trimmed();

    // 输出示例：0x06 0x52 0x06 0x15 0x03 0x04 0x26
    qDebug() << "I2C 原始数据：" << output;

    // 2. 分割数据
    QStringList list = output.split(" ", Qt::SkipEmptyParts);
    if (list.size() < 7) {
        qDebug() << "读取失败";
        // 获取系统当前时间
        QDateTime localTime = QDateTime::currentDateTime();

        // 根据语言标识拼接时间字符串
        if(this->biaozhiwei_yuyan == 0)
            this->timeshijian = "时间：" + localTime.toString("yyyy-MM-dd HH:mm:ss");
        else if(this->biaozhiwei_yuyan == 1)
            this->timeshijian = "T：" + localTime.toString("yyyy-MM-dd HH:mm:ss");
        else if(this->biaozhiwei_yuyan == 4)
            this->timeshijian = "時間：" + localTime.toString("yyyy-MM-dd HH:mm:ss");
        else if(this->biaozhiwei_yuyan == 5)
            this->timeshijian = "T：" + localTime.toString("yyyy-MM-dd HH:mm:ss");
        else
            this->timeshijian = "Время：" + localTime.toString("yyyy-MM-dd HH:mm:ss");
        return this->timeshijian;
    }

    int year_bcd  = list[6].toInt(nullptr, 16);
    int min_bcd   = list[1].toInt(nullptr, 16);
    int hour_bcd  = list[2].toInt(nullptr, 16);
    int day_bcd   = list[3].toInt(nullptr, 16);
    int month_bcd = list[5].toInt(nullptr, 16);
    int sec_bcd   = list[0].toInt(nullptr, 16);

    // BCD 转正常数字
    int year  = bcd2dec(year_bcd) + 2000;
    int min   = bcd2dec(min_bcd);
    int hour  = bcd2dec(hour_bcd);
    int day   = bcd2dec(day_bcd);
    int month = bcd2dec(month_bcd);
    int sec   = bcd2dec(sec_bcd);

    // 提取年、月、日、时、分、秒
    this->nnnn = year;
    this->yy = month;
    this->dd = day;
    this->ss = hour;
    this->ff = min;
    this->miao = sec;

    // 构建时间
    QDate date(year, month, day);
    QTime time(hour, min, sec);
    if (!date.isValid() || !time.isValid()) {
        qDebug() << "日期或时间无效，回退到系统时间！";
        // RTC数据无效（如未初始化），使用系统当前时间
        QDateTime localTime = QDateTime::currentDateTime();

        if(this->biaozhiwei_yuyan == 0)
            this->timeshijian = "时间：" + localTime.toString("yyyy-MM-dd HH:mm:ss");
        else if(this->biaozhiwei_yuyan == 1)
            this->timeshijian = "T：" + localTime.toString("yyyy-MM-dd HH:mm:ss");
        else if(this->biaozhiwei_yuyan == 4)
            this->timeshijian = "時間：" + localTime.toString("yyyy-MM-dd HH:mm:ss");
        else if(this->biaozhiwei_yuyan == 5)
            this->timeshijian = "T：" + localTime.toString("yyyy-MM-dd HH:mm:ss");
        else
            this->timeshijian = "Время：" + localTime.toString("yyyy-MM-dd HH:mm:ss");
        return this->timeshijian;
    }

    QDateTime utcTime(date, time, Qt::UTC);
    QDateTime localTime = utcTime.addSecs(1);  // +8小时

    qDebug() << localTime;
    // 定义自定义格式
    if(this->biaozhiwei_yuyan == 0)
        this->timeshijian = "时间：" + localTime.toString("yyyy-MM-dd HH:mm:ss");
    else if(this->biaozhiwei_yuyan == 1)
        this->timeshijian = "T：" + localTime.toString("yyyy-MM-dd HH:mm:ss");
    else if(this->biaozhiwei_yuyan == 4)
        this->timeshijian = "時間：" + localTime.toString("yyyy-MM-dd HH:mm:ss");
    else if(this->biaozhiwei_yuyan == 5)
        this->timeshijian = "T：" + localTime.toString("yyyy-MM-dd HH:mm:ss");
    else
        this->timeshijian = "Время：" + localTime.toString("yyyy-MM-dd HH:mm:ss");


    return this->timeshijian;

}





QString E_button::readBatteryLevel(){
    QFile file("/sys/class/power_supply/bms/capacity");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open file for reading:" << file.errorString();
        return "Error";
    }
    QTextStream in(&file);
    QString level = in.readLine();
    file.close();

    return level;

}

void E_button::myFunction() {
    if(this->m_mssz == 3 || this->m_mssz == 4)
    {
        if(this->m_irZoomLevel == 1)
        {
            this->cam->setIrZoom(2);
            this->m_irZoomLevel = 2;
        }
        return;
    }
    if(this->m_mssz == 1 && biaozhiwei_cishu < 2)
    {
        this->cam->zoom_in();
        this->cam->zoom_in();
        biaozhiwei_cishu++;
        return;
    }
    if(this->m_mssz == 0 && biaozhiwei_cishu_vl == 6 && biaozhiwei_cishu_sc == 0)
    {
        for (int var = 0; var < 10; var++) {
            this->cam->zoom_in();
            usleep(1000 * 80);
        }
        biaozhiwei_cishu_sc = 1;
        return;
    }
    if(this->cam != NULL && biaozhiwei_cishu != 2){
        this->cam->zoom_in();
        if(biaozhiwei_cishu_vl<6)
        biaozhiwei_cishu_vl++;
    }

}
const QString zengyishuzhi = "/opt/zengyishuzhi"; // 固定路径

// 读取文件中的整数，返回-1表示读取失败
int readValue() {
    QFile file(zengyishuzhi);
    if (!file.exists()) {
        qDebug() << "文件不存在，正在创建文件...";
        // 文件不存在时创建文件
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "无法创建文件";
            return -1;
        }
        QTextStream out(&file);
        out << 0;  // 默认值为0
        file.close();
        qDebug() << "文件已创建并写入默认值0";
    } else {
        // 文件存在，读取数据
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "无法打开文件读取";
            return -1;
        }
        QTextStream in(&file);
        int value;
        in >> value;
        file.close();


        return value;
    }
    return 0;  // 默认返回值
}

// 将整数写入文件
bool writeValue(int value) {

    QFile file(zengyishuzhi);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件写入";
        return false;
    }

    QTextStream out(&file);
    out << value;
    file.close();
    return true;
}
void E_button::myFunction1() {
    qDebug() << this->uv->m_gain_factor;
    if(this->uv != NULL){
        if(this->uv->m_gain_factor<100)
            this->uv->m_gain_factor = this->uv->m_gain_factor + 1;
    }
    writeValue(this->uv->m_gain_factor);

}
void E_button::anotherFunction() {
    if(this->m_mssz == 3 || this->m_mssz == 4)
    {
        if(this->m_irZoomLevel == 2)
        {
            this->cam->setIrZoom(1);
            this->m_irZoomLevel = 1;
        }
        return;
    }
    if(this->m_mssz == 1&& biaozhiwei_cishu > 0){
        this->cam->zoom_out();
        this->cam->zoom_out();
        biaozhiwei_cishu--;
        return;
    }
    if(this->m_mssz == 0 && biaozhiwei_cishu_sc == 1)
    {
        for (int var = 0; var < 10; var++) {
            this->cam->zoom_out();
            usleep(1000 * 80);
        }
        // biaozhiwei_cishu_vl = biaozhiwei_cishu_vl-6;
        biaozhiwei_cishu_sc = 0;
        return;
    }
    if(this->cam != NULL){
        this->cam->zoom_out();
        if(biaozhiwei_cishu_vl>0)
        biaozhiwei_cishu_vl--;
    }


}
void E_button::anotherFunction1() {
    qDebug() << this->uv->m_gain_factor;
    if(this->uv != NULL){
        if(this->uv->m_gain_factor >= 2)
            this->uv->m_gain_factor = this->uv->m_gain_factor - 1;
    }
    writeValue(this->uv->m_gain_factor);
}

QString E_button::getzy() {
    QString zz1 = "%";
    this->uv->m_gain_factor = readValue();
    if(this->biaozhiwei_yuyan == 0)
        this->zy1 = this->zy + QString::number(this->uv->m_gain_factor)+zz1;
    else if(this->biaozhiwei_yuyan == 1)
        this->zy1 = "G:" + QString::number(this->uv->m_gain_factor)+zz1;
    else if(this->biaozhiwei_yuyan == 2)
        this->zy1 = "Усиление:" + QString::number(this->uv->m_gain_factor)+zz1;
    else if(this->biaozhiwei_yuyan == 4)
        this->zy1 = "ゲイン:" + QString::number(this->uv->m_gain_factor)+zz1;
    else if(this->biaozhiwei_yuyan == 5)
        this->zy1 = "G:" + QString::number(this->uv->m_gain_factor)+zz1;
    else
        this->zy1 = "G:" + QString::number(this->uv->m_gain_factor)+zz1;
    return this->zy1;
}


QString E_button::getws() {
    QString zz;
    if(this->biaozhiwei_yuyan == 0)
        zz = "温湿度：";
    else
        zz = "T/H:";

    // 确保文件已打开
    if (!filew->isOpen() && !filew->open(QIODevice::ReadOnly)) {
        //qWarning() << "无法打开文件 ------451";
        return zz;
    }
    filew->seek(0);
    files->seek(0);
    QTextStream streamw(filew);
    QTextStream streams(files);
    QString firstTwoCharsw = streamw.read(2);
    QString firstTwoCharss = streams.read(2);
    w = firstTwoCharsw.toInt();
    s = firstTwoCharss.toInt();
    // // 关闭文件
    // filew->close();

    this->wd = zz + firstTwoCharss+ "℃" + "/" + firstTwoCharsw + "%";
    return this->wd;
}


QString E_button::getgz() {
    QString zz;
    if(this->biaozhiwei_yuyan == 0)
        zz = "计数：";
    else if(this->biaozhiwei_yuyan == 1)
        zz = "C:";
    else if(this->biaozhiwei_yuyan == 2)
        zz= "Количество:";
    else if(this->biaozhiwei_yuyan == 4)
        zz = "カウント:";
    else if(this->biaozhiwei_yuyan == 5)
        zz = "C:";
    else
        zz = "C:";
    if(this->biaozhiwei_jishu == 0){
        QString zz1 = "/M";
        this->gz = zz + QString::number(this->uv->m_photons) + zz1;
        this->guangzishu = this->uv->m_photons;
    }
    else{
        QString zz1 = "/S";
        this->gz = zz + QString::number(this->uv->m_photons) + zz1;
        this->guangzishu = this->uv->m_photons;
    }

    //qDebug() << this->uv->m_photons;
    return this->gz;
}

QString E_button::getmm() {
    QString zz;
    if(this->biaozhiwei_yuyan == 0)
        zz = "距离：";
    else if(this->biaozhiwei_yuyan == 1)
        zz = "D:";
    else if(this->biaozhiwei_yuyan == 2)
        zz = "Расстояние:";
    else if(this->biaozhiwei_yuyan == 4)
        zz = "距離:";
    else if(this->biaozhiwei_yuyan == 5)
        zz = "D:";
    else
        zz = "D:";
    QString zz1 = "m";
    if(this->biaozhiwei_jiguang == 0)
        this->mm = zz+"0";
    else
        this->mm = zz + QString::number(this->is->get_distance() / 1000) + "."+QString::number(this->is->get_distance() % 100)+zz1;
    //qDebug() << this->uv->m_photons;
    this->haomi = this->is->get_distance() /100;
    return this->mm;
}
int biaozhiwei_gps = 0;
char gps_jw[128] = {0};
QString E_button::getgps()
{
    sprintf(gps_jw,"N:%.2f E:%.2f",this->gps->mGPS_info.longitude,this->gps->mGPS_info.latitude);
    return gps_jw;
}
QString E_button::getgps_te()
{
    return this->gps->ptr1;
}
void E_button::getrtc(){
        // 文件路径
        QString dateFile = "/sys/class/rtc/rtc0/date";
        QString timeFile = "/sys/class/rtc/rtc0/time";

        // 变量初始化
        int year, month, day, hour, minute, second;

        // 读取 date 文件（获取年月日）
        QFile dateFileObj(dateFile);
        if (dateFileObj.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&dateFileObj);
            QString date = in.readLine();
            QStringList dateParts = date.split('-');
            if (dateParts.size() == 3) {
                this->nnnn = dateParts[0].toInt();
                this->yy = dateParts[1].toInt();
                this->dd = dateParts[2].toInt();
            }
            dateFileObj.close();
        } else {
            qDebug() << "无法打开 date 文件";
        }

        // 读取 time 文件（获取时分秒）
        QFile timeFileObj(timeFile);
        if (timeFileObj.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&timeFileObj);
            QString time = in.readLine();
            QStringList timeParts = time.split(':');
            if (timeParts.size() == 3) {
                this->ss = timeParts[0].toInt();
                this->ff = timeParts[1].toInt();
                this->miao = timeParts[2].toInt();
            }
            timeFileObj.close();
        } else {
            qDebug() << "无法打开 time 文件";
        }


        biaozhiwei_gps = 1;
}
int E_button::setjs() {
    switch (this->uv->m_frame_type) {
    case 0:
        this->uv->m_frame_type = 1;
        break;
    case 1:
        this->uv->m_frame_type = 3;
        break;
    case 2:
        this->uv->m_frame_type = 0;
        break;
    case 3:
        this->uv->m_frame_type = 4;
        break;
    case 4:
        this->uv->m_frame_type = 0;
        break;
    default:
        break;
    }
    qDebug() << this->uv->m_frame_type;
    return this->uv->m_frame_type;
}
int wifi_poor = 0;
void E_button::openWifiSettings()
{
    // // 设置当前工作目录
    // QDir::setCurrent("/");

    // // 创建 QProcess 对象
    // QProcess *process = new QProcess();

    // // 启动可执行文件
    // QString program = "./bin/qsetting";  // 你要执行的可执行文件
    // QStringList arguments;  // 你可以在这里添加参数，例如：arguments << "arg1" << "arg2"

    // process->start(program, arguments);

    // // 确保 QProcess 对象在进程结束前不会被销毁
    // QObject::connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), process, SLOT(deleteLater()));


    // // 等待进程完成
    // if (process->waitForFinished()) {
    //     QByteArray output = process->readAllStandardOutput();
    //     qDebug() << "Output: " << output;
    // } else {
    //     qDebug() << "Process failed to start.";
    // }
    // 保留空实现，不再启动外部qsetting
}

// 解析 iw scan SSID 输出：解码 \xNN 转义，并提取纯 SSID 名称
static QString decodeHexEscapes(const QByteArray &raw)
{
    QStringList lines = QString::fromUtf8(raw).split('\n');
    QStringList ssids;
    for (const QString &line : lines) {
        QByteArray decoded;
        QByteArray utf8Line = line.toUtf8();
        int i = 0;
        while (i < utf8Line.size()) {
            if (i + 3 < utf8Line.size() && utf8Line[i] == '\\' && utf8Line[i+1] == 'x') {
                QByteArray hex = utf8Line.mid(i + 2, 2);
                bool ok;
                char byte = (char)hex.toInt(&ok, 16);
                if (ok) {
                    decoded.append(byte);
                    i += 4;
                    continue;
                }
            }
            decoded.append(utf8Line[i]);
            i++;
        }
        QString decodedLine = QString::fromUtf8(decoded).trimmed();
        // 去除 "SSID: " 前缀（不区分大小写）
        if (decodedLine.startsWith("SSID:", Qt::CaseInsensitive)) {
            decodedLine = decodedLine.mid(5).trimmed();
        }
        if (!decodedLine.isEmpty()) {
            ssids.append(decodedLine);
        }
    }
    return ssids.join("\n");
}

QString E_button::wifiScan()
{
    QProcess proc;
    // 获取原始 SSID 行 (格式: "SSID: xxx" 或 "SSID: \xNN..."), C++ 端解码十六进制转义
    proc.start("sh", QStringList() << "-c"
        << "iw dev wlan0 scan 2>/dev/null | grep -i 'SSID:'");
    proc.waitForFinished(10000);
    QByteArray raw = proc.readAllStandardOutput();
    if (raw.isEmpty()) {
        return "";
    }
    return decodeHexEscapes(raw);
}

QString E_button::wifiConnect(const QString &ssid, const QString &password)
{
    // 写入 wpa_supplicant.conf
    QFile file("/tmp/wpa_supplicant.conf");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return "ERROR: cannot write config";
    }
    QString config = QString(
        "ctrl_interface=DIR=/var/run/wpa_supplicant\n"
        "network={\n"
        "    ssid=\"%1\"\n"
        "    key_mgmt=WPA-PSK\n"
        "    psk=\"%2\"\n"
        "}\n"
    ).arg(ssid, password);
    file.write(config.toUtf8());
    file.close();

    // 杀死旧进程
    QProcess::execute("killall", QStringList() << "wpa_supplicant");
    QThread::msleep(500);

    // 启动 wpa_supplicant
    QProcess::execute("wpa_supplicant", QStringList()
        << "-iwlan0" << "-Dnl80211" << "-c" << "/tmp/wpa_supplicant.conf" << "-B");

    // 等待连接
    QThread::msleep(3000);

    // 检查状态
    return wifiStatus();
}

QString E_button::wifiStatus()
{
    QProcess proc;
    proc.start("wpa_cli", QStringList() << "-i" << "wlan0" << "status");
    proc.waitForFinished(5000);
    return QString::fromUtf8(proc.readAllStandardOutput()).trimmed();
}

// ========== 蓝牙功能 ==========

QString E_button::bluetoothCheck()
{
    // 先尝试加载蓝牙
    QProcess proc;
    proc.start("hciconfig", QStringList() << "hci0" << "up");
    proc.waitForFinished(3000);

    // 用 hciconfig -a 检测蓝牙硬件是否存在
    proc.start("hciconfig", QStringList() << "-a");
    proc.waitForFinished(3000);
    QString output = QString::fromUtf8(proc.readAllStandardOutput()).trimmed();
    QString errOutput = QString::fromUtf8(proc.readAllStandardError()).trimmed();

    // 返回为空说明没有蓝牙设备
    if (output.isEmpty() && errOutput.isEmpty()) {
        return "ERROR: No Bluetooth device";
    }
    if (output.contains("No such device") || errOutput.contains("No such device")) {
        return "ERROR: No such device";
    }
    return "OK";
}

QString E_button::bluetoothScan()
{
    // 先确保蓝牙开启
    QProcess proc;
    proc.start("hciconfig", QStringList() << "hci0" << "up");
    proc.waitForFinished(2000);

    // 后台扫描8秒，然后用 bluetoothctl devices 获取已发现设备列表
    QProcess scanProc;
    scanProc.start("sh", QStringList() << "-c"
        << "bluetoothctl scan on & sleep 8; bluetoothctl devices; killall bluetoothctl 2>/dev/null");
    scanProc.waitForFinished(12000);
    QString output = QString::fromUtf8(scanProc.readAllStandardOutput());

    // 解析 bluetoothctl devices 输出
    // 格式: Device AA:BB:CC:DD:EE:FF Device Name
    QStringList lines = output.split('\n');
    QStringList results;
    for (const QString &line : lines) {
        if (line.startsWith("Device ")) {
            QString rest = line.mid(7).trimmed(); // 去掉 "Device " 前缀
            if (rest.length() >= 17) {
                QString addr = rest.left(17);
                QString name = rest.mid(17).trimmed();
                if (name.isEmpty()) {
                    name = "(未知)";
                }
                results.append(addr + "," + name);
            }
        }
    }
    return results.join("\n");
}

QString E_button::bluetoothPair(const QString &addr)
{
    QProcess proc;
    QString allOutput;

    // 启动交互式 bluetoothctl 会话
    proc.start("bluetoothctl");
    proc.waitForStarted(3000);

    // 信任设备
    proc.write(QString("trust %1\n").arg(addr).toUtf8());
    proc.waitForBytesWritten(1000);

    // 发起配对
    proc.write(QString("pair %1\n").arg(addr).toUtf8());
    proc.waitForBytesWritten(1000);

    // 循环等待 passkey 确认提示
    bool replied = false;
    bool paired = false;
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < 25000 && !paired) {
        if (proc.waitForReadyRead(500)) {
            QByteArray data = proc.readAllStandardOutput();
            QString output = QString::fromUtf8(data);
            allOutput += output;
            // 看到 passkey 提示，回复 yes
            if (!replied && output.contains("[agent] Confirm passkey")) {
                proc.write("yes\n");
                proc.waitForBytesWritten(1000);
                replied = true;
            }
            // 配对成功
            if (output.contains("Paired: yes") || output.contains("Pairing successful")) {
                paired = true;
            }
            // 配对失败
            if (output.contains("Failed") || output.contains("not available")) {
                break;
            }
        }
    }

    proc.write("quit\n");
    proc.waitForFinished(5000);
    // 读取剩余输出
    allOutput += QString::fromUtf8(proc.readAllStandardOutput());
    allOutput += QString::fromUtf8(proc.readAllStandardError());

    if (allOutput.contains("Paired: yes") || allOutput.contains("Pairing successful")) {
        return "OK";
    }
    return "FAIL:" + allOutput.trimmed();
}

QString E_button::bluetoothConnect(const QString &addr)
{
    QProcess proc;
    proc.start("bluetoothctl", QStringList() << "connect" << addr);
    proc.waitForFinished(15000);
    QString output = QString::fromUtf8(proc.readAllStandardOutput())
                     + QString::fromUtf8(proc.readAllStandardError());
    if (output.contains("Connection successful") || output.contains("Connected: yes")) {
        return "OK";
    }
    return output.trimmed();
}

QString E_button::bluetoothIsPaired(const QString &addr)
{
    QProcess proc;
    proc.start("bluetoothctl", QStringList() << "paired-devices");
    proc.waitForFinished(3000);
    QString output = QString::fromUtf8(proc.readAllStandardOutput());
    // 已配对设备列表格式: Device AA:BB:CC:DD:EE:FF Name
    if (output.contains(addr)) {
        return "OK";
    }
    return "";
}

QString E_button::bluetoothDisconnect(const QString &addr)
{
    QProcess proc;
    proc.start("bluetoothctl", QStringList() << "disconnect" << addr);
    proc.waitForFinished(5000);
    return "OK";
}

// ========== HTTP 文件服务器 ==========

QString E_button::getWlanIP()
{
    // 用 shell 命令获取 wlan0 IP，比 QNetworkInterface 更可靠
    QProcess proc;
    proc.start("sh", QStringList() << "-c"
        << "ip addr show wlan0 2>/dev/null | grep 'inet ' | awk '{print $2}' | cut -d/ -f1");
    proc.waitForFinished(3000);
    return QString::fromUtf8(proc.readAllStandardOutput()).trimmed();
}

QString E_button::startFileServer()
{
    if (m_fileServer) {
        return getWlanIP();
    }

    m_fileServer = new QTcpServer(this);
    connect(m_fileServer, &QTcpServer::newConnection,
            this, &E_button::onFileServerNewConnection);

    if (!m_fileServer->listen(QHostAddress::Any, 8080)) {
        delete m_fileServer;
        m_fileServer = nullptr;
        return "";
    }
    return getWlanIP();
}

void E_button::stopFileServer()
{
    if (m_fileServer) {
        m_fileServer->close();
        delete m_fileServer;
        m_fileServer = nullptr;
    }
}

// 根据扩展名返回 MIME 类型
static QString mimeType(const QString &path)
{
    if (path.endsWith(".jpg") || path.endsWith(".jpeg"))
        return "image/jpeg";
    if (path.endsWith(".png"))  return "image/png";
    if (path.endsWith(".bmp"))  return "image/bmp";
    if (path.endsWith(".h264")) return "video/h264";
    if (path.endsWith(".mp4"))  return "video/mp4";
    if (path.endsWith(".wav"))  return "audio/wav";
    return "application/octet-stream";
}

void E_button::onFileServerNewConnection()
{
    QTcpSocket *socket = m_fileServer->nextPendingConnection();
    if (!socket) return;

    connect(socket, &QTcpSocket::disconnected, socket, &QObject::deleteLater);

    // 等待数据到达
    socket->waitForReadyRead(3000);
    QByteArray request = socket->readAll();
    QString req = QString::fromUtf8(request);

    // 解析 GET 请求行: GET /path HTTP/1.1
    QStringList lines = req.split("\r\n");
    if (lines.isEmpty()) { socket->close(); return; }
    QStringList parts = lines[0].split(' ');
    if (parts.size() < 2) { socket->close(); return; }
    QString rawPath = parts[1];

    // URL 解码
    QString path = QUrl::fromPercentEncoding(rawPath.toUtf8());

    if (path == "/") {
        // ===== 首页：列出文件 =====
        QDir dir(m_mediaPath);
        QStringList filters;
        filters << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp" << "*.h264" << "*.mp4" << "*.wav";
        QFileInfoList files = dir.entryInfoList(filters, QDir::Files, QDir::Time);

        QString html;
        html += "<!DOCTYPE html><html><head><meta charset='utf-8'>";
        html += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
        html += "<title>文件下载</title><style>";
        html += "body{font-family:sans-serif;max-width:600px;margin:0 auto;padding:20px;background:#f5f5f5;}";
        html += "h1{color:#333;font-size:24px;}";
        html += "h2{color:#555;font-size:18px;margin:20px 0 8px;}";
        html += ".item{background:#fff;margin:8px 0;padding:12px 16px;border-radius:8px;display:flex;align-items:center;justify-content:space-between;box-shadow:0 1px 3px rgba(0,0,0,0.1);}";
        html += ".name{flex:1;overflow:hidden;text-overflow:ellipsis;white-space:nowrap;font-size:16px;color:#333;}";
        html += ".size{color:#999;font-size:13px;margin:0 12px;}";
        html += ".icon{font-size:20px;margin-right:10px;}";
        html += "a{background:#2196F3;color:#fff;text-decoration:none;padding:6px 16px;border-radius:6px;font-size:14px;white-space:nowrap;}";
        html += "</style></head><body>";
        html += "<h1>文件列表</h1>";

        auto fmtSize = [](qint64 bytes) -> QString {
            if (bytes < 1024) return QString::number(bytes) + " B";
            if (bytes < 1024*1024) return QString::number(bytes/1024.0, 'f', 1) + " KB";
            return QString::number(bytes/1048576.0, 'f', 1) + " MB";
        };

        // 分组：图片 / 视频
        QFileInfoList imageFiles, videoFiles;
        for (const QFileInfo &f : files) {
            QString ext = f.suffix().toLower();
            if (ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "bmp" || ext == "gif") {
                imageFiles.append(f);
            } else if (ext == "h264" || ext == "mp4") {
                videoFiles.append(f);
            }
        }

        auto renderItem = [&](const QFileInfo &f) -> QString {
            QString icon = f.suffix().toLower() == "h264" || f.suffix().toLower() == "mp4" ? "🎬" : "📷";
            QString encName = QString::fromUtf8(QUrl::toPercentEncoding(f.fileName()));
            return "<div class='item'>"
                   "<span class='icon'>" + icon + "</span>"
                   "<span class='name' title='" + f.fileName().toHtmlEscaped() + "'>" + f.fileName().toHtmlEscaped() + "</span>"
                   "<span class='size'>" + fmtSize(f.size()) + "</span>"
                   "<a href='/" + encName + "'>下载</a>"
                   "</div>";
        };

        html += "<h2>图片</h2>";
        for (const QFileInfo &f : imageFiles) {
            html += renderItem(f);
        }

        html += "<h2>视频</h2>";
        for (const QFileInfo &f : videoFiles) {
            html += renderItem(f);
        }

        html += "</body></html>";

        QByteArray resp = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: text/html; charset=utf-8\r\n"
                          "Content-Length: " + QByteArray::number(html.toUtf8().size()) + "\r\n"
                          "Connection: close\r\n\r\n" + html.toUtf8();
        socket->write(resp);
    } else {
        // ===== 下载文件 =====
        // 去掉开头的 /
        QString fileName = path.mid(1);
        QString filePath = m_mediaPath + "/" + fileName;

        QFile file(filePath);
        if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
            QByteArray body = "<h1>404 Not Found</h1>";
            QByteArray resp = "HTTP/1.1 404 Not Found\r\nContent-Length: "
                              + QByteArray::number(body.size()) + "\r\nConnection: close\r\n\r\n" + body;
            socket->write(resp);
        } else {
            QByteArray data = file.readAll();
            file.close();
            QByteArray header = "HTTP/1.1 200 OK\r\n"
                                "Content-Type: " + mimeType(filePath).toUtf8() + "\r\n"
                                "Content-Length: " + QByteArray::number(data.size()) + "\r\n"
                                "Content-Disposition: attachment; filename=\"" + fileName.toUtf8() + "\"\r\n"
                                "Connection: close\r\n\r\n";
            socket->write(header + data);
        }
    }
    socket->waitForBytesWritten(3000);
    socket->close();
}


int i = 0;
// void E_button::setmsqh(uint32_t color){
//     if(color==1){
//         this->cam->swtich_photons_color(0xFF0000FF);//001dahong
//         //this->uv->m_color = 0x06;
//     }
//     else if(color==6){
//         this->cam->swtich_photons_color(0xFFFF00FF);//101zihong
//         //this->uv->m_color = 0x05;
//     }
//     else if(color==2){
//         this->cam->swtich_photons_color(0xFFFFFFFF);//111bai
//         //this->uv->m_color = 0x01;
//     }
//     else if(color==3){
//         this->cam->swtich_photons_color(0xFFFF0000);//100lan
//         //this->uv->m_color = 0x07;
//     }
//     else if(color==5){
//         this->cam->swtich_photons_color(0xFFFFFF00);//110qing
//         //this->uv->m_color = 0x03;
//     }
//     else if(color==12){
//         this->cam->swtich_photons_color(0xFF00FF00);//010lv
//         //this->uv->m_color = 0x04;
//     }
//     else if(color==4){
//         this->cam->swtich_photons_color(0xFF00FFFF);//011huang
//         //this->uv->m_color = 0x02;
//     }
//     else if(color==7){
//         this->cam->swtich_photons_color(0xFF8800FF);//meihong
//         //this->uv->m_color = 0x0c;
//     }
//     else if(color==8){
//         this->cam->swtich_photons_color(0xFF880088);//zise
//         //this->uv->m_color = 0x0a;
//     }
//     else if(color==9){
//         this->cam->swtich_photons_color(0xFF888888);//huise
//         //this->uv->m_color = 0x0b;
//     }
//     else if(color==10){
//         this->cam->swtich_photons_color(0xFF004400);//shenlv
//         //this->uv->m_color = 0x08;
//     }
//     else if(color==11){
//         this->cam->swtich_photons_color(0xFFFF8844);//qianlan
//         //this->uv->m_color = 0x09;
//     }
// }


void E_button::setmsqh(uint32_t color){
    if(color==1){
        this->uv->m_color = 0x06;
    }
    else if(color==6){
        this->uv->m_color = 0x05;
    }
    else if(color==2){
        this->uv->m_color = 0x01;
    }
    else if(color==3){
        this->uv->m_color = 0x07;
    }
    else if(color==5){
        this->uv->m_color = 0x03;
    }
    else if(color==12){
        this->uv->m_color = 0x04;
    }
    else if(color==4){
        this->uv->m_color = 0x02;
    }
    else if(color==7){
        this->uv->m_color = 0x0c;
    }
    else if(color==8){
        this->uv->m_color = 0x0a;
    }
    else if(color==9){
        this->uv->m_color = 0x0b;
    }
    else if(color==10){
        this->uv->m_color = 0x08;
    }
    else if(color==11){
        this->uv->m_color = 0x09;
    }
}

void E_button::bgra8888() {
    QObject *mingzi = this->rootObject->findChild<QObject*>("mingzi");
    QObject *shijian = this->rootObject->findChild<QObject*>("shijian1");

    QString text = mingzi->property("text").toString();
    QString originalName = text;
    QString tupianshijian = shijian->property("text").toString();
        qDebug() << tupianshijian.mid(3);
    tupianshijian.replace('/', '-');
    tupianshijian.replace(' ', '_');
    tupianshijian.replace(':', '-');

    //qDebug() << "???????++341"+QString::number(this->nnnn)+"-"+QString::number(this->yy)+"-"+QString::number(this->dd)+"-"+QString::number(this->ss)+"-"+QString::number(this->ff)+"-"+QString::number(this->miao)+".jpg";
    QString filePath;
    if(text == ""){
        this->sizejpg++;
        if(0<sizejpg&& sizejpg<10)
            filePath = "/mnt/sdcard/Standard Storage/J000"+ QString::number(this->sizejpg) +".jpg";
        if(10<=sizejpg&& sizejpg<100)
            filePath = "/mnt/sdcard/Standard Storage/J00"+ QString::number(this->sizejpg) +".jpg";
        if(100<=sizejpg&& sizejpg<1000)
            filePath = "/mnt/sdcard/Standard Storage/J0"+ QString::number(this->sizejpg) +".jpg";
        if(1000<=sizejpg)
            filePath = "/mnt/sdcard/Standard Storage/J"+ QString::number(this->sizejpg) +".jpg";
        // filePath = "/mnt/sdcard/Standard Storage/"+tupianshijian.mid(3)+".jpg";
        //filePath = "/mnt/sdcard/shijian/"+QString::number(this->nnnn)+"-"+QString::number(this->yy)+"-"+QString::number(this->dd)+"-"+QString::number(this->ss)+"-"+QString::number(this->ff)+"-"+QString::number(this->miao)+".jpg";
    }else{
        filePath = "/mnt/sdcard/Standard Storage/" + text + ".jpg";
    }


    if (this->cam == NULL)
    {
        qDebug() << "camera_blend is NULL, cannot take photo";
        return;
    }

    if (this->cam->acquire_new_frame_data()) {
        // 等待最多 2000ms 获取拍照帧，每 50ms 检查一次
        void* data = NULL;
        for (int retry = 0; retry < 40; retry++)
        {
            QThread::msleep(50);
            data = this->cam->try_to_get_new_frame_data();
            if (data) break;
        }
        if (data) {
            uint8_t* newdata = static_cast<uint8_t*>(data);

            QImage bgraImage(newdata, 1920, 1080, QImage::Format_RGBA8888);
            this->image = bgraImage.convertToFormat(QImage::Format_RGBA8888);

            // 拍照帧已获取，此时让QML事件循环跑一次，把最新测量数据推送到 m_irMeasurementDataJson
            QCoreApplication::processEvents();

            // 创建一个 QPainter 对象，用于在图像上绘制
            QPainter painter(&this->image);
            painter.setPen(Qt::white); // 设置文本颜色

            painter.setFont(QFont("Arial", 50)); // 设置字体和大小
            //QString text = "MetaUVI";
            QString text = " ";
            text = this->rootObject->findChild<QObject*>("gsname")->property("text").toString();
            painter.drawText(10,75, text); // 在指定位置绘制文本
            painter.setFont(QFont("Arial", 30));


            // 增益、距离、计数仅在紫外和融合模式下显示，可见光和红外模式不显示
            if (this->m_mssz != 0 && this->m_mssz != 3) {
                text = this->rootObject->findChild<QObject*>("zengyi")->property("text").toString();
                painter.drawText(150,920, text);
                text = this->rootObject->findChild<QObject*>("juli")->property("text").toString();
                painter.drawText(650,920, text);
                text = this->rootObject->findChild<QObject*>("jishu")->property("text").toString();
                painter.drawText(1300,920, text);
            }

            text = this->rootObject->findChild<QObject*>("moshi")->property("text").toString();
            painter.drawText(150,1000, text);
            text = this->rootObject->findChild<QObject*>("wenshidu")->property("text").toString();
            painter.drawText(650,1000, text);
            // if(this->shijiankaiguan == 1){
            text = this->rootObject->findChild<QObject*>("shijian1")->property("text").toString();
            painter.drawText(1300,1000, text);/*}*/

            QObject* gpsObj = this->rootObject->findChild<QObject*>("gps");
            bool gpsSwitchOn = gpsObj && gpsObj->property("visible").toBool();
            if (this->gps && gpsSwitchOn) {
                sprintf(gps_jw, "N:%.2f E:%.2f",
                        this->gps->mGPS_info.longitude,
                        this->gps->mGPS_info.latitude);
                text = QString::fromUtf8(gps_jw);
                painter.drawText(1300, 75, text);
            }

            // ==== 红外模式下绘制测量工具叠加层 ====
            if (this->m_mssz == 3) {
                // processEvents() 已在上方调用，QML的updateTimer已将最新JSON写入 m_irMeasurementDataJson
                QString jsonStr = m_irMeasurementDataJson;
                QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
                if (doc.isObject()) {
                    QJsonObject data = doc.object();

                    // --- 绘制矩形框 ---
                    if (data["showRect"].toBool()) {
                        QJsonArray rects = data["rects"].toArray();
                        for (int i = 0; i < rects.size(); i++) {
                            QJsonObject r = rects[i].toObject();
                            int rx = (int)r["x"].toDouble();
                            int ry = (int)r["y"].toDouble();
                            int rw = (int)r["w"].toDouble();
                            int rh = (int)r["h"].toDouble();
                            float maxTemp = r["maxTemp"].toDouble();
                            int hotX = (int)r["hotX"].toDouble();
                            int hotY = (int)r["hotY"].toDouble();

                            // 绿色矩形框
                            QPen rectPen(QColor("#00FF00"), 4);
                            painter.setPen(rectPen);
                            painter.setBrush(Qt::NoBrush);
                            painter.drawRect(rx, ry, rw, rh);

                            // 最高温文本
                            painter.setPen(QPen(Qt::black, 2));
                            painter.setFont(QFont("Arial", 32, QFont::Bold));
                            painter.drawText(rx + 6, ry + 32, QString("Max: %1°C").arg(maxTemp, 0, 'f', 1));
                            painter.setPen(Qt::white);
                            painter.drawText(rx + 4, ry + 30, QString("Max: %1°C").arg(maxTemp, 0, 'f', 1));

                            // 最高温三角形
                            if (hotX > 0 && hotY > 0) {
                                int size = 20;
                                QPen triPen(Qt::red);
                                painter.setPen(triPen);
                                painter.setBrush(QColor(Qt::red));
                                QPointF tri[3] = {
                                    QPointF(hotX, hotY - size),
                                    QPointF(hotX - size * 0.6, hotY + size * 0.5),
                                    QPointF(hotX + size * 0.6, hotY + size * 0.5)
                                };
                                painter.drawConvexPolygon(tri, 3);
                            }
                        }
                    }

                    // --- 绘制直线 ---
                    if (data["showLine"].toBool()) {
                        int lx1 = (int)data["lineX1"].toDouble();
                        int ly1 = (int)data["lineY1"].toDouble();
                        int lx2 = (int)data["lineX2"].toDouble();
                        int ly2 = (int)data["lineY2"].toDouble();
                        int lHotX = (int)data["lineHotX"].toDouble();
                        int lHotY = (int)data["lineHotY"].toDouble();
                        float lMaxTemp = data["lineMaxTemp"].toDouble();

                        // 画直线
                        QPen linePen(QColor("#00FF00"), 4);
                        painter.setPen(linePen);
                        painter.drawLine(lx1, ly1, lx2, ly2);

                        // 端点圆
                        painter.setBrush(QColor("#4CAF50"));
                        painter.drawEllipse(QPointF(lx1, ly1), 5, 5);
                        painter.drawEllipse(QPointF(lx2, ly2), 5, 5);

                        // 中点橙色圆
                        int midX = (lx1 + lx2) / 2;
                        int midY = (ly1 + ly2) / 2;
                        painter.setBrush(QColor("#FF9800"));
                        painter.drawEllipse(QPointF(midX, midY), 5, 5);

                        // 最高温三角形
                        if (lHotX > 0 && lHotY > 0) {
                            int size = 20;
                            painter.setPen(QPen(Qt::red, 2));
                            painter.setBrush(QColor(Qt::red));
                            QPointF tri[3] = {
                                QPointF(lHotX, lHotY),
                                QPointF(lHotX - size * 0.6, lHotY + size),
                                QPointF(lHotX + size * 0.6, lHotY + size)
                            };
                            painter.drawConvexPolygon(tri, 3);

                            // 温度文本
                            painter.setPen(QPen(Qt::black, 3));
                            painter.setFont(QFont("Arial", 32, QFont::Bold));
                            QRectF textRect(lHotX - 50, lHotY + size + 8, 100, 40);
                            painter.drawText(textRect, Qt::AlignCenter, QString("%1°C").arg(lMaxTemp, 0, 'f', 1));
                            painter.setPen(Qt::white);
                            painter.drawText(textRect, Qt::AlignCenter, QString("%1°C").arg(lMaxTemp, 0, 'f', 1));
                        }
                    }

                    // --- 绘制点 ---
                    if (data["showPoint"].toBool()) {
                        QJsonArray points = data["points"].toArray();
                        for (int i = 0; i < points.size(); i++) {
                            QJsonObject p = points[i].toObject();
                            int px = (int)p["x"].toDouble();
                            int py = (int)p["y"].toDouble();
                            float ptTemp = p["temp"].toDouble();

                            // 红色圆
                            painter.setPen(QPen(Qt::white, 2));
                            painter.setBrush(QColor("#FF5722"));
                            painter.drawEllipse(QPointF(px, py), 14, 14);

                            // 温度文本
                            painter.setPen(QPen(Qt::black, 2));
                            painter.setFont(QFont("Arial", 24, QFont::Bold));
                            QRectF ptTextRect(px - 40, py + 22, 80, 30);
                            painter.drawText(ptTextRect, Qt::AlignCenter, QString("%1°C").arg(ptTemp, 0, 'f', 1));
                            painter.setPen(Qt::white);
                            painter.drawText(ptTextRect, Qt::AlignCenter, QString("%1°C").arg(ptTemp, 0, 'f', 1));
                        }
                    }

                    // --- 绘制全局最高温点三角形 ---
                    int gHotX = (int)data["globalHotX"].toDouble();
                    int gHotY = (int)data["globalHotY"].toDouble();
                    if (gHotX > 0 && gHotY > 0) {
                        int size = 20;
                        int margin = size + 5;
                        int drawX = qMax(285 + margin, qMin(1920 - 287 - margin, gHotX));
                        int drawY = qMax(margin, qMin(1080 - margin, gHotY));

                        painter.setPen(QPen(Qt::red, 2));
                        painter.setBrush(QColor(Qt::red));
                        QPointF tri[3] = {
                            QPointF(drawX, drawY - size),
                            QPointF(drawX - size * 0.6, drawY + size * 0.5),
                            QPointF(drawX + size * 0.6, drawY + size * 0.5)
                        };
                        painter.drawConvexPolygon(tri, 3);
                    }
                }
            }

            // ==== IR_UV融合 绿框 ====
            if (this->m_mssz == 4 && this->cam != NULL) {
                QPen uvDebugPen(QColor("#00FF00"), 3);
                painter.setPen(uvDebugPen);
                painter.setBrush(Qt::NoBrush);
                painter.drawRect(
                    this->cam->getIruvDstClippedX(),
                    this->cam->getIruvDstClippedY(),
                    this->cam->getIruvDstClippedWidth(),
                    this->cam->getIruvDstClippedHeight()
                    );
            }

            painter.end();
            // 使用QFile保存以确保数据刷写到SD卡
            {
                QFile imgFile(filePath);
                if (imgFile.open(QIODevice::WriteOnly)) {
                    QByteArray imgData;
                    QBuffer imgBuffer(&imgData);
                    imgBuffer.open(QIODevice::WriteOnly);
                    this->image.save(&imgBuffer, "JPG", 95);
                    imgBuffer.close();
                    imgFile.write(imgData);
                    imgFile.flush();
                    fsync(imgFile.handle());
                    imgFile.close();
                    qDebug() << "Image saved and synced to SD card:" << filePath;
                } else {
                    qWarning() << "Failed to save image to" << filePath;
                }
            }

            if (this->m_mssz == 3) {
                unsigned char *ir_yuv_data = nullptr;
                int ir_yuv_size = 0;
                unsigned char *ir_raw_data = nullptr;
                int ir_raw_size = 0;
                int ir_width = 0;
                int ir_height = 0;

                if (this->cam->get_ir_frame_data(&ir_yuv_data, &ir_yuv_size, &ir_raw_data, &ir_raw_size, &ir_width, &ir_height)) {
                    QString irDirPath = "/mnt/sdcard/Standard Storage/IR";
                    QDir irDir(irDirPath);
                    if (!irDir.exists()) {
                        irDir.mkpath(irDirPath);
                        qDebug() << "Created IR directory:" << irDirPath;
                    }

                    QString irFilePath;
                    if(originalName == ""){
                        if(0<sizejpg&& sizejpg<10)
                            irFilePath = irDirPath + "/IR000"+ QString::number(this->sizejpg) +".jpg";
                        if(10<=sizejpg&& sizejpg<100)
                            irFilePath = irDirPath + "/IR00"+ QString::number(this->sizejpg) +".jpg";
                        if(100<=sizejpg&& sizejpg<1000)
                            irFilePath = irDirPath + "/IR0"+ QString::number(this->sizejpg) +".jpg";
                        if(1000<=sizejpg)
                            irFilePath = irDirPath + "/IR"+ QString::number(this->sizejpg) +".jpg";
                    } else {
                        QString ledgerIrDirPath = "/mnt/sdcard/Ledger storage/IR";
                        QDir ledgerIrDir(ledgerIrDirPath);
                        if (!ledgerIrDir.exists()) {
                            ledgerIrDir.mkpath(ledgerIrDirPath);
                            qDebug() << "Created Ledger IR directory:" << ledgerIrDirPath;
                        }
                        irFilePath = ledgerIrDirPath + "/IR_" + originalName + ".jpg";
                    }

                    QImage irImage(ir_width, ir_height, QImage::Format_RGB888);
                    for (int y = 0; y < ir_height; y++) {
                        for (int x = 0; x < ir_width; x += 2) {
                            int idx = (y * ir_width + x) * 2;
                            int u = ir_yuv_data[idx];
                            int y0 = ir_yuv_data[idx + 1];
                            int v = ir_yuv_data[idx + 2];
                            int y1 = ir_yuv_data[idx + 3];

                            int r0, g0, b0, r1, g1, b1;
                            r0 = y0 + 1.402 * (v - 128);
                            g0 = y0 - 0.344 * (u - 128) - 0.714 * (v - 128);
                            b0 = y0 + 1.772 * (u - 128);
                            if (r0 < 0) r0 = 0; if (r0 > 255) r0 = 255;
                            if (g0 < 0) g0 = 0; if (g0 > 255) g0 = 255;
                            if (b0 < 0) b0 = 0; if (b0 > 255) b0 = 255;

                            r1 = y1 + 1.402 * (v - 128);
                            g1 = y1 - 0.344 * (u - 128) - 0.714 * (v - 128);
                            b1 = y1 + 1.772 * (u - 128);
                            if (r1 < 0) r1 = 0; if (r1 > 255) r1 = 255;
                            if (g1 < 0) g1 = 0; if (g1 > 255) g1 = 255;
                            if (b1 < 0) b1 = 0; if (b1 > 255) b1 = 255;

                            irImage.setPixel(x, y, qRgb(r0, g0, b0));
                            if (x + 1 < ir_width) {
                                irImage.setPixel(x + 1, y, qRgb(r1, g1, b1));
                            }
                        }
                    }

                    QFile irFile(irFilePath);
                    if (irFile.open(QIODevice::WriteOnly)) {
                        QByteArray jpgData;
                        QBuffer buffer(&jpgData);
                        buffer.open(QIODevice::WriteOnly);
                        irImage.save(&buffer, "JPG");
                        buffer.close();

                        USBSDK_GlobalHeatmapInfo heatMapInfo;
                        memset(&heatMapInfo, 0, sizeof(heatMapInfo));
                        heatMapInfo.emtype = RESOLUTION_640;
                        heatMapInfo.unWidth = ir_width;
                        heatMapInfo.unHeight = ir_height;
                        heatMapInfo.unSizeMap = ir_width * ir_height * 2;

                        float *tempData = new float[ir_width * ir_height];
                        if (USBSDK_GetGlobalHeatMapTemperature(&heatMapInfo, ir_raw_data, tempData)) {
                            short *shortTempData = new short[ir_width * ir_height];
                            for (int i = 0; i < ir_width * ir_height; i++) {
                                shortTempData[i] = static_cast<short>(tempData[i] * 100);
                            }
                            QByteArray tempDataArray(reinterpret_cast<const char*>(shortTempData), ir_width * ir_height * sizeof(short));
                            jpgData.append(tempDataArray);
                            delete[] shortTempData;
                        }
                        delete[] tempData;

                        USBSDK_RadCommonTemparam temperParam;
                        memset(&temperParam, 0, sizeof(temperParam));
                        if (USBSDK_GetCommonTemperParam(&temperParam)) {
                            QByteArray paramData(reinterpret_cast<const char*>(&temperParam), sizeof(USBSDK_RadCommonTemparam));
                            jpgData.append(paramData);
                        }
                        irFile.write(jpgData);
                        irFile.flush();
                        fsync(irFile.handle());
                        irFile.close();
                    } 
                }
            }
            
        } 
    } 

    
}

bool E_button::startluzhi(){
    if (this->cam == NULL)
    {
        qDebug() << "camera_blend is NULL, cannot start recording";
        return false;
    }
    //this->biaozhiwei_moshi = 1;
    bzero(this->buf,128);
    this->sizemp4++;
    if(0<sizemp4 && sizemp4<10)
        snprintf(this->buf, sizeof(this->buf), "/mnt/sdcard/Standard Storage/M000%d", this->sizemp4);
    if(10<=sizemp4 && sizemp4<100)
        snprintf(this->buf, sizeof(this->buf), "/mnt/sdcard/Standard Storage/M00%d", this->sizemp4);//0010
    if(100<=sizemp4&& sizemp4<1000)
        snprintf(this->buf, sizeof(this->buf), "/mnt/sdcard/Standard Storage/M0%d", this->sizemp4);//0100
    if(1000<=sizemp4)
        snprintf(this->buf, sizeof(this->buf), "/mnt/sdcard/Standard Storage/M%d", this->sizemp4);//0100

    bool ret = this->cam->start_record(this->buf);
    if (!ret)
    {
        qDebug() << "start_record failed for:" << this->buf;
        return ret;
    }
    qDebug()<<this->buf;

    // 同步启动音频录制
    const char *lastSlash = strrchr(this->buf, '/');
    const char *baseName = lastSlash ? lastSlash + 1 : this->buf;
    char audioPath[256];
    char mkdirCmd[256];
    snprintf(mkdirCmd, sizeof(mkdirCmd), "mkdir -p /opt/voice");
    system(mkdirCmd);
    snprintf(audioPath, sizeof(audioPath), "/opt/voice/%s.wav", baseName);
    char arecordCmd[512];
    snprintf(arecordCmd, sizeof(arecordCmd),
             "arecord -Dhw:0,0 -f cd -r 44100 -c 2 %s &", audioPath);
    system(arecordCmd);
    qDebug() << "Audio recording started:" << audioPath;

    return ret;
}
bool E_button::overluzhi(){
    if (this->cam == NULL)
    {
        qDebug() << "camera_blend is NULL, cannot stop recording";
        return false;
    }

    // 停止音频录制（SIGINT 让 arecord 正常关闭 wav 文件头）
    system("pkill -INT -f 'arecord.*voice'");
    usleep(500000); // 等 0.5s 确保文件写完
    qDebug() << "Audio recording stopped";

    // 降噪处理 + 滤波降12dB
    const char *lastSlash = strrchr(this->buf, '/');
    const char *baseName = lastSlash ? lastSlash + 1 : this->buf;
    char audioPath[256], cleanAudioPath[256], soxCmd[512];
    snprintf(audioPath, sizeof(audioPath), "/opt/voice/%s.wav", baseName);
    snprintf(cleanAudioPath, sizeof(cleanAudioPath), "/opt/voice/%s_clean.wav", baseName);
    snprintf(soxCmd, sizeof(soxCmd),
             "sox %s %s gain -12 highpass 100 lowpass 8000",
             audioPath, cleanAudioPath);
    qDebug() << "Running sox:" << soxCmd;
    int soxRet = system(soxCmd);
    if (soxRet != 0) {
        qDebug() << "sox failed, fallback: copy raw wav";
        // sox 失败时直接复制原始 wav 作为兜底
        char cpCmd[512];
        snprintf(cpCmd, sizeof(cpCmd), "cp %s %s", audioPath, cleanAudioPath);
        system(cpCmd);
    }
    qDebug() << "Audio processing done:" << cleanAudioPath;

    //this->biaozhiwei_moshi = 0;
    this->cam->stop_record();
    qDebug()<<this->buf;
    return true;
}

QString E_button::getVideoThumbnail(const QString &videoPath)
{
    qDebug() << "getVideoThumbnail called with path:" << videoPath;

    // 检查视频文件是否存在
    if (!QFile::exists(videoPath)) {
        qDebug() << "Video file does not exist:" << videoPath;
        return "file:///opt/bofang.jpg";
    }

    // 生成缩略图文件名
    QString thumbnailPath = "/tmp/thumb_" +
                            QString::number(QDateTime::currentMSecsSinceEpoch()) + ".jpg";

    qDebug() << "Thumbnail will be saved to:" << thumbnailPath;

    // 构建 ffmpeg 命令参数
    QStringList args;
    args << "-i" << videoPath
         << "-ss" << "00:00:01"     // 跳到第1秒
         << "-vframes" << "1"        // 只取1帧
         << "-s" << "480x270"        // 缩略图大小
         << thumbnailPath
         << "-y";                    // 覆盖已存在文件

    // 创建 QProcess 对象
    QProcess process;
    process.start("ffmpeg", args);

    qDebug() << "Running ffmpeg with args:" << args;

    // 等待完成，超时3秒
    if (!process.waitForFinished(3000)) {
        qDebug() << "ffmpeg timeout or error:" << process.errorString();
        qDebug() << "ffmpeg stderr:" << process.readAllStandardError();
        return "file:///opt/bofang.jpg";
    }

    // 检查进程退出状态
    if (process.exitCode() != 0) {
        qDebug() << "ffmpeg exited with code:" << process.exitCode();
        qDebug() << "ffmpeg stderr:" << process.readAllStandardError();
    }

    // 检查缩略图是否生成成功
    if (QFile::exists(thumbnailPath)) {
        QFileInfo fileInfo(thumbnailPath);
        if (fileInfo.size() > 0) {
            qDebug() << "Thumbnail created successfully:" << thumbnailPath
                     << "size:" << fileInfo.size() << "bytes";
            return "file://" + thumbnailPath;
        } else {
            qDebug() << "Thumbnail file is empty";
            QFile::remove(thumbnailPath);
        }
    } else {
        qDebug() << "Thumbnail file was not created";
    }

    // 失败时返回默认图标
    return "file:///opt/bofang.jpg";
}

// ========== 蜂鸣器报警音 ==========
void E_button::playAlarm()
{
    // 如果已经在播放，不重复启动
    if (m_alarmProcess && m_alarmProcess->state() == QProcess::Running) {
        return;
    }

    if (!m_alarmProcess) {
        m_alarmProcess = new QProcess(this);
    }

    QStringList args;
    args << "-D" << "plughw:0,0" << "/opt/buzzer.wav";
    m_alarmProcess->start("aplay", args);
    qDebug() << "Alarm sound started";
}

void E_button::stopAlarm()
{
    if (m_alarmProcess && m_alarmProcess->state() == QProcess::Running) {
        m_alarmProcess->kill();
        m_alarmProcess->waitForFinished(500);
        qDebug() << "Alarm sound stopped";
    }
}
//gps重连
bool E_button::gps_reinit(){
    return this->gps->GPS_reinit();
}
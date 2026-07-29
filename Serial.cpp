
#include "Serial.h"
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
typedef long long timestamp_t;

class Timestamp
{

public:
    // 获取以毫秒计的时间戳
    static timestamp_t now()
    {
        struct timeval tv;
        gettimeofday(&tv, 0);
        timestamp_t time = (timestamp_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
        return time;
    }
};

Serial::Serial() : rxStream()
{
    pthread_mutex_init(&rxMutex, 0);
    pthread_mutex_init(&txMutex, 0);
}

Serial::~Serial()
{
    close();
}

void *receiveThread(void *arg)
{
    Serial *serial = (Serial *)arg;
    char buf[64];
    int len;
    for (;;)
    {
        pthread_testcancel();
        len = read(serial->fd, buf, sizeof(buf));
        if (len > 0)
        {
            pthread_mutex_lock(&serial->rxMutex);
            serial->rxStream.append(buf, len);
            pthread_mutex_unlock(&serial->rxMutex);
        }
        else
            usleep(33 * 1000);
    }
    return NULL;
}
void *transportThread(void *arg)
{
    Serial *serial = (Serial *)arg;
    char buf[64];
    timeval tv;

    int len;
    for (;;)
    {
        pthread_mutex_lock(&serial->txMutex);
        len = serial->txStream.take(buf, sizeof(buf));
        pthread_mutex_unlock(&serial->txMutex);
        if (len > 0)
            write(serial->fd, buf, len);
        usleep(50000);
    }
    return NULL;
}

int Serial::open(const char *dev, int baud, int dataBits, int parityMode, int stopBits)
{
    struct termios options;
    bzero(&options, sizeof(options));
    int baudT = transformBaud(baud);
    if (baudT < 0)
        return BAUD_NOT_SUPPORTED;
    cfsetispeed(&options, baudT);
    cfsetospeed(&options, baudT);
    pthread_mutex_init(&rxMutex, 0);
    pthread_mutex_init(&txMutex, 0);

    int dataBitsT = transformDataBits(dataBits);
    if (dataBitsT < 0)
        return DATABITS_NOT_SUPPORTED;
    options.c_cflag |= dataBitsT;
    if (parityMode == PARITY_ODD)
    {
        options.c_cflag |= PARENB;
        options.c_cflag |= PARODD;
    }
    else if (parityMode == PARITY_EVEN)
        options.c_cflag |= PARENB;
    else if (parityMode != PARITY_NONE)
        return PARITYMODE_NOT_SUPPORTED;
    if (stopBits == 2)
        options.c_cflag |= CSTOPB;
    else if (stopBits != 1)
        return STOPBITS_NOT_SUPPORTED;
    options.c_cflag |= CREAD | CLOCAL;
    options.c_cc[VTIME] = 1;
    options.c_cc[VMIN] = 1;
    fd = ::open(dev, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0)
        return DEV_NOT_FOUND;
    if (tcsetattr(fd, TCSANOW, &options))
        return CONFIG_FAIL;
    if (tcflush(fd, TCIOFLUSH))
        return CONFIG_FAIL;
    if (pthread_create(&rxTid, 0, receiveThread, this) != 0)
        return NEW_THREAD_FAIL;
    if (pthread_create(&txTid, 0, transportThread, this) != 0)
        return NEW_THREAD_FAIL;
    return OK;
}

void Serial::close()
{
    if (fd >= 0)
    {
        ::close(fd);
        fd = -1;
        pthread_cancel(rxTid);
        pthread_cancel(txTid);
        pthread_join(rxTid, NULL);
        pthread_join(txTid, NULL);
        pthread_mutex_destroy(&rxMutex);
        pthread_mutex_destroy(&txMutex);
    }
}

void Serial::write(const char *data, int len)
{

    pthread_mutex_lock(&txMutex);
    txStream.append(data, len);
    pthread_mutex_unlock(&txMutex);
}
int Serial::available()
{
    return rxStream.getLength();
}

int Serial::peek(char *buf, int len)
{
    len = rxStream.peek(buf, len);
    return len;
}

int Serial::read(char *buf, int len, int timeout)
{
    timestamp_t start = Timestamp::now();
    int total = 0;
    while (total < len)
    {
        pthread_mutex_lock(&rxMutex);
        int readLen = rxStream.take(buf + total, len - total);
        pthread_mutex_unlock(&rxMutex);
        if (readLen > 0)
            total += readLen;
        timestamp_t now = Timestamp::now();
        if (now >= start + timeout)
            break;
        usleep(1000);
    }
    return total;
}

int Serial::read(char *buf, int maxLen, const char *end, int timeout)
{
    int endLen = strlen(end);
    timestamp_t start = Timestamp::now();
    int total = 0;
    while (total < maxLen)
    {
        pthread_mutex_lock(&rxMutex);
        int readLen = rxStream.take(buf + total, 1);
        pthread_mutex_unlock(&rxMutex);
        if (readLen > 0)
        {
            total += readLen;
            if (endsWith(buf, total, end, endLen))
            {

                return total;
            }
        }
        timestamp_t now = Timestamp::now();
        if (now >= start + timeout)
            return READ_TIMEOUT;
        usleep(1000);
    }
    return READ_BUFFER_FULL;
}
int Serial::read_at(char *buf, int Len, const char *begin, int beginLen, int timeout)
{
    timestamp_t start = Timestamp::now();
    timestamp_t now;
    int readLen;
    for (;;)
    {
        pthread_mutex_lock(&rxMutex);
        readLen = rxStream.take(buf, 1);
        pthread_mutex_unlock(&rxMutex);

        if (readLen > 0 && buf[0] == begin[0])
        {
            readLen = rxStream.peek(buf + 1, beginLen - 1);
            if (readLen > 0 && !memcmp(buf, begin, beginLen))
            {
                pthread_mutex_lock(&rxMutex);
                readLen = rxStream.take(buf + 1, Len - 1) + 1;
                pthread_mutex_unlock(&rxMutex);
                return readLen;
            }
        }

        now = Timestamp::now();
        if (now >= start + timeout)
            return READ_TIMEOUT;
        usleep(1000);
    }
    return READ_BUFFER_FULL;
}

int Serial::flushIO()
{
    int ret;
    pthread_mutex_lock(&rxMutex);
    pthread_mutex_lock(&txMutex);

    while (rxStream.getLength() > 0)
    {
        rxStream.take();
    }
    while (txStream.getLength() > 0)
    {
        txStream.take();
    }
    ret  = tcflush(fd, TCIOFLUSH);
    pthread_mutex_unlock(&rxMutex);
    pthread_mutex_unlock(&txMutex);
    return ret;
}

int Serial::transformBaud(int baud)
{
    int map[][2] = {{2400, B2400}, {4800, B4800}, {9600, B9600}, {19200, B19200}, {38400, B38400}, {57600, B57600}, {115200, B115200}};
    for (int i = 0; i < sizeof(map) / sizeof(int) / 2; i++)
        if (map[i][0] == baud)
            return map[i][1];
    return -1;
}

int Serial::transformDataBits(int dataBits)
{
    int map[][2] = {{5, CS5}, {6, CS6}, {7, CS7}, {8, CS8}};
    for (int i = 0; i < sizeof(map) / sizeof(int) / 2; i++)
        if (map[i][0] == dataBits)
            return map[i][1];
    return -1;
}

bool Serial::endsWith(const char *str, int strLen, const char *end, int endLen)
{
    if (strLen < endLen)
        return false;
    for (int i = endLen - 1; i >= 0; i--)
        if (end[i] != str[strLen - endLen + i])
            return false;
    return true;
}

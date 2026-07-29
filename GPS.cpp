#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "GPS.h"
#include <unistd.h>
#include <iostream>
#include <QDebug>
void *GPS_ServiceThread(void *arg)
{
   GPS_service *gps = (GPS_service *)arg;
   for (;;)
   {
      gps->receive_task();
      pthread_testcancel();
      usleep(100 * 1000);
   }
   return nullptr;
}

GPS_service::GPS_service()
{
}

GPS_service::~GPS_service()
{
}

bool GPS_service::GPS_init()
{
    qDebug() << "GPS_init_start";
    const char* devList[] = { DEV_GPS_0, DEV_GPS_1,DEV_GPS_2,DEV_GPS_3,DEV_GPS_4};
    for (int i = 0; i < 5; i++) {
        qDebug() << "GPS_init trying:" << devList[i];
        if (Serial::OK == m_serial.open(devList[i], GPS_BAUD_RATE, GPS_DATA_BITS, GPS_PARITY, GPS_STOP_BITS)) {
            qDebug() << "GPS_init found GPS at:" << devList[i];
            if (pthread_create(&service_tid, 0, GPS_ServiceThread, this) != 0) {
                qDebug() << "GPSpthread create failed";
                m_serial.close();
                is_running = false;
                return false;
            }
            qDebug() << "GPS_init_end";
            is_running = true;
            return true;
        }
    }
    qDebug() << "serial open failed on all devices";
    is_running = false;
    return false;
}

bool GPS_service::GPS_reinit(){
    if(is_running){
        GPS_deinit();
    }
    qDebug() << "GPS_reinit" << endl;
    // 重试 5 次，每次等 1 秒（USB GPS 枚举需要时间）
    for (int i = 0; i < 5; i++) {
        if (GPS_init()) {
            qDebug() << "GPS_reinit success on attempt" << (i + 1);
            return true;
        }
        if (i < 4) {
            qDebug() << "GPS_reinit retry" << (i + 1) << "/5, waiting 1s...";
            sleep(1);
        }
    }
    qDebug() << "GPS_reinit failed after 5 attempts";
    return false;
}

void GPS_service::GPS_deinit()
{
    if(is_running){
        pthread_cancel(service_tid);
        pthread_join(service_tid, NULL);
        m_serial.close();
        is_running = false;
    }
}

int numb = 0;
int GPS_service::GPS_analyse(char *buff)
{
   char *ptr = NULL;
   if (strlen(buff) < 10)
   {
      return -1;
   }
   /* 如果buff字符串中包含字符"$GPRMC"则将$GPR*C的地址赋值给ptr */

   if (NULL == (ptr = strstr(buff, "$GNRMC")))
      return -1;

   /* sscanf 格式化输入 */
   // if(numb == 0){

   sscanf(ptr, "$GNRMC,%f,%c,%f,%c,%f,%c,%f,%f,%d,,,%c*", &(mGPS_info.time),
          &(mGPS_info.pos_state),
          &(mGPS_info.latitude),
          &(mGPS_info.N_S),
          &(mGPS_info.longitude),
          &(mGPS_info.E_W),
          &(mGPS_info.speed),
          &(mGPS_info.direction),
          &(mGPS_info.date),
          &(mGPS_info.mode));
       numb = 1;

   // }if(numb == 1){
   //     sscanf(ptr, "$gnrmc,%d.000,%c,%f,%c,%f,%c,%f,%f,%d,,,%c*", &(mGPS_info.time),
   //            &(mGPS_info.pos_state),
   //            &(mGPS_info.latitude),
   //            &(mGPS_info.N_S),
   //            &(mGPS_info.longitude),
   //            &(mGPS_info.E_W),
   //            &(mGPS_info.speed),
   //            &(mGPS_info.direction),
   //            &(mGPS_info.date),
   //            &(mGPS_info.mode));
   //     numb = 0;
   // }
   //std::cerr << ptr << std::endl;

   bzero(GPS_service::ptr1,128);
   int size = sizeof(ptr);
   for(int i = 0;i < 128 ; i++)
   {
       GPS_service::ptr1[i] = ptr[i];
   }
       qDebug()<< mGPS_info.time<<"--"<< mGPS_info.date;
   return 0;
}

void GPS_service::receive_task()
{
   char buff[256] = {0};
   int count = 0;
   count = m_serial.read(buff, sizeof(buff), msg_end, 500);
   if (count > 0)
   {
       char buff_t[256] = {0};
       int buff_numb = 0;
       for(int i =0;i<256;i++){
           if(buff[i]!=0){
               buff_t[buff_numb]=buff[i];
               buff_numb++;
           }
       }

      GPS_analyse(buff_t);
   }
   else
   {
      //std::cerr << "GPS_service::receive_task(): m_serial.read() failed." << count << std::endl;
   }
   return;
}

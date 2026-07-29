#ifndef __GPS_H__
#define __GPS_H__
#include "Serial.h"
#include <pthread.h>
#define DEV_GPS "/dev/ttyACM1"
#define DEV_GPS_0 "/dev/ttyACM0"
#define DEV_GPS_1 "/dev/ttyACM1"
#define DEV_GPS_2 "/dev/ttyACM2"
#define DEV_GPS_3 "/dev/ttyACM3"
#define DEV_GPS_4 "/dev/ttyACM4"
#define GPS_BAUD_RATE 9600
#define GPS_DATA_BITS 8
#define GPS_STOP_BITS 1
#define GPS_PARITY Serial::PARITY_NONE

typedef struct __gprmc__
{
   float time; /* gps定位时间 */
   char pos_state;    /*gps状态位*/
   char N_S;          /* 北纬_南纬 */
   float latitude;    /* 纬度 */
   char E_W;          /* 东经_西经 */
   float longitude;   /* 经度 */
   float speed;       /* 速度 */
   float direction;   /*航向 */
   unsigned int date; /*  日期  */
   float declination; /* 磁偏角 */
   char dd;
   char mode; /* GPS模式位 */
} GPRMC;

class GPS_service
{
public:
   GPS_service();
   ~GPS_service();
   bool GPS_init();
   bool GPS_reinit();
   void GPS_deinit();
   GPRMC mGPS_info{0};
   friend void *GPS_ServiceThread(void *arg);
   char ptr1[256] = {0};

private:
   bool is_running = false;
   Serial m_serial;
   pthread_t service_tid;
   const char *msg_end = "\r\n";
   int GPS_analyse(char *buff);
   void receive_task();
};

#endif

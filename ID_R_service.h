#ifndef ID_R_SERVICE_H
#define ID_R_SERVICE_H
#include "Serial.h"
#include <pthread.h>
#define DEV_LASER "/dev/ttyUSB0"
#define LASER_BAUD_RATE 115200
#define LASER_DATA_BITS 8
#define LASER_STOP_BITS 1
#define LASER_PARITY Serial::PARITY_NONE
#define SUCCESS 0
#define FAIL -1

class ID_R_service
{
private:
    /* data */
    typedef struct _laser_info
    {
        /* data */
        bool response;
        bool laser_light;
        unsigned int times;
        unsigned int distance;
        unsigned int max_distance;
        unsigned int min_distance;
    } laser_info;

    laser_info m_current_status{0, 0, 0, 0, 0, 0};

    void BCD2ASCII(unsigned char *buf, unsigned int num);
    unsigned int ASCII2BCD(unsigned char *buf, unsigned int length);
    int preprocess_message(unsigned char *message);
    void process_message(unsigned char *message);
    Serial m_serial;
    pthread_t service_tid;
    pthread_mutex_t m_Serial_Mutex = PTHREAD_MUTEX_INITIALIZER;

    const char *open_laser_cmd = "$0003260130&";    // 01 开
    const char *close_laser_cmd = "$0003260029&";   // 00 关
    const char *start_measure100_cmd = "$00022527&"; // 100次测量
    const char *start_continue_measure_cmd = "$00022426&";    // 连续测量
    const char *msg_start = "$";
    const char *msg_end = "&";
    int flush_serial_io_buffer(void);

public:
    ID_R_service(){};
    ~ID_R_service(){
        deinit();
    };
    bool init(void);
    void deinit(void);
    bool turn_on_laser(void);  // 开激光
    bool turn_off_laser(void); // 关激光
    bool start_continue_measure(void);  // 开测距
    bool get_laser_switch_status(void);
    unsigned int get_distance(void);
    static bool is_init_ok;
    friend void *ID_R_ServiceThread(void *arg);
};

#endif

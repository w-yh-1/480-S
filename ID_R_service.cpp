#include "ID_R_service.h"
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
bool ID_R_service::is_init_ok = false;

void *ID_R_ServiceThread(void *arg)
{
    char receivebuffer[64];
    ID_R_service *ID_R = (ID_R_service *)arg;
    int counts;
    int start_pos;
    int iii = 1;
    for (;;)
    {
        if(iii == 102){
            bool bl = ID_R->start_continue_measure();
            //std::cout << bl <<"+++++++++" << std::endl;
            iii = 0;
        }
        pthread_mutex_lock(&ID_R->m_Serial_Mutex);
        counts = ID_R->m_serial.available();
        //std::cout << "life" << iii <<"++++"<< counts << std::endl;

        if (counts > 0)
        {
            iii++;
            do
            {
                bzero(receivebuffer, sizeof(receivebuffer));
                counts = ID_R->m_serial.read(receivebuffer, sizeof(receivebuffer), ID_R->msg_end, 100);
                pthread_mutex_unlock(&ID_R->m_Serial_Mutex);

                if (start_pos = ID_R->preprocess_message((unsigned char *)receivebuffer) == FAIL)
                    break;

                ID_R->process_message((unsigned char *)&receivebuffer[start_pos]);
                // 打印当前测距值
                //std::cout <<"distance:"<<ID_R->m_current_status.distance <<"mm"<< std::endl;

            } while (0);

        }
        else
            pthread_mutex_unlock(&ID_R->m_Serial_Mutex);

        pthread_testcancel();
        usleep(200 * 1000);

    }
    return nullptr;
}

int ID_R_service::flush_serial_io_buffer(void)
{
    return m_serial.flushIO();
}

bool ID_R_service::init(void)
{
    if (is_init_ok)
        return false;

    // 打开串口
    if (Serial::OK != m_serial.open(DEV_LASER, LASER_BAUD_RATE, LASER_DATA_BITS, LASER_PARITY, LASER_STOP_BITS))
        return false;

    if (pthread_create(&service_tid, 0, ID_R_ServiceThread, this) != 0)
        return false;

    is_init_ok = true;

    return true;
}

void ID_R_service::deinit(void)
{
    is_init_ok = false;
    pthread_cancel(service_tid);
    turn_off_laser();
    m_serial.close();
    pthread_mutex_destroy(&m_Serial_Mutex);
}

/**
 * Converts a given unsigned integer into a string of ASCII digits.
 *
 * @param buf pointer to the character array where the ASCII digits will be stored
 * @param num the unsigned integer to be converted
 *
 * @return void
 */
void ID_R_service::BCD2ASCII(unsigned char *buf, unsigned int num)
{
    if (nullptr == buf)
    {
        std::cerr << "ID_R_service::BCD2ASCII(): buf is nullptr." << std::endl;
        return;
    }

    int digits;
    digits = num == 0 ? 1 : static_cast<int>(log10(num)) + 1;
    for (int i = 1; i < digits; i++)
    {
        *buf = num / ((digits - i) * 10) % 10 + '0';
        buf++;
    }
    *buf = num % 10 + '0';
}

/**
 * Converts a string of ASCII digits into a BCD (Binary-Coded Decimal) unsigned integer.
 *
 * @param buf pointer to the character array where the ASCII digits are stored
 * @param digits the number of ASCII digits in the character array
 *
 * @return the BCD unsigned integer representation of the ASCII digits
 *
 * @throws std::cerr error message if buf is nullptr, digits is 0, or non-numeric characters exist in the character array
 */
unsigned int ID_R_service::ASCII2BCD(unsigned char *buf, unsigned int digits)
{
    if (nullptr == buf)
    {
        std::cerr << "ID_R_service::ASCII2BCD(): buf is nullptr." << std::endl;
        return 0;
    }

    if (digits == 0)
    {
        std::cerr << "ID_R_service::ASCII2BCD(): digits is 0." << std::endl;
        return 0;
    }


    unsigned int ret = 0;
    for (unsigned int i = 0; i < digits; i++)
    {
        if (buf[i] < '0' || buf[i] > '9')
        {
            std::cerr << "ID_R_service::ASCII2BCD(): Non-numeric character(s) exist." << std::endl;
            return 0;
        }
        ret = ret * 10 + (buf[i] - '0');
    }

    return ret;
}


/**
 * Checks the integrity of the message by verifying if the checksum is correct.
 *
 * @param message A pointer to the message to be checked.
 *
 * @return message start position if the message is correct, FAIL otherwise.
 *
 * @throws None.
 */
int ID_R_service::preprocess_message(unsigned char *message)
{
    int message_len = 0;
    int message_date_len = 0;
    int message_sum = 0;
    int i = 0;
    while (message[i] != '$' && message[i] != '&')
        i++;

    if (message[i] != '$')
    {

        std::cerr << message; // 打印 message;
        std::cerr << "check_message(): dosent start with '$'." << std::endl;
        return FAIL;
    }
    message = message + i;
    message_len = strlen((char *)message);
    message_date_len = ASCII2BCD(&message[3], 2);
    if(message_len<message_date_len*2+6)
    {
        std::cerr << message; // 打印 message;
        std::cerr << "check_message(): len err" << std::endl;
        return FAIL;
    }
    for (int i = 0; i < message_date_len; i++)
    {
        message_sum += ASCII2BCD(&message[3 + i * 2], 2);
    }
    message_sum %= 100;
    if (message_sum != ASCII2BCD(&message[message_date_len * 2 + 3], 2))
        return FAIL;

    return i;
}

/**
 * Process a message received from the device.
 *
 * This function takes a message in binary format, extracts the message type,
 * and performs the corresponding action based on the message type.
 *
 * @param message A pointer to the message to be processed.
 *
 * @return None.
 *
 * @throws None.
 */
void ID_R_service::process_message(unsigned char *message)
{
    int message_type = ASCII2BCD(&message[5], 2);
    //std::cout <<"---------------------" << message_type << std::endl;
    switch (message_type)
    {
    case 33: // 应答
        /* code */
        m_current_status.response = true;
        break;
    case 24: // 连续测量(100次)
    case 25: // 连续测量(∞次)
        m_current_status.times = ASCII2BCD(&message[7], 4);
        m_current_status.distance = ASCII2BCD(&message[11], 8);
        m_current_status.max_distance = ASCII2BCD(&message[19], 8);
        m_current_status.min_distance = ASCII2BCD(&message[27], 8);
        break;
    case 21: // 单次测量
        m_current_status.distance = ASCII2BCD(&message[7], 8);
        break;
    case 26: // 激光开关
        m_current_status.laser_light = !m_current_status.laser_light;
        break;
    default:
        std::cerr << "ID_R_service::process_message(): Unknown message type." << message_type << std::endl;
        break;
    }
}

/**
 * Turns on the laser by sending a command to the serial device and waiting for a response.
 *
 * @return True if the laser is turned on successfully, false otherwise.
 *
 * @throws None.
 */
bool ID_R_service::turn_on_laser(void)
{
    char receivebuffer[64] = {0};
    int counts = 0;
    int retry_times = 2;
    int start_pos;

    m_current_status.response = false;
    pthread_mutex_lock(&m_Serial_Mutex);
    m_serial.write(open_laser_cmd, strlen(open_laser_cmd)); // 发送 开激光命令
    do
    {

        // 接收回传，超时等待200ms
        counts = m_serial.read(receivebuffer, sizeof(receivebuffer) / sizeof(receivebuffer[0]), msg_end, 1000);

        if (counts <= 0)
        {
            std::cerr << "ID_R_service::turn_on_laser(): m_serial.read() failed." << counts << std::endl;
            continue;
        }

        if (start_pos = preprocess_message((unsigned char *)receivebuffer) == FAIL)
        {
            bzero(receivebuffer, sizeof(receivebuffer) / sizeof(receivebuffer[0]));
            std::cerr << "ID_R_service::turn_on_laser(): check_message() failed." << std::endl;
            continue;
        }

        process_message((unsigned char *)&receivebuffer[start_pos]);
        if (!m_current_status.response)
        {
            bzero(receivebuffer, sizeof(receivebuffer) / sizeof(receivebuffer[0]));
            std::cerr << "ID_R_service::turn_on_laser(): m_current_status.response is false." << std::endl;
            continue;
        }

        pthread_mutex_unlock(&m_Serial_Mutex);
        return true;
    } while (retry_times--);

    pthread_mutex_unlock(&m_Serial_Mutex);
    return false;
}

/**
 * Turns off the laser by sending a command to the serial device and waiting for a response.
 *
 * @return True if the laser is turned off successfully, false otherwise.
 *
 * @throws None.
 */
bool ID_R_service::turn_off_laser(void)
{
    char receivebuffer[64] = {0};
    int counts = 0;
    int retry_times = 3;
    int start_pos;

    m_current_status.response = false;
    pthread_mutex_lock(&m_Serial_Mutex);
    // 发送 关激光命令
    m_serial.write(close_laser_cmd, strlen(close_laser_cmd));
    do
    {
        // 接收回传，超时等待200ms
        counts = m_serial.read(receivebuffer, sizeof(receivebuffer) / sizeof(receivebuffer[0]), msg_end, 200);

        if (counts <= 0)
        {
            std::cerr << "ID_R_service::turn_off_laser(): m_serial.read() failed." << counts << std::endl;
            continue;
        }

        if (start_pos = preprocess_message((unsigned char *)receivebuffer) == FAIL)
        {
            std::cerr << "ID_R_service::turn_off_laser(): check_message() failed." << std::endl;
            continue;
        }

        process_message((unsigned char *)&receivebuffer[start_pos]);
        std::cout <<"---------------------" << (int)receivebuffer[start_pos] << std::endl;
        if (!m_current_status.response)
        {
            std::cerr << "ID_R_service::turn_off_laser(): m_current_status.response is false." << std::endl;
            continue;
        }

        pthread_mutex_unlock(&m_Serial_Mutex);
        return true;
    } while (retry_times--);

    pthread_mutex_unlock(&m_Serial_Mutex);
    return false;
}
// 开始连续测量
bool ID_R_service::start_continue_measure(void)
{
    char receivebuffer[64] = {0};
    int counts = 0;
    int retry_times = 3;
    int start_pos;

    m_current_status.response = false;
    pthread_mutex_lock(&m_Serial_Mutex);
    // 发送 关激光命令
    m_serial.write(start_continue_measure_cmd, strlen(start_continue_measure_cmd));

    do
    {
        // 接收回传，超时等待200ms
        counts = m_serial.read(receivebuffer, sizeof(receivebuffer) / sizeof(receivebuffer[0]), msg_end, 200);

        if (counts <= 0)
        {
            std::cerr << "ID_R_service::start_continue_measure(): m_serial.read() failed." << counts << std::endl;
            continue;
        }

        if (start_pos = preprocess_message((unsigned char *)receivebuffer) == FAIL)
        {
            std::cerr << "ID_R_service::start_continue_measure(): check_message() failed." << std::endl;
            bzero(receivebuffer, sizeof(receivebuffer) / sizeof(receivebuffer[0]));
            continue;
        }

        process_message((unsigned char *)&receivebuffer[start_pos]);

        if (!m_current_status.response)
        {
            std::cerr << "ID_R_service::start_continue_measure(): m_current_status.response is false." << std::endl;
            bzero(receivebuffer, sizeof(receivebuffer) / sizeof(receivebuffer[0]));
            continue;
        }
        pthread_mutex_unlock(&m_Serial_Mutex);
        return true;
    } while (retry_times--);

    pthread_mutex_unlock(&m_Serial_Mutex);
    turn_off_laser();
    return false;
}

bool ID_R_service::get_laser_switch_status(void)
{
    return m_current_status.laser_light;
}

unsigned int ID_R_service::get_distance(void)
{
    return m_current_status.distance;
}

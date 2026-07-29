//
// Created by bxc on 2022/12/15.
// Improved by leefoo on 2024/12/22.
//
#include "RtpSendPs.h"
#include "Log.h"

#ifdef WIN32
#include <WS2tcpip.h>
#endif

// #define RTP_NONBLOCK
RtpSendPs::RtpSendPs(const char *rtpServerIp, int rtpServerPort, int localRtpPort, bool isTcp, uint32_t ssrc) : mRtpServerIp(rtpServerIp),
                                                                                                                mRtpServerPort(rtpServerPort),
                                                                                                                mLocalRtpPort(localRtpPort),
                                                                                                                mIsTcp(isTcp),
                                                                                                                mSsrc(ssrc),
                                                                                                                mIsStop(true)
{
    strcpy(ip_buffer,mRtpServerIp);
    if ((mSockFd = socket(AF_INET, mIsTcp ? SOCK_STREAM : SOCK_DGRAM, mIsTcp ? IPPROTO_TCP : IPPROTO_UDP)) == -1)
    {
        LOGE("创建套接字失败");
        return;
    }
    int ret;
    // 为udp的socket绑定指定IP或指定端口 start
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(mLocalRtpPort);
    // addr.sin_addr.s_addr = inet_addr("localRtpIp");
    addr.sin_addr.s_addr = INADDR_ANY;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    ret = bind(mSockFd, (struct sockaddr *)&addr, addr_len);
    if (ret < 0)
    {
        LOGE("绑定套接字失败");
        return;
    }
    // 为udp的socket绑定指定IP或指定端口 end
    if (mIsTcp)
    {
        addr.sin_port = htons(mRtpServerPort);
        inet_pton(AF_INET, ip_buffer, &addr.sin_addr);
        if (connect(mSockFd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            LOGE("Error connecting to remote server");
            close(mSockFd);
            return;
        }
    }
int on=1;
 setsockopt(mSockFd,SOL_SOCKET,SO_REUSEADDR | SO_BROADCAST,&on,sizeof(on));
//
#ifdef RTP_NONBLOCK
#ifndef WIN32 // Linux系统

    int ul = 1;
    ret = ioctl(mSockFd, FIONBIO, &ul); // 设置为非阻塞模式
    if (ret == -1)
    {
        LOGE("设置套接字非阻塞失败");
        return;
    }
#else
    unsigned long ul = 1;
    ret = ioctlsocket(mSockFd, FIONBIO, (unsigned long *)&ul); // 设置非阻塞

    if (ret == SOCKET_ERROR)
    {
        LOGE("设置套接字非阻塞失败");
        return;
    }
#endif // !WIN32
#endif // RTP_NONBLOCK
}
RtpSendPs::~RtpSendPs()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    if (mThread)
    {
        mThread->join();
        delete mThread;
        mThread = nullptr;
    }

    if (mSockFd > -1)
    {
#ifndef WIN32 // Linux系统
        close(mSockFd);
#else
        closesocket(mSockFd);
#endif // !WIN32
        mSockFd = -1;
    }
}

NaluInfo RtpSendPs::getNextH264Nalu(FILE *file, unsigned char *buffer)
{
    NaluInfo naluInfo = {0};
    static const unsigned char START_CODE_4BYTES[] = {0x00, 0x00, 0x00, 0x01};
    static const unsigned char START_CODE_3BYTES[] = {0x00, 0x00, 0x01};
    static unsigned int lastStartCodeSize = 0;
    unsigned int bytesRead = 0;
    unsigned int naluSize = 0;
    unsigned int startCodeFound = 0;
    unsigned int startCodeSize = 0;

    if (lastStartCodeSize > 0)
    {
        naluSize = lastStartCodeSize;
        startCodeSize = lastStartCodeSize;
        startCodeFound = 1;
        switch (startCodeSize)
        {
        case 3:
            memcpy(buffer, START_CODE_3BYTES, 3);
            break;
        case 4:
            memcpy(buffer, START_CODE_4BYTES, 4);
            break;
        default:
            break;
        }
        lastStartCodeSize = 0;
    }
    // 循环读取数据，直到找到起始码
    while (!startCodeFound && !feof(file))
    {
        bytesRead = fread(&buffer[naluSize], 1, 1, file);
        if (bytesRead == 0)
        {
            break;
        }
        naluSize++;
        // 检查起始码
        if (naluSize >= 3 && memcmp(&buffer[naluSize - 3], START_CODE_3BYTES, 3) == 0)
        {
            startCodeFound = 1;
            startCodeSize = 3;
            break;
        }
        else if (naluSize >= 4 && memcmp(&buffer[naluSize - 4], START_CODE_4BYTES, 4) == 0)
        {
            startCodeFound = 1;
            startCodeSize = 4;
            break;
        }
    }

    if (startCodeFound)
    {
        startCodeFound = 0;
        // 继续读取直到下一个起始码或文件结束
        while (!feof(file) && !startCodeFound)
        {
            bytesRead = fread(&buffer[naluSize], 1, 1, file);
            if (bytesRead == 0)
            {
                break;
            }
            naluSize++;

            // 检查下一个起始码

            if (naluSize >= 3 && memcmp(&buffer[naluSize - 3], START_CODE_3BYTES, 3) == 0)
            {
                startCodeFound = 1;
                lastStartCodeSize = 3;
                break;
            }
            else if (naluSize >= 4 && memcmp(&buffer[naluSize - 4], START_CODE_4BYTES, 4) == 0)
            {
                startCodeFound = 1;
                lastStartCodeSize = 4;
                break;
            }
        }
        if (startCodeFound)
        {
            naluInfo.naluLength = naluSize - startCodeSize;
            naluInfo.naluType = buffer[startCodeSize] & 0x1f;
        }
    }
    return naluInfo;
}

/***
 *@remark:  音视频数据的打包成ps流，并封装成rtp
 *@param :  pData      [in] 需要发送的音视频数据
 *          nFrameLen  [in] 发送数据的长度
 *          pPacker    [in] 数据包的一些信息，包括时间戳，rtp数据buff，发送的socket相关信息
 *          stream_type[in] 数据类型 0 视频 1 音频
 *@return:  0 success others failed
 */

int RtpSendPs::gb28181_streampackageForH264(char *pData, int nFrameLen, Data_Info_s *pPacker, int stream_type)
{
    char *pSendBuff = pPacker->sendBuff + RTP_HDR_LEN;
    int nPos = 0;
    int nSize = 0;

    memset(pPacker->sendBuff, 0, 256);
    // 1 package for ps header
    gb28181_make_ps_header(pSendBuff, pPacker->s64CurPts);
    nPos += PS_HDR_LEN;
    // 2 system header
    if (pPacker->naluType == 0x07) // sps，代表IDR帧开始
    {
        // 如果是I帧的话，则添加sys头和psm头
        gb28181_make_sys_header(pSendBuff + nPos);
        nPos += SYS_HDR_LEN;
        gb28181_make_psm_header(pSendBuff + nPos);
        nPos += PSM_HDR_LEN;
    }
    // 后面的数据分片分包并添加pes头和rtp头
    while (nFrameLen > 0)
    {
        // pes帧的长度不要超过short类型，超过就需要分片循环行发送
        nSize = (nFrameLen > PS_PES_PAYLOAD_SIZE) ? PS_PES_PAYLOAD_SIZE : nFrameLen;
        // 添加pes头
        gb28181_make_pes_header(pSendBuff + nPos, stream_type ? 0xC0 : 0xE0, nSize, pPacker->s64CurPts, pPacker->s64CurPts);
        nPos += PES_HDR_LEN;
        // 添加rtp头并发送数据
        gb28181_send_rtp_pack(pData, nSize, nPos, ((nSize == nFrameLen) ? 1 : 0), pPacker);
        // 每次分片发送后计算剩余数据的长度，并移动数据的指针
        nFrameLen -= nSize;
        pData += nSize;
        nPos = 0;
    }
    return 0;
}

/***
 *@remark:   ps头的封装,里面的具体数据的填写已经占位，可以参考标准
 *@param :   pData  [in] 填充ps头数据的地址
 *           s64Src [in] 时间戳
 *@return:   0 success, others failed
 */
int RtpSendPs::gb28181_make_ps_header(char *pData, unsigned long long s64Scr)
{
#ifdef EXT_CLOCK_FREQ
    unsigned long long lScrBase = s64Scr / 300;
    unsigned long long lScrExt = s64Scr % 300;
#else
    unsigned long long lScrBase = s64Scr;
    unsigned long long lScrExt = 0;
#endif
    bits_buffer_s bitsBuffer;
    bitsBuffer.i_size = PS_HDR_LEN;
    bitsBuffer.i_data = 0;
    bitsBuffer.i_mask = 0x80; // 二进制：1000 0000 这里是为了后面对一个字节的每一位进行操作，避免大小端夸字节字序错乱
    bitsBuffer.p_data = (unsigned char *)(pData);
    memset(bitsBuffer.p_data, 0, PS_HDR_LEN);
    // byte:0-3
    bits_write(&bitsBuffer, 32, 0x000001BA); /*start codes*/
    // byte:4
    bits_write(&bitsBuffer, 2, 1);                       /*marker bits '01b'*/
    bits_write(&bitsBuffer, 3, (lScrBase >> 30) & 0x07); /*System clock [32..30]*/
    bits_write(&bitsBuffer, 1, 1);                       /*marker bit*/
    // byte:4-6
    bits_write(&bitsBuffer, 15, (lScrBase >> 15) & 0x7FFF); /*System clock [29..15]*/
    bits_write(&bitsBuffer, 1, 1);                          /*marker bit*/
    // byte:6-8
    bits_write(&bitsBuffer, 15, lScrBase & 0x7fff); /*System clock [14..0]*/
    bits_write(&bitsBuffer, 1, 1);                  /*marker bit*/
    // byte:8-9
    bits_write(&bitsBuffer, 9, lScrExt & 0x01ff); /*System clock ext. 27MHZ时基用300*base+ext表示时间戳*/
    bits_write(&bitsBuffer, 1, 1);                /*marker bit*/
    // byte:10
    bits_write(&bitsBuffer, 22, (255) & 0x3fffff); /*bit rate(n units of 50 bytes/400 bit per second.)*/
    bits_write(&bitsBuffer, 2, 3);                 /*marker bits '11'*/
    bits_write(&bitsBuffer, 5, 0x1f);              /*reserved(reserved for future use)*/
    bits_write(&bitsBuffer, 3, 0);                 /*stuffing length*/
    return 0;
}

/***
 *@remark:   sys头的封装,里面的具体数据的填写已经占位，可以参考标准
 *@param :   pData  [in] 填充ps头数据的地址
 *@return:   0 success, others failed
 */
int RtpSendPs::gb28181_make_sys_header(char *pData)
{

    bits_buffer_s bitsBuffer;
    bitsBuffer.i_size = SYS_HDR_LEN;
    bitsBuffer.i_data = 0;
    bitsBuffer.i_mask = 0x80;
    bitsBuffer.p_data = (unsigned char *)(pData);
    memset(bitsBuffer.p_data, 0, SYS_HDR_LEN);
    /*system header*/
    bits_write(&bitsBuffer, 32, 0x000001BB);      /*start code*/
    bits_write(&bitsBuffer, 16, SYS_HDR_LEN - 6); /*header_length 表示次字节后面的长度，后面的相关头也是次意思*/
    bits_write(&bitsBuffer, 1, 1);                /*marker_bit*/
    bits_write(&bitsBuffer, 22, 50000);           /*rate_bound*/
    bits_write(&bitsBuffer, 1, 1);                /*marker_bit*/
    bits_write(&bitsBuffer, 6, 1);                /*audio_bound*/
    bits_write(&bitsBuffer, 1, 0);                /*fixed_flag */
    bits_write(&bitsBuffer, 1, 1);                /*CSPS_flag */
    bits_write(&bitsBuffer, 1, 1);                /*system_audio_lock_flag*/
    bits_write(&bitsBuffer, 1, 1);                /*system_video_lock_flag*/
    bits_write(&bitsBuffer, 1, 1);                /*marker_bit*/
    bits_write(&bitsBuffer, 5, 1);                /*video_bound*/
    bits_write(&bitsBuffer, 1, 0);                /*dif from mpeg1*/
    bits_write(&bitsBuffer, 7, 0x7F);             /*reserver*/
    /*audio stream bound*/
    bits_write(&bitsBuffer, 8, 0xC0); /*stream_id*/
    bits_write(&bitsBuffer, 2, 3);    /*marker_bit */
    bits_write(&bitsBuffer, 1, 0);    /*PSTD_buffer_bound_scale*/
    bits_write(&bitsBuffer, 13, 512); /*PSTD_buffer_size_bound*/
    /*video stream bound*/
    bits_write(&bitsBuffer, 8, 0xE0);  /*stream_id*/
    bits_write(&bitsBuffer, 2, 3);     /*marker_bit */
    bits_write(&bitsBuffer, 1, 1);     /*PSTD_buffer_bound_scale*/
    bits_write(&bitsBuffer, 13, 2048); /*PSTD_buffer_size_bound*/
    return 0;
}

/***
 *@remark:   psm头的封装,里面的具体数据的填写已经占位，可以参考标准
 *@param :   pData  [in] 填充ps头数据的地址
 *@return:   0 success, others failed
 */
int RtpSendPs::gb28181_make_psm_header(char *pData)
{

    bits_buffer_s bitsBuffer;
    bitsBuffer.i_size = PSM_HDR_LEN;
    bitsBuffer.i_data = 0;
    bitsBuffer.i_mask = 0x80;
    bitsBuffer.p_data = (unsigned char *)(pData);
    memset(bitsBuffer.p_data, 0, PSM_HDR_LEN);
    bits_write(&bitsBuffer, 24, 0x000001); /*start code*/
    bits_write(&bitsBuffer, 8, 0xBC);      /*map stream id*/
    bits_write(&bitsBuffer, 16, 18);       /*program stream map length*/
    bits_write(&bitsBuffer, 1, 1);         /*current next indicator */
    bits_write(&bitsBuffer, 2, 3);         /*reserved*/
    bits_write(&bitsBuffer, 5, 0);         /*program stream map version*/
    bits_write(&bitsBuffer, 7, 0x7F);      /*reserved */
    bits_write(&bitsBuffer, 1, 1);         /*marker bit */
    bits_write(&bitsBuffer, 16, 0);        /*programe stream info length*/
    bits_write(&bitsBuffer, 16, 8);        /*elementary stream map length  is*/
    /*audio*/
    bits_write(&bitsBuffer, 8, 0x90); /*stream_type*/
    bits_write(&bitsBuffer, 8, 0xC0); /*elementary_stream_id*/
    bits_write(&bitsBuffer, 16, 0);   /*elementary_stream_info_length is*/
    /*video*/
    bits_write(&bitsBuffer, 8, 0x1B); /*stream_type*/
    bits_write(&bitsBuffer, 8, 0xE0); /*elementary_stream_id*/
    bits_write(&bitsBuffer, 16, 0);   /*elementary_stream_info_length */
    /*crc (2e b9 0f 3d)*/
    bits_write(&bitsBuffer, 8, 0x45); /*crc (24~31) bits*/
    bits_write(&bitsBuffer, 8, 0xBD); /*crc (16~23) bits*/
    bits_write(&bitsBuffer, 8, 0xDC); /*crc (8~15) bits*/
    bits_write(&bitsBuffer, 8, 0xF4); /*crc (0~7) bits*/
    return 0;
}

/***
 *@remark:   pes头的封装,里面的具体数据的填写已经占位，可以参考标准
 *@param :   pData      [in] 填充ps头数据的地址
 *           stream_id  [in] 码流类型
 *           paylaod_len[in] 负载长度
 *           pts        [in] 时间戳
 *           dts        [in]
 *@return:   0 success, others failed
 */
int RtpSendPs::gb28181_make_pes_header(char *pData, int stream_id, int payload_len, unsigned long long pts, unsigned long long dts)
{

    bits_buffer_s bitsBuffer;
    bitsBuffer.i_size = PES_HDR_LEN;
    bitsBuffer.i_data = 0;
    bitsBuffer.i_mask = 0x80;
    bitsBuffer.p_data = (unsigned char *)(pData);
    memset(bitsBuffer.p_data, 0, PES_HDR_LEN);
    /*system header*/
    bits_write(&bitsBuffer, 24, 0x000001);                          /*start code*/
    bits_write(&bitsBuffer, 8, (stream_id));                        /*streamID*/
    bits_write(&bitsBuffer, 16, (payload_len) + 13); /*packet_len*/ // 指出pes分组中数据长度和该字节后的长度和
    bits_write(&bitsBuffer, 2, 2);                                  /*'10'*/
    bits_write(&bitsBuffer, 2, 0);                                  /*scrambling_control*/
    bits_write(&bitsBuffer, 1, 0);                                  /*priority*/
    bits_write(&bitsBuffer, 1, 0);                                  /*data_alignment_indicator*/
    bits_write(&bitsBuffer, 1, 0);                                  /*copyright*/
    bits_write(&bitsBuffer, 1, 0);                                  /*original_or_copy*/
    bits_write(&bitsBuffer, 1, 1);                                  /*PTS_flag*/
    bits_write(&bitsBuffer, 1, 1);                                  /*DTS_flag*/
    bits_write(&bitsBuffer, 1, 0);                                  /*ESCR_flag*/
    bits_write(&bitsBuffer, 1, 0);                                  /*ES_rate_flag*/
    bits_write(&bitsBuffer, 1, 0);                                  /*DSM_trick_mode_flag*/
    bits_write(&bitsBuffer, 1, 0);                                  /*additional_copy_info_flag*/
    bits_write(&bitsBuffer, 1, 0);                                  /*PES_CRC_flag*/
    bits_write(&bitsBuffer, 1, 0);                                  /*PES_extension_flag*/
    bits_write(&bitsBuffer, 8, 10);                                 /*header_data_length*/
    // 指出包含在 PES 分组标题中的可选字段和任何填充字节所占用的总字节数。该字段之前
    // 的字节指出了有无可选字段。

    /*PTS,DTS*/
    bits_write(&bitsBuffer, 4, 3);                    /*'0011'*/
    bits_write(&bitsBuffer, 3, ((pts) >> 30) & 0x07); /*PTS[32..30]*/
    bits_write(&bitsBuffer, 1, 1);
    bits_write(&bitsBuffer, 15, ((pts) >> 15) & 0x7FFF); /*PTS[29..15]*/
    bits_write(&bitsBuffer, 1, 1);
    bits_write(&bitsBuffer, 15, (pts) & 0x7FFF); /*PTS[14..0]*/
    bits_write(&bitsBuffer, 1, 1);
    bits_write(&bitsBuffer, 4, 1);                    /*'0001'*/
    bits_write(&bitsBuffer, 3, ((dts) >> 30) & 0x07); /*DTS[32..30]*/
    bits_write(&bitsBuffer, 1, 1);
    bits_write(&bitsBuffer, 15, ((dts) >> 15) & 0x7FFF); /*DTS[29..15]*/
    bits_write(&bitsBuffer, 1, 1);
    bits_write(&bitsBuffer, 15, (dts) & 0x7FFF); /*DTS[14..0]*/
    bits_write(&bitsBuffer, 1, 1);
    return 0;
}

/**
 * @remark   rtp头的打包，并循环发送数据
 * @param :  pData      [in] 发送的数据地址
 *           nDatalen   [in] 发送数据的长度
 *           mark_flag  [in] mark标志位
 *           curpts     [in] 时间戳
 *           pPacker    [in] 数据包的基本信息
 * @return:  0 success, others failed
 */
int RtpSendPs::gb28181_send_rtp_pack(char *databuff, uint nDataLen, uint nPsHeaderLen, int mark_flag, Data_Info_s *pPacker)
{
    char *pSendBuff = pPacker->sendBuff;
    uint nSendSize;
    int ret;
    while (nDataLen > 0)
    {
        nSendSize = std::min(nDataLen, RTP_MAX_PACKET_BUFF - RTP_HDR_LEN - nPsHeaderLen);
        gb28181_make_rtp_header(pSendBuff, (nSendSize == nDataLen) ? mark_flag : 0, ++pPacker->u16CSeq, pPacker->s64CurPts, pPacker->u32Ssrc);
        memcpy(pSendBuff + RTP_HDR_LEN + nPsHeaderLen, databuff, nSendSize);
        ret = SendDataBuff(pSendBuff, RTP_HDR_LEN + nPsHeaderLen + nSendSize);
        if (ret != (RTP_HDR_LEN + nSendSize + nPsHeaderLen))
        {
            LOGE("SendDataBuff error: ret=%d,should=%d,error: %s", ret, RTP_HDR_LEN + nSendSize + nPsHeaderLen, strerror(errno));
            return -1;
        }

        databuff += nSendSize;
        nDataLen -= nSendSize;
        nPsHeaderLen = 0; // PS头部已经发送完毕，不再计入
    }
    return 0;
}

/**
 * @remark 设置rtp头
 * @param pData
 * @param marker_flag
 * @param cseq
 * @param curpts
 * @param ssrc
 * @return
 */
int RtpSendPs::gb28181_make_rtp_header(char *pData, int marker_flag, unsigned short cseq, long long curpts, unsigned int ssrc)
{
    bits_buffer_s bitsBuffer;
    if (pData == NULL)
        return -1;
    bitsBuffer.i_size = RTP_HDR_LEN;
    bitsBuffer.i_data = 0;
    bitsBuffer.i_mask = 0x80;
    bitsBuffer.p_data = (unsigned char *)(pData);
    memset(bitsBuffer.p_data, 0, RTP_HDR_LEN);
    // byte:0
    bits_write(&bitsBuffer, 2, RTP_VERSION); /* rtp version  */
    bits_write(&bitsBuffer, 1, 0);           /* rtp padding  */
    bits_write(&bitsBuffer, 1, 0);           /* rtp extension  */
    bits_write(&bitsBuffer, 4, 0);           /* rtp CSRC count */
    // byte:1
    bits_write(&bitsBuffer, 1, (marker_flag)); /* rtp marker   */
    bits_write(&bitsBuffer, 7, 96);            /* rtp payload type*/
    // byte:2-3
    bits_write(&bitsBuffer, 16, (cseq)); /* rtp sequence    */
    // byte:4-7
    bits_write(&bitsBuffer, 32, (curpts)); /* rtp timestamp   */
    // byte:8-11
    bits_write(&bitsBuffer, 32, (ssrc)); /* rtp SSRC    */
    return 0;
}

#ifdef RTP_NONBLOCK
// 发送数据包
int RtpSendPs::SendDataBuff(char *buff, int size)
{
    int len;
    int ret = 0;
    int retry = 5;
    sockaddr_in addr_serv;
    if (mSockFd <= 0)
        return -1;

    // send函数专用于TCP链接，sendto函数专用与UDP连接。
    while (retry--)
    {
        if (mIsTcp)
        {
            ret = send(mSockFd, buff, size, 0);
        }
        else
        {
            memset(&addr_serv, 0, sizeof(addr_serv)); // memset 在一段内存块中填充某个给定的值，它是对较大的结构体或数组进行清零操作的一种最快方法
            addr_serv.sin_family = AF_INET;
            addr_serv.sin_addr.s_addr = inet_addr(ip_buffer);
            addr_serv.sin_port = htons(mRtpServerPort);
            len = sizeof(addr_serv);
            ret = sendto(mSockFd, buff, size, 0, (struct sockaddr *)&addr_serv, len);
        }
        if (ret < 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
    return ret;
}
#else
int RtpSendPs::SendDataBuff(char *buff, int size)
{
    int len;
    int ret = 0;
    static int count=0;
    sockaddr_in addr_serv;
    if (mSockFd <= 0)
        return -1;
    // send函数专用于TCP链接，sendto函数专用与UDP连接。
    if (mIsTcp)
    {
        ret = send(mSockFd, buff, size, 0);
    }
    else
    {
        memset(&addr_serv, 0, sizeof(addr_serv)); // memset 在一段内存块中填充某个给定的值，它是对较大的结构体或数组进行清零操作的一种最快方法
        addr_serv.sin_family = AF_INET;
        addr_serv.sin_addr.s_addr = inet_addr(ip_buffer);
        addr_serv.sin_port = htons(mRtpServerPort);
        len = sizeof(addr_serv);
        ret = sendto(mSockFd, buff, size, 0, (struct sockaddr *)&addr_serv, len);
        if(count++%100==0)
        {
            char ip_str[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &addr_serv.sin_addr, ip_str, sizeof(ip_str));

        }

    }
    return ret;
}
#endif
void RtpSendPs::SendData(void *ptr, size_t len, uint8_t type, bool isEnd)
{
    static Data_Info_s pPacker{
        0,    // s64CurPts
        0,    // IFrame
        0,    // u16CSeq
        mSsrc // u32Ssrc
    };

    if (mIsStop || ptr == NULL || len == 0)
        return;

    pPacker.naluType = type & 0x1f;
    switch (pPacker.naluType)
    {
    case 0x07: // SPS
    case 0x08: // PPS
    case 0x05: // I
    case 0x01: // P
    case 0x09: // AUD分隔符
        break;
    default:
        LOGI("未知的nalu类型:%02x", pPacker.naluType);
        return;
    }
    gb28181_streampackageForH264((char *)ptr, len, &pPacker, 0); // 0 表示传递的是视频数据
    if (isEnd && (pPacker.naluType == 0x01 || pPacker.naluType == 0x05))
        pPacker.s64CurPts += 3200;
}
void RtpSendPs::SendDataThread(void *arg)
{
    RtpSendPs *rtpSendPs = (RtpSendPs *)arg;

    Data_Info_s pPacker;
    pPacker.naluType = 0;
    pPacker.u32Ssrc = rtpSendPs->mSsrc; // 10进制的ssrc
    pPacker.s64CurPts = 0;
    const char *filename = "./test-long.h264";
    FILE *fp = fopen(filename, "rb");
    NaluInfo curNaluInfo = {0};
    if (!fp)
    {
        LOGE("fopen error:%s", filename);
        return;
    }
    char *buf = (char *)malloc(1024 * 1024);

    while (!rtpSendPs->mIsStop)
    {
        curNaluInfo = rtpSendPs->getNextH264Nalu(fp, (unsigned char *)buf);
        if (curNaluInfo.naluLength <= 0)
        {
            LOGI("发送数据已完成，主动退出");
            rtpSendPs->mIsStop = true;
            break;
        }
        // 将h264码流读取到的一个一个nalu封装到ps并通过rtp推流
        pPacker.naluType = curNaluInfo.naluType;
        switch (pPacker.naluType)
        {
        case 0x07: // SPS
        case 0x08: // PPS
        case 0x05: // I
        case 0x01: // P
        case 0x09: // AUD分隔符
            break;
        default:
            LOGI("未知的nalu类型:%02x", pPacker.naluType);
            continue;
        }
        rtpSendPs->gb28181_streampackageForH264(buf, curNaluInfo.naluLength, &pPacker, 0); // 末尾0 表示传递的是视频数据
        if (pPacker.naluType == 0x01 || pPacker.naluType == 0x05)
        {
            pPacker.s64CurPts += 3600;
            std::this_thread::sleep_for(std::chrono::milliseconds(35)); // 35ms
        }
    }
    // 关闭文件,释放资源
    free(buf);
    buf = nullptr;
    fclose(fp);
}
void RtpSendPs::start()
{
    if (mSockFd > -1)
        mIsStop = false;
}
void RtpSendPs::stop()
{
    mIsStop = true;
}

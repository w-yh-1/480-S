extern "C"{
#include<stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
 #include <string.h>
#include <stdlib.h>

}
#include "myrk_decode.h"
#include "drm_display.h"
MyrkDecode* MyrkDecode::sInstance=NULL;

MyrkDecode::MyrkDecode(MppCodingType type,int width,int height)
{
    mDecodeType = type;
    mWidth = width;
    mHeight = height;
}
MyrkDecode::~MyrkDecode()
{
}
int MyrkDecode::initDecode()
{
    int ret = 0;
    MpiCmd mpi_cmd = MPP_CMD_BASE;
    MppParam split;
    RK_U32 need_split = 1;
    mPktBuf = new char[PKT_SIZE];
    int hor_stride = CODEC_ALIGN(mWidth, 16);
    int ver_srride = CODEC_ALIGN(mHeight, 16);
    ret = mpp_packet_init(&mPkt, mPktBuf, PKT_SIZE);
    if (ret) {
        printf("mpp_packet_init error %d", ret);
        return -1;
    }
    ret = mpp_create(&mCtx, &mApi);
    if (ret != MPP_OK) {
        printf("mpp_create error %d", ret);
        return -1;
    }
    //流读取
    mpi_cmd = MPP_DEC_SET_PARSER_SPLIT_MODE;
    split = &need_split;
    ret = mApi->control(mCtx, mpi_cmd, split);
    if (ret != MPP_OK) {
        printf("mApi->control error\n");
        return	-1;
    }

    ret = mpp_init(mCtx, MPP_CTX_DEC, mDecodeType);
    if (ret != MPP_OK) {
        printf("mpp_init error \n");
        return -1;
    }
    ret = mpp_frame_init(&mdstFrm);
    if (ret) {
        printf("mpp_frame_init error %d\n", ret);
        return -1;
    }

    mpp_buffer_get(mmFrmGrp,&mmBuffer,10*hor_stride * ver_srride * 3 / 2);
    // mpp_frame_set_width(mdstFrm, mWidth);
    // mpp_frame_set_height(mdstFrm, mHeight);
    // mpp_frame_set_hor_stride(mdstFrm, hor_stride);
    // mpp_frame_set_ver_stride(mdstFrm, ver_srride);
    mpp_frame_set_width(mdstFrm, mHeight);
    mpp_frame_set_height(mdstFrm, mWidth);
    mpp_frame_set_hor_stride(mdstFrm, ver_srride);
    mpp_frame_set_ver_stride(mdstFrm, hor_stride);
    mpp_frame_set_fmt(mdstFrm, MPP_FMT_YUV420SP);
    mpp_frame_set_buffer(mdstFrm, mmBuffer);
    return 0;
}
MyrkDecode* MyrkDecode::getInstance()
{

    if(sInstance==NULL)
    {
        sInstance = new MyrkDecode(MPP_VIDEO_CodingAVC,1920,1088);
    }
    return sInstance;
}
void* MyrkDecode::sendStreamToDecode(void *args)
{
    FILE *mFile;
    int readLen = 0;
    int ret = 0;
    decodeParamer *decodeP = (decodeParamer*)(args);
    MppPacket smPkt = *decodeP->packet;
    MppApi *smApi = decodeP->mApi;
    MppCtx smCtx = *decodeP->mCtx;
    char *buf = (char*)malloc(READ_SIZE);
    if(buf==NULL)
        return NULL;
    mFile = fopen("/root/Tennis1080p.h264","rb");
    if(!mFile)
    {
        printf("open file error \n");
        return NULL;
    }
    while(1)
    {
        readLen =fread(buf, 1, READ_SIZE, mFile);
        if(readLen > 0)
        {
            mpp_packet_write(smPkt, 0, buf, readLen);
            mpp_packet_set_pos(smPkt, buf);
            mpp_packet_set_length(smPkt, readLen);
            retryagin:
            ret = smApi->decode_put_packet(smCtx, smPkt);
            if (ret != MPP_OK)
            {
                usleep(10000);
                printf("retry agein \n");
                goto retryagin;
            }
            //printf("readLen:%d \n",readLen);
         }
         else
         {
            fseek(mFile,0,SEEK_SET);
            continue;
         }
         usleep(1000*30);

    }
    return NULL;
}
void* MyrkDecode::getFrameForDecode(void *args)
{
    int ret = 0;
    decodeParamer *decodeP = (decodeParamer*)(args);
    MppPacket smPkt = *decodeP->packet;
    MppApi *smApi = decodeP->mApi;
    MppCtx smCtx = *decodeP->mCtx;
    MppFrame *srcFrm = decodeP->srcFrm;
    MppFrame *dstFrm = decodeP->dstFrm;
    MppBufferGroup *mFrmGrp = decodeP->frameGrp;
    while(1)
    {
        try_again:
        ret = smApi->decode_get_frame(smCtx,srcFrm);
        if(ret==MPP_ERR_TIMEOUT)
        {
            printf("time out \n");
            usleep(1000*40);
            goto try_again;
        }
        if (ret == MPP_OK)
        {
            if (*srcFrm)
            {
                if (mpp_frame_get_info_change(*srcFrm))
                {
                    RK_U32 width = mpp_frame_get_width(*srcFrm);
                    RK_U32 height = mpp_frame_get_height(*srcFrm);
                    RK_U32 hor_stride = mpp_frame_get_hor_stride(*srcFrm);
                    RK_U32 ver_stride = mpp_frame_get_ver_stride(*srcFrm);

                    ret = mpp_buffer_group_get_internal(mFrmGrp, MPP_BUFFER_TYPE_DRM);
                    if (ret) {
                          printf("get mpp buffer group failed ret %d.\n", ret);
                          break;
                    }

                    smApi->control(smCtx, MPP_DEC_SET_EXT_BUF_GROUP, *mFrmGrp);
                    smApi->control(smCtx, MPP_DEC_SET_INFO_CHANGE_READY, NULL);
                }
                else
                {
                    //memcpy(dstFrm,srcFrm,sizeof(MppFrame));
                    //此处进行RGA合成，如需要多路合成，可自行处理
                    static int count = 0;
                    sp_bo *bo;
                    uint32_t handles[4], pitches[4], offsets[4];
                    int width, height;
                    int frm_size, ret, fd, err;
                    RK_U32 hor_stride = mpp_frame_get_hor_stride(*srcFrm);
                    RK_U32 ver_stride = mpp_frame_get_ver_stride(*srcFrm);
                    width = mpp_frame_get_width(*srcFrm);
                    height = mpp_frame_get_height(*srcFrm);
                    rga_info_t dst_info;
                    memset(&dst_info,0,sizeof(rga_info_t));
                    dst_info.fd = mpp_buffer_get_fd(mpp_frame_get_buffer(*dstFrm));
                   //dst_info.mmuFlag = 1;
                    dst_info.virAddr = NULL;
                    dst_info.phyAddr = NULL;
                    rga_info_t src_info;
                    memset(&src_info,0,sizeof(rga_info_t));
                    src_info.fd = mpp_buffer_get_fd(mpp_frame_get_buffer(*srcFrm));
                    src_info.virAddr = NULL;
                    src_info.phyAddr = NULL;
                    src_info.rotation = HAL_TRANSFORM_ROT_270;
                    rga_set_rect(&src_info.rect,0,0,width,height,hor_stride,ver_stride,RK_FORMAT_YCbCr_420_SP);//RK_FORMAT_YCrCb_420_P//RK_FORMAT_YCbCr_420_P
                    // rga_set_rect(&dst_info.rect,0,0,RGA_ALIGN(width,16),RGA_ALIGN(height,16),hor_stride,ver_stride,RK_FORMAT_YCbCr_420_SP);
                    rga_set_rect(&dst_info.rect,0,0,RGA_ALIGN(height,16),RGA_ALIGN(width,16),ver_stride,hor_stride,RK_FORMAT_YCbCr_420_SP);
                    c_RkRgaBlit(&src_info,&dst_info,NULL);
                    //printf("get a frame \n");
                    mpp_frame_deinit(srcFrm);
                    *srcFrm = NULL;
               }
          }
       }
        usleep(10000);
    }
    return NULL;
}
void MyrkDecode::sendStreamThread()
{
    int ret = 0;
    decodeParamer *decodeP = (decodeParamer*)malloc(sizeof(decodeParamer));
    decodeP->mApi = mApi;
    decodeP->mCtx = &mCtx;
    decodeP->packet = &mPkt;
    ret = pthread_create(&mpth1, NULL, sendStreamToDecode, decodeP);
    if(ret < 0)
        printf("pthread_create mpth1 error \n");
    pthread_detach(mpth1);
}
void MyrkDecode:: getFrameThread()
{
    int ret = 0;
    decodeParamer *decodeP = (decodeParamer*)malloc(sizeof(decodeParamer));
    decodeP->mApi = mApi;
    decodeP->mCtx = &mCtx;
    decodeP->packet = &mPkt;
    decodeP->srcFrm = &msrcFrm;
    decodeP->frameGrp = &mmFrmGrp;
    decodeP->dstFrm = &mdstFrm;
    ret = pthread_create(&mpth2, NULL, getFrameForDecode, decodeP);
    if(ret < 0)
        printf("pthread_create mpth1 error \n");
    pthread_detach(mpth2);
}

MppFrame* MyrkDecode::getDstFrame()
{
    return &mdstFrm;
}


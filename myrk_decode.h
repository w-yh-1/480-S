#ifndef INITDECODE_H
#define INITDECODE_H

#include "common.h"

class MyrkDecode
{
    public:
    MyrkDecode(MppCodingType type,int width,int height);
    ~MyrkDecode();
    int initDecode();
    static void *sendStreamToDecode(void *args);
    static void *getFrameForDecode(void *args);
    void sendStreamThread();
    void getFrameThread();
    MppFrame* getDstFrame();
    static MyrkDecode* getInstance();
    private:
    MppCodingType mDecodeType;
    char *mPktBuf;
    MppPacket mPkt;
    MppCtx mCtx;
    MppApi *mApi;
    MppFrame mdstFrm;
    MppFrame msrcFrm;
    MppBuffer mmBuffer;
    MppBufferGroup mmFrmGrp;
    int mWidth;
    int mHeight;
    pthread_t mpth1;//send stream
    pthread_t mpth2;//get frame
    pthread_t mpth3; //show frame
    static MyrkDecode* sInstance;
};
#endif // INITDECODE_H

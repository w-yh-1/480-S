#ifndef drm_display_H
#define drm_display_H
#include "common.h"
#include "drm_buf.h"
/* init and set drm */
class DrmDisplay
{
    public:
    DrmDisplay();
    ~DrmDisplay();
    int initDrm();
    int get_supported_format(struct sp_plane* plane, uint32_t* format);
    int initialize_screens();
    int initialize_plane();
    struct sp_bo* create_sp_bo(struct sp_dev* dev, uint32_t width, uint32_t height,
                               uint32_t depth, uint32_t bpp, uint32_t format, uint32_t flags);
    int add_fb_sp_bo(struct sp_bo* bo, uint32_t format);
    int map_sp_bo(struct sp_bo* bo);
    void free_sp_bo(struct sp_bo* bo);
    struct sp_plane* get_sp_plane(struct sp_dev *dev, struct sp_crtc *crtc);
    int is_supported_format(struct sp_plane* plane, uint32_t format);
    int drm_plane_set_property(drmModePlane * plane,const char *prop_name, uint64_t prop_value);
    void destroy_sp_dev(struct sp_dev* dev);
    void put_sp_plane(struct sp_plane *plane);
    struct sp_dev* getSpdev();
    sp_crtc* getCrtcOne();
    sp_crtc* getCrtcTwo();
    sp_plane** getOnePlane();
    sp_plane** getTwoPlane();
    sp_plane* getFirstPlane();
    sp_plane* getSecondPlane();

    //display
    static void *startShowVideo(void *args);
    static void *showFrameTask(void *arg);
    void showFrameThread();
    bool showFrameThread(DRM_Buffer*frame_drm_buf);
    
    static DrmDisplay* getInstance();
    sp_bo *createSpBoFromDrmBuf(DRM_Buffer *frame_drm_buf,uint32_t format);
    int Vsync();
    void freeSpBo(sp_bo *bo);
    void display(sp_bo *bo);
private:
    struct sp_dev* mdev;
    sp_plane **mPlanesOne;
    sp_plane **mPlanesTwo;
    sp_crtc *mCrtcOne;
    sp_crtc *mCrtcTwo;
    sp_plane *mPlaneFirst=NULL;
    sp_plane *mPlaneSeconde=NULL;
    static DrmDisplay* sInstance;
    pthread_t mpth3; //show frame

    private:
    DRM_Buffer* mFrameDrmBuf=NULL;
};
#endif // drm_display_H

#ifndef _COMMON_H_
#define _COMMON_H_
extern "C"{
#include <pthread.h>
#include <drm/drm.h>
#include <drm/drm_fourcc.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include "rk_mpi.h"
#include "rk_type.h"
#include "drmrga.h"
int  c_RkRgaInit();
void c_RkRgaDeInit();
void c_RkRgaGetContext(void **ctx);
int  c_RkRgaBlit(rga_info_t *src, rga_info_t *dst, rga_info_t *src1);
int  c_RkRgaColorFill(rga_info_t *dst);
int  c_RkRgaFlush();

}
#define READ_SIZE   1024*50
#define PKT_SIZE    1024*100
#define RKXIMAGE_COLOR_KEY 0x010203
#define RK_COLOR_KEY_EN (1UL << 31)
#define CODEC_ALIGN(x, a)   (((x)+(a)-1)&~((a)-1))
#define RGA_ALIGN(x, a)   (((x)+(a)-1)&~((a)-1))

struct sp_bo {
    struct sp_dev *dev;

    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t bpp;
    uint32_t format;
    uint32_t flags;

    uint32_t fb_id;
    uint32_t handle;
    void *map_addr;
    uint32_t pitch;
    uint32_t size;
};
struct sp_plane {
    struct sp_dev *dev;
    drmModePlanePtr plane;
    struct sp_bo *bo;
    int in_use;
    uint32_t format;

    /* Property ID's */
    uint32_t crtc_pid;
    uint32_t fb_pid;
    uint32_t zpos_pid;
    uint32_t crtc_x_pid;
    uint32_t crtc_y_pid;
    uint32_t crtc_w_pid;
    uint32_t crtc_h_pid;
    uint32_t src_x_pid;
    uint32_t src_y_pid;
    uint32_t src_w_pid;
    uint32_t src_h_pid;
};
struct sp_crtc {
    drmModeCrtcPtr crtc;
    int pipe;
    int num_planes;
    struct sp_bo *scanout;
};
struct sp_dev {
    int fd;
    int num_connectors;
    drmModeConnectorPtr *connectors;
    int num_encoders;
    drmModeEncoderPtr *encoders;
    int num_crtcs;
    struct sp_crtc *crtcs;
    int num_planes;
    struct sp_plane *planes;
};

struct displayParamer{
    struct sp_dev* display_dev;
    sp_crtc *display_crtc;
    sp_plane *display_plane;
    int frame_width, frame_height, frame_fd;
};
struct decodeParamer{
    MppPacket *packet;
    MppApi *mApi;
    MppCtx *mCtx;
    MppFrame *srcFrm;
    MppFrame *dstFrm;
    MppBufferGroup *frameGrp;
    struct sp_dev* mdev;
    sp_crtc *mCrtcOne;
    sp_crtc *mCrtcTwo;
    sp_plane *mPlaneFirst;
    sp_plane *mPlaneSeconde;
};
#endif // _COMMON_H_

#include "drm_buf.h"
#include "RgaUtils.h"
uint get_bpp_by_format(int format)
{
    uint bpp;
    switch (format)
    {
    case RK_FORMAT_YCbCr_420_SP:
        bpp = 32;
        break;
    case RK_FORMAT_RGB_888:
    case RK_FORMAT_BGR_888:
        bpp = 24;
        break;
    case RK_FORMAT_YCbCr_422_SP:
    case RK_FORMAT_YUYV_422:
    case RK_FORMAT_BPP2:
        bpp = 16;
        break;
    case RK_FORMAT_RGBA_8888:
    case RK_FORMAT_BGRA_8888:
    case RK_FORMAT_ARGB_8888:
    case RK_FORMAT_ABGR_8888:
        bpp = 32;
        break;
    case RK_FORMAT_BPP8:
        bpp = 8;
        break;
    }
    return bpp;
}
int get_drm_buffer(DRM_Buffer *buf, uint W, uint H, int rk_fmt)
{
    struct drm_mode_create_dumb arg;
    struct drm_mode_map_dumb arg1;
    int handle, size, pitch;
    int ret;
    int drm_fd;
    int dma_fd;
    drm_fd = open("/dev/dri/card0", O_RDWR, 0);
    if (drm_fd < 0)
    {
        printf("failed to open card0\n");
        return -1;
    }
    buf->drm_fd = drm_fd;
    memset(&arg, 0, sizeof(arg));
    memset(&arg1, 0, sizeof(arg1));
    arg.bpp = get_bpp_from_format(rk_fmt)*8;
    // arg.bpp = get_bpp_by_format(rk_fmt);
    arg.width = W;
    arg.height = H;

    ret = drmIoctl(drm_fd, DRM_IOCTL_MODE_CREATE_DUMB, &arg);
    if (ret)
    {
        fprintf(stderr, "failed to create dumb buffer: %s\n", strerror(errno));
        return ret;
    }
    buf->size = arg.size;
    buf->pitch = arg.pitch;
    printf("dumb buffer size %d pitch %d bpp %d\n", arg.size, arg.pitch, arg.bpp);
    buf->drmBuf_handle = arg.handle;
    arg1.handle = arg.handle;
    ret = drmIoctl(drm_fd, DRM_IOCTL_MODE_MAP_DUMB, &arg1);
    if (ret)
        return ret;
    buf->ptr = mmap(NULL, arg.size, PROT_READ | PROT_WRITE, MAP_SHARED, drm_fd, arg1.offset);

    ret = drmPrimeHandleToFD(drm_fd, arg.handle, 0, &dma_fd); // close(dma_fd)减引用计数
    if (ret)
    {
        fprintf(stderr, "failed to export dumb buffer: %s\n", strerror(errno));
        return ret;
    }

    buf->dma_fd = dma_fd;
    buf->rga_handle = importbuffer_fd(dma_fd, arg.size);
    buf->rga_buf = wrapbuffer_handle(buf->rga_handle, arg.width, arg.height, rk_fmt);

    buf->width = arg.width;
    buf->height = arg.height;
    buf->bpp = arg.bpp;
    return 0;
}
void drm_release_buffer(DRM_Buffer *buf)
{
    struct drm_mode_destroy_dumb arg;
    int ret;
    memset(&arg, 0, sizeof(arg));
    if (buf->rga_handle)
        releasebuffer_handle(buf->rga_handle);
    else
        return;
    munmap(buf->ptr, buf->size);
    if (buf->dma_fd)
        close(buf->dma_fd);

    arg.handle = buf->drmBuf_handle;
    ret = drmIoctl(buf->drm_fd, DRM_IOCTL_MODE_DESTROY_DUMB, &arg);
    if (ret)
        fprintf(stderr, "failed to destroy dumb buffer: %s\n", strerror(errno));
}

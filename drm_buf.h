#ifndef DRM_BUF_H
#define DRM_BUF_H

// #include <drm.h>
// #include <drm_fourcc.h>
#include <xf86drm.h>
#include <unistd.h>
#include <fcntl.h>
#include <im2d.hpp>
#include <rga.h>
#include <iostream>
#include <string.h>
#include <sys/mman.h>

typedef struct DRM_Buffer_t
{
    uint drm_fd;
    uint drmBuf_handle;
    uint dma_fd;
    void *ptr;
    uint width;
    uint height;
    uint bpp;
    uint pitch;
    size_t size;
    rga_buffer_handle_t rga_handle;
    rga_buffer_t rga_buf;
} DRM_Buffer;

int get_drm_buffer(DRM_Buffer *buf, uint W, uint H, int rk_fmt);
void release_drm_buffer(DRM_Buffer *buf);



#endif
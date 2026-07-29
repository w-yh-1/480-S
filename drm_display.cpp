extern "C"
{
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
}

#include "drm_display.h"
#include "drm_mode.h"
#include "drm_buf.h"
DrmDisplay *DrmDisplay::sInstance = NULL;
DrmDisplay::DrmDisplay()
{
}
DrmDisplay::~DrmDisplay()
{
}

/**
 * Initializes the DRM device.
 *
 * This method initializes the DRM device by opening the device file and
 * retrieving the device's capabilities. It also allocates memory for the
 * device's connectors, encoders, and planes.
 *
 * @return 0 on success, -1 on failure
 */
int DrmDisplay::initDrm()
{
    int fd = 0, ret = 0, i = 0, j = 0;
    drmModeRes *drm_resources = NULL;
    drmModePlaneRes *plane_res = NULL;

    /* Open the DRM device file */
    fd = open("/dev/dri/card0", O_RDWR | O_CLOEXEC);
    if (fd < 0)
    {
        printf("failed to open card0\n");
        return -1;
    }

    /* Allocate and initialize the mdev structure */
    mdev = (struct sp_dev *)calloc(1, sizeof(*mdev));
    if (!mdev)
    {
        printf("calloc dev error\n");
        return -1;
    }
    mdev->fd = fd;

    /* Set DRM client capabilities */
    ret = drmSetClientCap(mdev->fd, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1);
    if (ret)
    {
        printf("drmSetClientCap error\n");
        goto err;
    }
    ret = drmSetClientCap(mdev->fd, DRM_CLIENT_CAP_ATOMIC, 1);
    if (ret)
    {
        printf("drmSetClientCap error\n");
        goto err;
    }

    /* Get DRM resources */
    drm_resources = drmModeGetResources(mdev->fd);
    if (!drm_resources)
    {
        printf("drmModeGetResources error\n");
        goto err;
    }
    mdev->num_connectors = drm_resources->count_connectors;
    mdev->connectors = (drmModeConnectorPtr *)calloc(mdev->num_connectors, sizeof(*mdev->connectors));
    if (!mdev->connectors)
    {
        printf("calloc error\n");
        goto err;
    }

    /* Retrieve connectors */
    for (i = 0; i < mdev->num_connectors; i++)
    {
        mdev->connectors[i] = drmModeGetConnector(mdev->fd,
                                                  drm_resources->connectors[i]);
        if (!mdev->connectors[i])
        {
            printf("drmModeGetConnector error %d\n", i);
            goto err;
        }
    }

    // Allocate memory for the device's encoders
    mdev->num_encoders = drm_resources->count_encoders;
    mdev->encoders = (drmModeEncoderPtr *)calloc(mdev->num_encoders, sizeof(*mdev->encoders));
    if (!mdev->encoders)
    {
        printf("calloc error\n");
        goto err;
    }

    // Retrieve the device's encoders
    for (i = 0; i < mdev->num_encoders; i++)
    {
        mdev->encoders[i] = drmModeGetEncoder(mdev->fd, drm_resources->encoders[i]);
        if (!mdev->encoders[i])
        {
            printf("drmModeGetEncoder error %d\n", i);
            goto err;
        }
    }

    // Allocate memory for the device's CRTCs
    mdev->num_crtcs = drm_resources->count_crtcs;
    mdev->crtcs = (struct sp_crtc *)calloc(mdev->num_crtcs, sizeof(struct sp_crtc));
    if (!mdev->crtcs)
    {
        printf("calloc error\n");
        goto err;
    }

    // Retrieve the device's CRTCs
    for (i = 0; i < mdev->num_crtcs; i++)
    {
        mdev->crtcs[i].crtc = drmModeGetCrtc(mdev->fd, drm_resources->crtcs[i]);
        if (!mdev->crtcs[i].crtc)
        {
            printf("drmModeGetCrtc error %d\n", i);
            goto err;
        }
        mdev->crtcs[i].scanout = NULL;
        mdev->crtcs[i].pipe = i;
        mdev->crtcs[i].num_planes = 0;
    }

    // Allocate memory for the device's planes
    plane_res = drmModeGetPlaneResources(mdev->fd);
    if (!plane_res)
    {
        printf("drmModeGetPlaneResources error\n");
        goto err;
    }
    mdev->num_planes = plane_res->count_planes;
    mdev->planes = (struct sp_plane *)calloc(mdev->num_planes, sizeof(struct sp_plane));

    // Retrieve the device's planes
    for (i = 0; i < mdev->num_planes; i++)
    {
        drmModeObjectPropertiesPtr props;
        struct sp_plane *plane = &mdev->planes[i];

        plane->dev = mdev;
        plane->plane = drmModeGetPlane(mdev->fd, plane_res->planes[i]);
        if (!plane->plane)
        {
            printf("drmModeGetPlane error %d\n", i);
            goto err;
        }
        plane->bo = NULL;
        plane->in_use = 0;

        ret = get_supported_format(plane, &plane->format);
        if (ret)
        {
            printf("failed to get supported format: %d\n", ret);
            goto err;
        }

        for (j = 0; j < mdev->num_crtcs; j++)
        {
            if (plane->plane->possible_crtcs & (1 << j))
                mdev->crtcs[j].num_planes++;
        }
    }

    // Free resources
    if (plane_res)
        drmModeFreePlaneResources(plane_res);
    if (drm_resources)
        drmModeFreeResources(drm_resources);

    // Initialize the screens
    // ret = initialize_screens();
    // if (ret)
    //     return -1;

    // Initialize the planes
    ret = initialize_plane();
    printf("init drm ok \n");
    return 0;
err:
    if (plane_res)
        drmModeFreePlaneResources(plane_res);
    if (drm_resources)
        drmModeFreeResources(drm_resources);
    destroy_sp_dev(mdev);
    return -1;
}

int DrmDisplay::get_supported_format(struct sp_plane *plane, uint32_t *format)
{
    uint32_t i;
    for (i = 0; i < plane->plane->count_formats; i++)
    {
        if (plane->plane->formats[i] == DRM_FORMAT_XRGB8888 ||
            plane->plane->formats[i] == DRM_FORMAT_ARGB8888 ||
            plane->plane->formats[i] == DRM_FORMAT_RGBA8888 ||
            plane->plane->formats[i] == DRM_FORMAT_BGR888)
        {
            *format = plane->plane->formats[i];
            return 0;
        }
    }
    printf("No suitable formats found!\n");
    return -ENOENT;
}
int DrmDisplay::initialize_screens()
{
    int ret, i, j;
    if (mdev == NULL)
        return -1;
    for (i = 0; i < mdev->num_connectors; i++)
    {
        drmModeConnectorPtr p_connector = mdev->connectors[i];
        drmModeModeInfoPtr p_mode_info = NULL;
        drmModeEncoderPtr p_encoder = NULL;
        struct sp_crtc *p_crtc = NULL;

        if (p_connector->connection != DRM_MODE_CONNECTED)
            continue;

        if (!p_connector->count_modes)
        {
            printf("connector has no modes, skipping\n");
            continue;
        }

        /* Take the first unless there's a preferred mode */
        p_mode_info = &p_connector->modes[0];
        for (j = 0; j < p_connector->count_modes; j++)
        {
            drmModeModeInfoPtr tmp_mode_info = &p_connector->modes[j];

            if (!(tmp_mode_info->type & DRM_MODE_TYPE_PREFERRED))
                continue;

            p_mode_info = tmp_mode_info;
            break;
        }

        if (!p_connector->encoder_id)
        {
            /*
             * if default drm encoder not attached connector, just
             * select the first one.
             */
            if (mdev->num_encoders)
            {
                p_encoder = mdev->encoders[0];
                p_connector->encoder_id = p_encoder->encoder_id;
            }
            else
            {
                printf("no encoder attached to the connector\n");
                continue;
            }
        }

        for (j = 0; j < mdev->num_encoders; j++)
        {
            p_encoder = mdev->encoders[j];
            if (p_encoder->encoder_id == p_connector->encoder_id)
                break;
        }
        if (j == mdev->num_encoders)
        {
            printf("could not find encoder for the connector\n");
            continue;
        }

        if (!p_encoder->crtc_id)
        {
            /*
             * if default drm crtc not attached encoder, just
             * select the first one.
             */
            if (mdev->num_crtcs)
            {
                p_crtc = &mdev->crtcs[j];
                p_encoder->crtc_id = p_crtc->crtc->crtc_id;
            }
            else
            {
                printf("no crtc attached to the encoder\n");
                continue;
            }
        }

        for (j = 0; j < mdev->num_crtcs; j++)
        {
            p_crtc = &mdev->crtcs[j];

            if (p_crtc->crtc->crtc_id == p_encoder->crtc_id)
                break;
        }
        if (j == mdev->num_crtcs)
        {
            printf("could not find crtc for the encoder\n");
            continue;
        }
        if (p_crtc->scanout)
        {
            printf("crtc already in use\n");
            continue;
        }

        /* XXX: Hardcoding the format here... :| */
        p_crtc->scanout = create_sp_bo(mdev, p_mode_info->hdisplay, p_mode_info->vdisplay,
                                       24, 32, DRM_FORMAT_XRGB8888, 0);
        printf("create scanout bo %dx%d\n", p_mode_info->hdisplay, p_mode_info->vdisplay);
        if (!p_crtc->scanout)
        {
            printf("failed to create new scanout bo\n");
            continue;
        }

        // fill_bo(p_crtc->scanout, 0x0, 0x0, 0x0, 0x0);

        ret = drmModeSetCrtc(mdev->fd, p_crtc->crtc->crtc_id,
                             p_crtc->scanout->fb_id, 0, 0, &p_connector->connector_id,
                             1, p_mode_info);
        if (ret)
        {
            printf("failed to set crtc mode ret=%d\n", ret);
            continue;
        }

        p_crtc->crtc = drmModeGetCrtc(mdev->fd, p_crtc->crtc->crtc_id);
        /*
         * Todo:
         * I don't know why crtc mode is empty, just copy PREFERRED mode
         * for it.
         */
        memcpy(&p_crtc->crtc->mode, p_mode_info, sizeof(*p_mode_info));
        // mCrtcOne = p_crtc;
    }
    return 0;
}
struct sp_bo *DrmDisplay::create_sp_bo(struct sp_dev *dev, uint32_t width, uint32_t height,
                                       uint32_t depth, uint32_t bpp, uint32_t format, uint32_t flags)
{
    int ret;
    struct drm_mode_create_dumb cd;
    struct sp_bo *bo;
    memset(&cd, 0, sizeof(cd));
    bo = (struct sp_bo *)calloc(1, sizeof(*bo));
    if (!bo)
        return NULL;

    cd.height = height;
    cd.width = width;
    cd.bpp = bpp;
    cd.flags = flags;
    ret = drmIoctl(dev->fd, DRM_IOCTL_MODE_CREATE_DUMB, &cd);
    if (ret)
    {
        printf("failed to create sp_bo %d\n", ret);
        goto err;
    }
    bo->dev = dev;
    bo->width = width;
    bo->height = height;
    bo->depth = depth;
    bo->bpp = bpp;
    bo->format = format;
    bo->flags = flags;

    bo->handle = cd.handle;
    bo->pitch = cd.pitch;
    bo->size = cd.size;
    ret = add_fb_sp_bo(bo, format);
    if (ret)
    {
        printf("failed to add fb ret=%d\n", ret);
        goto err;
    }
    ret = map_sp_bo(bo);
    if (ret)
    {
        printf("failed to map bo ret=%d\n", ret);
        goto err;
    }
    return bo;
err:
    free_sp_bo(bo);
    return NULL;
}
int DrmDisplay::add_fb_sp_bo(struct sp_bo *bo, uint32_t format)
{
    int ret;
    uint32_t handles[4], pitches[4], offsets[4];

    handles[0] = bo->handle;
    pitches[0] = bo->pitch;
    offsets[0] = 0;

    if (format == DRM_FORMAT_NV12 || format == DRM_FORMAT_NV16)
    {
        handles[1] = bo->handle;
        pitches[0] = bo->width;
        pitches[1] = bo->width;
        offsets[1] = bo->width * bo->height;
    }

    ret = drmModeAddFB2(bo->dev->fd, bo->width, bo->height,
                        format, handles, pitches, offsets,
                        &bo->fb_id, bo->flags);
    if (ret)
    {
        printf("failed to create fb ret=%d\n", ret);
        return ret;
    }
    return 0;
}
int DrmDisplay::map_sp_bo(struct sp_bo *bo)
{
    int ret;
    struct drm_mode_map_dumb md;

    if (bo->map_addr)
        return 0;

    md.handle = bo->handle;
    ret = drmIoctl(bo->dev->fd, DRM_IOCTL_MODE_MAP_DUMB, &md);
    if (ret)
    {
        printf("failed to map sp_bo ret=%d\n", ret);
        return ret;
    }

    bo->map_addr = mmap(NULL, bo->size, PROT_READ | PROT_WRITE, MAP_SHARED,
                        bo->dev->fd, md.offset);
    if (bo->map_addr == MAP_FAILED)
    {
        printf("failed to map bo ret=%d\n", -errno);
        return -errno;
    }
    return 0;
}
void DrmDisplay::free_sp_bo(struct sp_bo *bo)
{
    int ret;
    struct drm_mode_destroy_dumb dd;
    struct drm_mode_destroy_dumb cc;
    if (!bo)
        return;

    if (bo->map_addr)
        munmap(bo->map_addr, bo->size);

    if (bo->fb_id)
    {
        ret = drmModeRmFB(bo->dev->fd, bo->fb_id);
        if (ret)
            printf("Failed to rmfb ret=%d!\n", ret);
    }

    if (bo->handle)
    {
        dd.handle = bo->handle;
        ret = drmIoctl(bo->dev->fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dd);
        if (ret)
            printf("Failed to destroy buffer ret=%d\n", ret);
    }
    free(bo);
}
int DrmDisplay::initialize_plane()
{
    int i = 0;
    int countPlane0 = 0;
    if (mdev == NULL)
        return -1;
    mPlanesOne = (sp_plane **)calloc(mdev->num_planes, sizeof(*mPlanesOne));
    if (!mPlanesOne)
    {
        printf("calloc error\n");
        return -1;
    }
    mCrtcOne = &mdev->crtcs[3];
    for (i = 0; i < mCrtcOne->num_planes; i++)
    {
        mPlanesOne[i] = get_sp_plane(mdev, mCrtcOne);
        if (mPlanesOne[i] == NULL)
            continue;
        // get_primar_sp_plane(mDev, mCrtc);
        if (is_supported_format(mPlanesOne[i], DRM_FORMAT_BGR888)) // TODO:try to set rotation
        {
            if (/* drm_plane_set_property(mPlanesOne[i]->plane, "rotation", DRM_MODE_ROTATE_90) == 0 && */ countPlane0 == 0)
            {
                mPlaneFirst = mPlanesOne[i];
                printf("set mPlaneFirst success,index = %d,plane id = %d\n",i , mPlaneFirst->plane->plane_id);
                countPlane0++; 
                continue;
            }
            drm_plane_set_property(mPlanesOne[i]->plane, "colorkey", RKXIMAGE_COLOR_KEY | RK_COLOR_KEY_EN);
            // drm_plane_set_property(mPlanesOne[i]->plane, "zpos", 1);
        }
    }
    if (!mPlaneFirst)
    {
        printf("failed to get m_Plane0.\n");
        free(mPlanesOne);
        return -1;
    }
    return 0;
}
struct sp_plane *DrmDisplay::get_sp_plane(struct sp_dev *dev, struct sp_crtc *crtc)
{
    int i;

    for (i = 0; i < dev->num_planes; i++)
    {
        struct sp_plane *p = &dev->planes[i];
        if (p->in_use)
            continue;
        if (!(p->plane->possible_crtcs & (1 << crtc->pipe)))
            continue;
        p->in_use = 1;
        return p;
    }
    return NULL;
}
int DrmDisplay::is_supported_format(struct sp_plane *plane, uint32_t format)
{
    uint32_t i;
    for (i = 0; i < plane->plane->count_formats; i++)
    {
        if (plane->plane->formats[i] == format)
            return 1;
    }
    return 0;
}
int DrmDisplay::drm_plane_set_property(drmModePlane *plane, const char *prop_name, uint64_t prop_value)
{
    drmModeObjectPropertiesPtr props;
    drmModePropertyPtr prop;
    int i, ret = -1;

    props = drmModeObjectGetProperties(mdev->fd, plane->plane_id,
                                       DRM_MODE_OBJECT_PLANE);
    if (!props)
        return -1;

    for (i = 0; i < props->count_props; i++)
    {
        prop = drmModeGetProperty(mdev->fd, props->props[i]);
        if (prop && !strcmp(prop->name, prop_name))
        {
            ret = drmModeObjectSetProperty(mdev->fd, plane->plane_id,
                                           DRM_MODE_OBJECT_PLANE, props->props[i], prop_value);
        }
        drmModeFreeProperty(prop);
    }

    drmModeFreeObjectProperties(props);
    return ret < 0 ? -1 : 0;
}
void DrmDisplay::destroy_sp_dev(struct sp_dev *dev)
{
    int i;
    if (dev->planes)
    {
        for (i = 0; i < dev->num_planes; i++)
        {
            if (dev->planes[i].in_use)
                put_sp_plane(&dev->planes[i]);
            if (dev->planes[i].plane)
                drmModeFreePlane(dev->planes[i].plane);
            if (dev->planes[i].bo)
                free_sp_bo(dev->planes[i].bo);
        }
        free(dev->planes);
    }
    if (dev->crtcs)
    {
        for (i = 0; i < dev->num_crtcs; i++)
        {
            if (dev->crtcs[i].crtc)
                drmModeFreeCrtc(dev->crtcs[i].crtc);
            if (dev->crtcs[i].scanout)
                free_sp_bo(dev->crtcs[i].scanout);
        }
        free(dev->crtcs);
    }
    if (dev->encoders)
    {
        for (i = 0; i < dev->num_encoders; i++)
        {
            if (dev->encoders[i])
                drmModeFreeEncoder(dev->encoders[i]);
        }
        free(dev->encoders);
    }
    if (dev->connectors)
    {
        for (i = 0; i < dev->num_connectors; i++)
        {
            if (dev->connectors[i])
                drmModeFreeConnector(dev->connectors[i]);
        }
        free(dev->connectors);
    }

    close(dev->fd);
    free(dev);
}
void DrmDisplay::put_sp_plane(struct sp_plane *plane)
{
    drmModePlanePtr p;

    /* Get the latest plane information (most notably the crtc_id) */
    p = drmModeGetPlane(plane->dev->fd, plane->plane->plane_id);
    if (p)
        plane->plane = p;

    if (plane->plane->crtc_id)
        drmModeSetPlane(plane->dev->fd, plane->plane->plane_id,
                        plane->plane->crtc_id, 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0);

    if (plane->bo)
    {
        free_sp_bo(plane->bo);
        plane->bo = NULL;
    }
    plane->in_use = 0;
}
struct sp_dev *DrmDisplay::getSpdev()
{
    if (mdev)
        return mdev;
    else
        return NULL;
}
sp_crtc *DrmDisplay::getCrtcOne()
{
    return mCrtcOne;
}
sp_crtc *DrmDisplay::getCrtcTwo()
{
    return mCrtcTwo;
}
sp_plane **DrmDisplay::getOnePlane()
{
    return mPlanesOne;
}
sp_plane **DrmDisplay::getTwoPlane()
{
    return mPlanesTwo;
}
sp_plane *DrmDisplay::getFirstPlane()
{
    return mPlaneFirst;
}
sp_plane *DrmDisplay::getSecondPlane()
{
    return mPlaneSeconde;
}
DrmDisplay *DrmDisplay::getInstance()
{

    if (sInstance == NULL)
    {
        sInstance = new DrmDisplay();
    }
    return sInstance;
}
sp_bo *DrmDisplay::createSpBoFromDrmBuf(DRM_Buffer *frame_drm_buf, uint32_t format)
{
    sp_bo *bo = NULL;
    int width, height, ret;
    uint32_t handles[4]{0}, pitches[4]{0}, offsets[4]{0};

    bo = (struct sp_bo *)calloc(1, sizeof(struct sp_bo));
    if (!bo)
    {
        printf("failed to calloc bo.\n");
        return NULL;
    }

    // width = CODEC_ALIGN(frame_drm_buf->width, 16);
    // height = CODEC_ALIGN(frame_drm_buf->height, 16);
    width = frame_drm_buf->width;
    height = frame_drm_buf->height;
    drmPrimeFDToHandle(mdev->fd, frame_drm_buf->dma_fd, &bo->handle);
    bo->dev = mdev;
    bo->width = width;
    bo->height = height;
    bo->format = format;
    bo->pitch = frame_drm_buf->pitch;
    bo->size = frame_drm_buf->size;
    bo->flags = 0;

    handles[0] = bo->handle;
    pitches[0] = bo->pitch;
    offsets[0] = 0;
    switch (format)
    {
    case DRM_FORMAT_NV12:
    case DRM_FORMAT_NV16:
        bo->depth = 16;
        bo->bpp = 32;
        handles[1] = bo->handle;
        pitches[0] = bo->width;
        pitches[1] = bo->width;
        offsets[1] = bo->width * bo->height;
        break;
    case DRM_FORMAT_BGR888:
        bo->depth = 24;
        bo->bpp = 24;
        bo->format = DRM_FORMAT_RGB888;
        break;

    default:
        return NULL;
    }

    ret = drmModeAddFB2(mdev->fd, bo->width, bo->height,
                        bo->format, handles, pitches, offsets,
                        &bo->fb_id, bo->flags);
    if (ret != 0)
    {
        printf("failed to exec drmModeAddFb2.\n");
        return NULL;
    }

    return bo;
}
int DrmDisplay::Vsync()
{
    drmVBlank vbl;
    vbl.request.type = DRM_VBLANK_RELATIVE;
    vbl.request.sequence = 1; // 等待下一个 VBlank

    int ret = drmWaitVBlank(mdev->fd, &vbl);
    return ret;
}
void DrmDisplay::display(struct sp_bo *bo)
{
    int ret;
    sp_crtc *mcrtc0 = mCrtcOne;
    sp_plane *mplane0 = mPlaneFirst;
    if (!mcrtc0 || !mplane0)
    {
        printf("mcrtc0 or mplane0 is null.\n");
        return;
    }
    // ret = drmModeSetPlane(mdev->fd, mplane0->plane->plane_id,
    //                       mcrtc0->crtc->crtc_id, bo->fb_id, 0, 0, 0,
    //                       mcrtc0->crtc->mode.hdisplay,
    //                       mcrtc0->crtc->mode.vdisplay,
    //                       0, 0, bo->width<<16, bo->height<<16 );
    ret = drmModeSetPlane(mdev->fd, mplane0->plane->plane_id,
                          mcrtc0->crtc->crtc_id, bo->fb_id, 0, 0, 0,
                          mcrtc0->crtc->mode.hdisplay,
                          mcrtc0->crtc->mode.vdisplay,
                          0, 0, bo->width << 16, bo->height << 16);
    if (ret)
    {
        printf("failed to exec drmModeSetPlane. ret=%d\n", ret);
    }
}

void DrmDisplay::freeSpBo(struct sp_bo *bo)
{
    int ret;
    if (bo)
    {
        if (bo->fb_id)
        {
            ret = drmModeRmFB(mdev->fd, bo->fb_id);
            if (ret)
                printf("failed to exec drmModeRmFB.\n");
        }
        if (bo->handle)
        {
            struct drm_gem_close req = {
                .handle = bo->handle,
            };

            drmIoctl(bo->dev->fd, DRM_IOCTL_GEM_CLOSE, &req);
        }
        free(bo);
    }
}
// display 暂时无用
void *DrmDisplay::startShowVideo(void *args)
{
    int ret = 0;
    int err, width, height, frm_size, fd;
    uint32_t handles[4], pitches[4], offsets[4];
    decodeParamer *paramter = (decodeParamer *)(args);
    MppFrame *dstFrm = paramter->dstFrm;
    sp_dev *mdev = paramter->mdev;
    sp_crtc *mcrtc0 = paramter->mCrtcOne;
    sp_plane *mplane0 = paramter->mPlaneFirst;

    do
    {

        err = mpp_frame_get_errinfo(*dstFrm) | mpp_frame_get_discard(*dstFrm);
        if (err)
        {
            printf("get err info %d discard %d,go back.\n",
                   mpp_frame_get_errinfo(*dstFrm),
                   mpp_frame_get_discard(*dstFrm));
            continue;
        }

        width = mpp_frame_get_width(*dstFrm);

        height = mpp_frame_get_height(*dstFrm);

        width = CODEC_ALIGN(width, 16);
        height = CODEC_ALIGN(height, 16);
        frm_size = width * height * 3 / 2;

        sp_bo *bo = NULL;
        fd = mpp_buffer_get_fd(mpp_frame_get_buffer(*dstFrm));

        bo = (struct sp_bo *)calloc(1, sizeof(struct sp_bo));
        if (!bo)
        {
            printf("failed to calloc bo.\n");
            continue;
        }

        drmPrimeFDToHandle(mdev->fd, fd, &bo->handle);
        bo->dev = mdev;
        bo->width = width;
        bo->height = height;
        bo->depth = 16;
        bo->bpp = 32;
        bo->format = DRM_FORMAT_NV12; // DRM_FORMAT_NV12;
        bo->flags = 0;

        handles[0] = bo->handle;
        pitches[0] = width;
        offsets[0] = 0;
        handles[1] = bo->handle;
        pitches[1] = width;
        offsets[1] = width * height;

        ret = drmModeAddFB2(mdev->fd, bo->width, bo->height,
                            bo->format, handles, pitches, offsets,
                            &bo->fb_id, bo->flags);
        if (ret != 0)
        {
            printf("failed to exec drmModeAddFb2.\n");
            continue;
        }

        for (;;)
        {
            ret = drmModeSetPlane(mdev->fd, mplane0->plane->plane_id,
                                  mcrtc0->crtc->crtc_id, bo->fb_id, 0, 0, 0,
                                  mcrtc0->crtc->mode.hdisplay,
                                  mcrtc0->crtc->mode.vdisplay,
                                  0, 0, bo->width << 16, bo->height << 16);
            if (ret)
            {
                printf("failed to exec drmModeSetPlane.\n");
            }
            usleep(1000 * 30);
        }
        if (bo)
        {
            if (bo->fb_id)
            {
                ret = drmModeRmFB(mdev->fd, bo->fb_id);
                if (ret)
                    printf("failed to exec drmModeRmFB.\n");
            }
            if (bo->handle)
            {
                struct drm_gem_close req = {
                    .handle = bo->handle,
                };

                drmIoctl(bo->dev->fd, DRM_IOCTL_GEM_CLOSE, &req);
            }
            free(bo);
        }
    } while (0);
}
// void DrmDisplay::showFrameThread()
// {
//     int ret = 0;
//     MyrkDecode *decode = MyrkDecode::getInstance();
//     decodeParamer *decodeP = (decodeParamer *)malloc(sizeof(decodeParamer));

//     decodeP->dstFrm = decode->getDstFrame();
//     decodeP->mdev = mdev;
//     decodeP->mCrtcOne = mCrtcOne;
//     decodeP->mPlaneFirst = mPlaneFirst;
//     ret = pthread_create(&mpth3, NULL, startShowVideo, decodeP);
//     if (ret < 0)
//         printf("pthread_create mpth3 error \n");
//     pthread_detach(mpth3);
// }

// 暂时无用
bool DrmDisplay::showFrameThread(DRM_Buffer *frame_drm_buf)
{
    int ret = 0;
    mFrameDrmBuf = frame_drm_buf;
    ret = pthread_create(&mpth3, NULL, showFrameTask, this);
    if (ret < 0)
    {
        printf("pthread_create mpth3 error \n");
        return false;
    }
    pthread_detach(mpth3);
    return true;
}

// 暂时无用
void *DrmDisplay::showFrameTask(void *args)
{
    int ret = 0;
    int err, width, height, frm_size, fd;
    uint32_t handles[4], pitches[4], offsets[4];
    DrmDisplay *p = (DrmDisplay *)(args);
    DRM_Buffer *dstFrm = p->mFrameDrmBuf;
    sp_dev *mdev = p->mdev;
    sp_crtc *mcrtc = p->mCrtcOne;
    sp_plane *mplane = p->mPlaneFirst;

    width = dstFrm->width;
    height = dstFrm->height;

    width = CODEC_ALIGN(width, 16);
    height = CODEC_ALIGN(height, 16);
    frm_size = width * height * 3 / 2;

    fd = dstFrm->dma_fd;
    sp_bo *bo;
    for (;;)
    {
        bo = (struct sp_bo *)calloc(1, sizeof(struct sp_bo));
        if (!bo)
        {
            printf("failed to calloc bo.\n");
            continue;
        }

        // drmPrimeFDToHandle(mdev->fd, fd, &bo->handle);
        bo->handle = dstFrm->drmBuf_handle;
        bo->dev = mdev;
        bo->width = width;
        bo->height = height;
        bo->depth = 16;
        bo->bpp = 32;
        bo->format = DRM_FORMAT_NV12; // DRM_FORMAT_NV12;
        bo->flags = 0;

        handles[0] = bo->handle;
        pitches[0] = width;
        offsets[0] = 0;
        handles[1] = bo->handle;
        pitches[1] = width;
        offsets[1] = width * height;
        ret = drmModeAddFB2(mdev->fd, bo->width, bo->height,
                            bo->format, handles, pitches, offsets,
                            &bo->fb_id, bo->flags);
        if (ret != 0)
        {
            printf("failed to exec drmModeAddFb2.\n");
            continue;
        }
        ret = drmModeSetPlane(mdev->fd, mplane->plane->plane_id,
                              mcrtc->crtc->crtc_id, bo->fb_id, 0, 0, 0,
                              mcrtc->crtc->mode.hdisplay,
                              mcrtc->crtc->mode.vdisplay,
                              0, 0, bo->width << 16, bo->height << 16);
        if (ret)
        {
            printf("failed to exec drmModeSetPlane.\n");
        }
        if (mplane->bo)
        {
            if (mplane->bo->fb_id)
            {
                ret = drmModeRmFB(mdev->fd, mplane->bo->fb_id);
                if (ret)
                    printf("failed to exec drmModeRmFB.\n");
            }
            if (mplane->bo->handle)
            {
                struct drm_gem_close req = {
                    .handle = mplane->bo->handle,
                };

                drmIoctl(bo->dev->fd, DRM_IOCTL_GEM_CLOSE, &req);
            }
            free(mplane->bo);
        }
        mplane->bo = bo;
        usleep(1000 * 30);
    }
}

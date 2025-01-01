
#include <iostream>
#include <memory>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#include "logger.hpp"
#include "unique_handle.hpp"

using namespace zzwlib;
using namespace std;

static logger main_logger("drm_test", loglevel::log_verbose_level);

auto fd_deletor = [](int fd) {
    LOGI(main_logger, "close fd: %d", fd);
    close(fd);
};

auto drmModeResDeletor = [](drmModeRes *res) {
    if (res) {
        LOGI(main_logger, "free res");
        drmModeFreeResources(res);
    }
};

auto drmConnectorDeletor = [](drmModeConnector *connector) {
    if (connector) {
        LOGI(main_logger, "free connector");
        drmModeFreeConnector(connector);
    }
};

auto drmEncoderDeletor = [](drmModeEncoder *encoder) {
    if (encoder) {
        LOGI(main_logger, "free encoder");
        drmModeFreeEncoder(encoder);
    }
};

struct drm_mapped_buf {
    uint8_t *data = nullptr;
    uint32_t size = 0;
    drm_mapped_buf(uint8_t *data_, uint32_t size_) : data(data_), size(size_) {}
};

auto drmMappedBufDeletor = [](drm_mapped_buf *buf) {
    if (buf) {
        LOGI(main_logger, "unmap buffer");
        if (buf->data)
            munmap(buf->data, buf->size);
        else
            LOGI(main_logger, "try to unmap invalid buffer");
    }
};

struct drm_fb_dev {
    int drm_fd;
    int buf_id;
    // format info
    uint32_t width = 0;
    uint32_t height = 0;
    uint8_t  depth = 0;
    uint8_t  bpp = 0;
    uint32_t pitch = 0;
    // data area
    unique_ptr<drm_mapped_buf, decltype(drmMappedBufDeletor)> mapped_buf
             = unique_ptr<drm_mapped_buf, decltype(drmMappedBufDeletor)>(static_cast<drm_mapped_buf*>(nullptr), drmMappedBufDeletor);
    drm_fb_dev(int drm_fd_, int buf_id_) : drm_fd(drm_fd_), buf_id(buf_id_) {}
};

auto drmModeFBDeletor = [](drm_fb_dev *fb_ptr) {
    if (fb_ptr) {
        if (fb_ptr->mapped_buf)
            fb_ptr->mapped_buf.reset();
        LOGI(main_logger, "rm fb buffer");
        if (fb_ptr->buf_id && fb_ptr->drm_fd)
            drmModeRmFB(fb_ptr->drm_fd, fb_ptr->buf_id);
        else
            LOGI(main_logger, "try to rm invalid fb; drm fd: %d, buf id: %d", fb_ptr->drm_fd, fb_ptr->buf_id);
    }
};

unique_ptr<drmModeRes, decltype(drmModeResDeletor)> get_drm_resources(int drm_fd) {
    unique_ptr<drmModeRes, decltype(drmModeResDeletor)> res(drmModeGetResources(drm_fd), drmModeResDeletor);
    if (!res) {
        LOGE(main_logger, "can not get drm resources");
    }
    return res;
}

unique_ptr<drmModeConnector, decltype(drmConnectorDeletor)> select_drm_connector(int drm_fd, const drmModeRes &res) {
    for (int i = 0; i < res.count_connectors; i++) {
        unique_ptr<drmModeConnector, decltype(drmConnectorDeletor)> connector(drmModeGetConnector(drm_fd, res.connectors[i]), drmConnectorDeletor);
        if (!connector) {
            LOGE(main_logger, "can not get drm connector: %d", res.connectors[i]);
            continue;
        }
        if (connector->connection == DRM_MODE_CONNECTED && connector->count_modes > 0) {
            LOGI(main_logger, "find connector: %d", res.connectors[i]);
            return connector;
        }
    }
    unique_ptr<drmModeConnector, decltype(drmConnectorDeletor)> nullCon(nullptr, drmConnectorDeletor);
    return nullCon;
}

unique_ptr<drmModeEncoder, decltype(drmEncoderDeletor)> select_drm_encoder(int drm_fd,  const drmModeConnector &connector) {

    unique_ptr<drmModeEncoder, decltype(drmEncoderDeletor)> encoder(drmModeGetEncoder(drm_fd, connector.encoder_id), drmEncoderDeletor);
    if (!encoder) {
        LOGE(main_logger, "can not get drm encoder: %d", connector.encoder_id);
    }
    LOGI(main_logger, "find encoder %d with crtc id %d", encoder->encoder_id, encoder->crtc_id);

    return encoder;
}

unique_ptr<drm_fb_dev, decltype(drmModeFBDeletor)> create_drm_fb(int drm_fd, uint32_t width, uint32_t height) {
    uint8_t bpp = 32;
    uint8_t depth = 24;
    int ret = 0;
    unique_ptr<drm_fb_dev, decltype(drmModeFBDeletor)> invalid_fb_ptr(nullptr, drmModeFBDeletor);

    struct drm_mode_create_dumb create_dumb = {
        .height = height,
        .width = width,
        .bpp = bpp,
    };
    /* create dumb buffer */
    ret = drmIoctl(drm_fd, DRM_IOCTL_MODE_CREATE_DUMB, &create_dumb);
    if (ret) {
        LOGE(main_logger, "can not create dumb buffer");
        return invalid_fb_ptr;
    }

    auto dumb_deletor = [drm_fd](int handle) {
        struct drm_mode_destroy_dumb destroy_dumb = {
            .handle = handle,
        };
        drmIoctl(drm_fd, DRM_IOCTL_MODE_DESTROY_DUMB, &destroy_dumb);
    };
    // dump_handle will be released automatically
    unique_handle<decltype(dumb_deletor)> dumb_handle(create_dumb.handle, dumb_deletor);

    /* create drm fb */
    uint32_t buf_id;
    ret = drmModeAddFB(drm_fd, width, height, /*depth*/depth, /*bpp*/bpp,
            /*pitch*/create_dumb.pitch, /*bo_handle*/create_dumb.handle, /*buf_id*/&buf_id);
    if (ret) {
        LOGE(main_logger, "can not create drm fb");
        return invalid_fb_ptr;
    }
    // here fb_handle is valid. it will be released automatically
    unique_ptr<drm_fb_dev, decltype(drmModeFBDeletor)> fb_ptr(new drm_fb_dev(drm_fd,buf_id), drmModeFBDeletor);

    struct drm_mode_map_dumb map_dumb = {
       .handle = create_dumb.handle,
    };
    ret = drmIoctl(drm_fd, DRM_IOCTL_MODE_MAP_DUMB, &map_dumb);
    if (ret) {
        LOGE(main_logger, "can not map dumb buffer");
        return invalid_fb_ptr;
    }

    /* perform actual memory mapping */
    auto map = mmap(0, create_dumb.size, PROT_READ | PROT_WRITE, MAP_SHARED,
                drm_fd, map_dumb.offset);
    if (map == MAP_FAILED) {
        LOGE(main_logger, "cannot mmap dumb buffer, errno (%d)",errno);
        return invalid_fb_ptr;
    }
    // map will be released automatically
    unique_ptr<drm_mapped_buf, decltype(drmMappedBufDeletor)> mapped_buf(new drm_mapped_buf((uint8_t*)map, create_dumb.size), drmMappedBufDeletor);
    if (!mapped_buf) {
        LOGE(main_logger, "can not create mapped buffer");
        return invalid_fb_ptr;
    }
    fb_ptr->mapped_buf.reset(mapped_buf.release());

    /* clear the framebuffer to 0 */
    fb_ptr->width = width;
    fb_ptr->height = height;
    fb_ptr->depth = depth;
    fb_ptr->bpp = bpp;
    fb_ptr->pitch = create_dumb.pitch;
    // log fb info
    LOGI(main_logger, "create fb: width: %d, height: %d, depth: %d, bpp: %d, pitch: %d, buf id: %d",
            fb_ptr->width, fb_ptr->height, fb_ptr->depth, fb_ptr->bpp, fb_ptr->pitch, fb_ptr->buf_id);
    return fb_ptr;
}

int main(int argc, char **argv)
{
    // unique_xxx objects will be released automatically
    // and they will be released in reverse order of their creation
    // since they are on stack;

    const char *dev_name = "/dev/dri/card1";

    unique_handle<decltype(fd_deletor)> drm_handle(open(dev_name, O_RDWR | O_CLOEXEC), fd_deletor);

    if (!drm_handle) {
        LOGE(main_logger, "can not open %s", dev_name);
        return -1;
    }

    char *name = drmGetDeviceNameFromFd2(drm_handle.get());

    LOGI(main_logger, "get name: %s", name);

    uint64_t has_dumb;

    if (drmGetCap(drm_handle.get(), DRM_CAP_DUMB_BUFFER, &has_dumb) < 0) {
        LOGE(main_logger, "can not get DRM_CAP_DUMB_BUFFER");
        return -1;
    }

    auto res = get_drm_resources(drm_handle.get());
    if (!res) {
        LOGE(main_logger, "can not get drm resources");
        return -1;
    }

    auto connector = select_drm_connector(drm_handle.get(), *res);
    if (!connector) {
        LOGE(main_logger, "can not find connector");
        return -1;
    }

    auto encoder = select_drm_encoder(drm_handle.get(), *connector);
    if (!encoder) {
        LOGE(main_logger, "can not find encoder");
        return -1;
    }

    auto fb1 = create_drm_fb(drm_handle.get(), connector->modes[0].hdisplay, connector->modes[0].vdisplay);
    if (!fb1) {
        LOGE(main_logger, "can not create fb");
        return -1;
    }

    auto fb2 = create_drm_fb(drm_handle.get(), connector->modes[0].hdisplay, connector->modes[0].vdisplay);
    if (!fb2) {
        LOGE(main_logger, "can not create fb");
        return -1;
    }

    auto saved_crtc = drmModeGetCrtc(drm_handle.get(), encoder->crtc_id);
    // 0 - draw on fb1, then show fb1;
    // 1 - draw on fb2, then show fb2;
    for (int frame = 0; frame < 10; frame++) {
        int fb_idx = frame % 2;
        // draw a red screen
        for (int i = 0; i < fb1->height; i++) {
            for (int j = 0; j < fb1->width; j++) {
                uint8_t blue = 0xff;
                uint8_t green = 0xff;
                uint8_t red = 0xff;
                if (frame % 5 == 1) {
                    green = 0x00;
                    red = 0x00;
                } else if (frame % 5 == 2) {
                    blue = 0x00;
                    red = 0x00;
                } else if (frame % 5 == 3) {
                    blue = 0x00;
                    green = 0x00;
                } else if (frame % 5 == 4) {
                    blue = 0x00;
                    green = 0x00;
                    red = 0x00;
                }
                drm_fb_dev *fb = fb1.get();
                if (fb_idx == 1) {
                    fb = fb2.get();
                }
                fb->mapped_buf->data[i * fb->pitch + j * fb->bpp / 8 + 0] = blue; // blue
                fb->mapped_buf->data[i * fb->pitch + j * fb->bpp / 8 + 1] = green; // green
                fb->mapped_buf->data[i * fb->pitch + j * fb->bpp / 8 + 2] = red; // red
                fb->mapped_buf->data[i * fb->pitch + j * fb->bpp / 8 + 3] = 0xff; // alpha
            }
        }
        LOGI(main_logger, "set crtc");
        drm_fb_dev *fb = fb1.get();
        if (fb_idx == 1) {
            fb = fb2.get();
        }
        int ret = drmModeSetCrtc(drm_handle.get(), encoder->crtc_id, fb->buf_id, 0, 0, &connector->connector_id, 1, &connector->modes[0]);
        if (ret) {
            LOGE(main_logger, "can not set crtc");
            return -1;
        }
        sleep(3);
    }
    // restore saved crtc
    LOGI(main_logger, "restore crtc");
    drmModeSetCrtc(drm_handle.get(), saved_crtc->crtc_id, saved_crtc->buffer_id, saved_crtc->x, saved_crtc->y, &connector->connector_id, 1, &saved_crtc->mode);
    return 0;
}

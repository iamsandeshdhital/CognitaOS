/**
 * @file zerocopy.c
 * @brief Zero-copy memory fabric
 */

#include "cognita/hal.h"
#include <stdlib.h>
#include <string.h>

/* ========================================================================
 * Zero-Copy Fabric
 * ======================================================================== */

struct zerocopy_fabric {
    uint64_t    zf_region_count;
    struct zf_region {
        void       *zr_addr;
        size_t      zr_len;
        uint64_t    zr_handle;
        uint8_t     zr_crystal;     /* which crystal can access this */
    } zf_regions[1024];
};

int hal_zerocopy_init(void **fabric) {
    if (!fabric) return COG_EINVAL;

    struct zerocopy_fabric *f =
        (struct zerocopy_fabric *)calloc(1, sizeof(*f));
    if (!f) return COG_ENOMEM;

    *fabric = f;
    return COG_OK;
}

int hal_zerocopy_register(void *fabric, void *addr, size_t len,
                          uint64_t *handle)
{
    if (!fabric || !addr || len == 0 || !handle)
        return COG_EINVAL;

    struct zerocopy_fabric *f = (struct zerocopy_fabric *)fabric;
    if (f->zf_region_count >= 1024)
        return COG_EBUSY;

    uint64_t idx = f->zf_region_count++;
    f->zf_regions[idx].zr_addr = addr;
    f->zf_regions[idx].zr_len = len;
    f->zf_regions[idx].zr_handle = idx + 1;
    f->zf_regions[idx].zr_crystal = 0xFF;  /* all crystals */

    *handle = f->zf_regions[idx].zr_handle;
    return COG_OK;
}

int hal_zerocopy_transfer(void *fabric, uint64_t src_handle,
                          uint64_t dst_handle, size_t len)
{
    if (!fabric || src_handle == 0 || dst_handle == 0 || len == 0)
        return COG_EINVAL;

    /* In production, this programs the DMA engine for zero-copy transfer */
    /* The source and destination crystals can access each other's memory
     * directly through the CXL/photonic fabric without CPU involvement */

    return COG_OK;
}

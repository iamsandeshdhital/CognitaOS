/**
 * @file photonic.c
 * @brief Photonic interconnect driver
 */

#include "cognita/hal.h"
#include <stdlib.h>
#include <string.h>

/* ========================================================================
 * Photonic Interconnect
 * ======================================================================== */

int hal_photonic_init(struct photonic_interconnect **pi) {
    if (!pi) return COG_EINVAL;

    struct photonic_interconnect *p =
        (struct photonic_interconnect *)calloc(1, sizeof(*p));
    if (!p) return COG_ENOMEM;

    p->pi_channel_count = 64;
    p->pi_channels_per_fiber = 8;
    p->pi_bandwidth_per_channel = 100.0f;  /* Gbps */

    *pi = p;
    return COG_OK;
}

int hal_photonic_send(struct photonic_interconnect *pi, uint32_t channel,
                      const void *data, size_t len)
{
    if (!pi || !data || len == 0)
        return COG_EINVAL;

    if (channel >= pi->pi_channel_count)
        return COG_EINVAL;

    /* In production, this drives the silicon photonics hardware */
    return COG_OK;
}

int hal_photonic_recv(struct photonic_interconnect *pi, uint32_t channel,
                      void *data, size_t len)
{
    if (!pi || !data || len == 0)
        return COG_EINVAL;

    if (channel >= pi->pi_channel_count)
        return COG_EINVAL;

    /* In production, this reads from the photonic receiver */
    return COG_OK;
}

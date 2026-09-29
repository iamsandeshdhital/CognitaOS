/**
 * @file crystal.c
 * @brief Compute crystal abstraction
 */

#include "cognita/hal.h"
#include <stdlib.h>
#include <string.h>

/* ========================================================================
 * Crystal Discovery
 * ======================================================================== */

int hal_crystal_discover(struct compute_crystal **crystals,
                         uint32_t *count)
{
    if (!crystals || !count)
        return COG_EINVAL;

    struct compute_crystal *c = (struct compute_crystal *)calloc(
        4, sizeof(struct compute_crystal));
    if (!c) return COG_ENOMEM;

    c[0] = (struct compute_crystal){
        .cc_kind = COG_HW_CPU,
        .cc_units = 128,
        .cc_utilization = 0.0f,
        .cc_power_watts = 65.0f,
        .cc_temperature_c = 45.0f,
    };

    c[1] = (struct compute_crystal){
        .cc_kind = COG_HW_NPU,
        .cc_units = 32,
        .cc_utilization = 0.0f,
        .cc_power_watts = 300.0f,
        .cc_temperature_c = 55.0f,
    };

    c[2] = (struct compute_crystal){
        .cc_kind = COG_HW_GPU,
        .cc_units = 142,
        .cc_utilization = 0.0f,
        .cc_power_watts = 450.0f,
        .cc_temperature_c = 60.0f,
    };

    c[3] = (struct compute_crystal){
        .cc_kind = COG_HW_QPU,
        .cc_units = 1024,
        .cc_utilization = 0.0f,
        .cc_power_watts = 25.0f,
        .cc_temperature_c = 0.015f,
    };

    *crystals = c;
    *count = 4;

    return COG_OK;
}

/* ========================================================================
 * Crystal Allocate
 * ======================================================================== */

int hal_crystal_allocate(cog_hw_kind_t kind, uint64_t units,
                         struct compute_crystal **crystal)
{
    if (!crystal || units == 0)
        return COG_EINVAL;

    struct compute_crystal crystals[4];
    uint32_t count = 4;
    hal_crystal_discover(&crystals, &count);

    for (uint32_t i = 0; i < count; i++) {
        if (crystals[i].cc_kind == kind &&
            crystals[i].cc_units - crystals[i].cc_allocated_units >= units) {
            *crystal = (struct compute_crystal *)malloc(
                sizeof(struct compute_crystal));
            if (!*crystal) return COG_ENOMEM;
            memcpy(*crystal, &crystals[i], sizeof(struct compute_crystal));
            (*crystal)->cc_allocated_units = units;
            return COG_OK;
        }
    }

    return COG_E_HW_UNAVAILABLE;
}

/* ========================================================================
 * Crystal Submit / Sync
 * ======================================================================== */

int hal_crystal_submit(struct compute_crystal *crystal, const char *kernel,
                       const void *args, size_t args_len,
                       uint64_t *work_item_id)
{
    if (!crystal || !kernel || !work_item_id)
        return COG_EINVAL;

    static uint64_t next_work_id = 1;
    *work_item_id = next_work_id++;

    return COG_OK;
}

int hal_crystal_sync(struct compute_crystal *crystal, uint64_t work_item_id,
                     uint64_t timeout_ms, void **result, size_t *result_len)
{
    if (!crystal || !result || !result_len)
        return COG_EINVAL;

    *result = (void *)1;
    *result_len = 0;

    return COG_OK;
}

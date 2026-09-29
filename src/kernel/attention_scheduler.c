/**
 * @file attention_scheduler.c
 * @brief Single-pass attention scheduler for CognitaOS
 */

#include "cognita/kernel.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

/* ========================================================================
 * Softmax
 * ======================================================================== */

static void softmax(float *scores, uint32_t count) {
    float max = scores[0];
    for (uint32_t i = 1; i < count; i++) {
        if (scores[i] > max) max = scores[i];
    }

    float sum = 0.0f;
    for (uint32_t i = 0; i < count; i++) {
        scores[i] = expf(scores[i] - max);
        sum += scores[i];
    }

    float inv_sum = 1.0f / sum;
    for (uint32_t i = 0; i < count; i++) {
        scores[i] *= inv_sum;
    }
}

/* ========================================================================
 * Dot Product
 * ======================================================================== */

static float dot_product(const float *a, const float *b, uint32_t dim) {
    float sum = 0.0f;
    for (uint32_t i = 0; i < dim; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

/* ========================================================================
 * Attention Schedule Compute
 * ======================================================================== */

int attention_schedule_compute(const float *query,
                               const float *keys,
                               const float *values,
                               uint32_t num_entities,
                               struct attention_schedule_output *output)
{
    if (!query || !keys || !values || !output || num_entities == 0)
        return COG_EINVAL;

    if (num_entities > MAX_ENTITIES)
        return COG_EINVAL;

    uint32_t dim = MODEL_DIM;
    float scale = 1.0f / sqrtf((float)dim);

    /* Compute attention scores: Q * K^T */
    float scores[MAX_ENTITIES];
    for (uint32_t i = 0; i < num_entities; i++) {
        scores[i] = dot_product(query, keys + i * dim, dim) * scale;
    }

    /* Softmax to get attention weights */
    softmax(scores, num_entities);

    /* Compute output: attention_weights * V */
    memset(output, 0, sizeof(*output));
    output->allocation_count = num_entities;

    for (uint32_t i = 0; i < num_entities; i++) {
        output->allocations[i].ea_entity_id = i;
        output->allocations[i].ea_attention_weight = scores[i];

        /* Derive allocation decisions from value vectors */
        const float *v = values + i * dim;
        output->allocations[i].ea_cycles_allocated =
            (uint64_t)(v[0] * 1000000.0f) + 1000;
        output->allocations[i].ea_memory_pages =
            (uint32_t)(v[1] * 4096.0f) + 16;
        output->allocations[i].ea_power_state =
            (uint8_t)(v[2] * 4.0f);
        output->allocations[i].ea_core_affinity_mask =
            (uint8_t)(v[3] * 255.0f);
        output->allocations[i].ea_npu_partition =
            (uint8_t)(v[4] * 32.0f);
        output->allocations[i].ea_gpu_sm_count =
            (uint16_t)(v[5] * 142.0f);

        output->total_system_utility += scores[i] * v[6];
        output->predicted_energy_joules += scores[i] * v[7];
    }

    return COG_OK;
}

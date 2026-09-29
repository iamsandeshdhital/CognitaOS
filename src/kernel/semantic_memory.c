/**
 * @file semantic_memory.c
 * @brief Differentiable semantic memory manager
 */

#include "cognita/kernel.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ========================================================================
 * SWS Create / Destroy
 * ======================================================================== */

struct semantic_working_set_header {
    uint64_t    sws_magic;
    uint32_t    sws_version;
    uint32_t    sws_entry_count;
    uint32_t    sws_capacity;
    uint32_t    sws_max_dim;
    uint64_t    sws_entity_id;
    uint64_t    sws_epoch;
    uint8_t     sws_lock;
    uint8_t     sws_state;
};

#define SWS_MAGIC   0x5357534F5331ULL  /* "SWSOS1" */
#define SWS_VERSION 1

struct semantic_working_set *sws_create(uint64_t entity_id, uint32_t max_dim) {
    if (max_dim == 0 || max_dim > SWS_MAX_DIM)
        max_dim = SWS_DEFAULT_DIM;

    size_t size = sizeof(struct semantic_working_set_header) +
                  SWS_MAX_ENTRIES * sizeof(struct sws_entry);

    struct semantic_working_set_header *header =
        (struct semantic_working_set_header *)calloc(1, size);
    if (!header) return NULL;

    header->sws_magic = SWS_MAGIC;
    header->sws_version = SWS_VERSION;
    header->sws_capacity = SWS_MAX_ENTRIES;
    header->sws_max_dim = max_dim;
    header->sws_entity_id = entity_id;
    header->sws_epoch = 1;
    header->sws_state = 0;  /* ACTIVE */

    return (struct semantic_working_set *)header;
}

void sws_destroy(struct semantic_working_set *sws) {
    free(sws);
}

/* ========================================================================
 * SWS Save / Restore
 * ======================================================================== */

int sws_save(struct sched_entity *entity) {
    if (!entity || !entity->sws)
        return COG_EINVAL;

    struct semantic_working_set_header *header =
        (struct semantic_working_set_header *)entity->sws;

    if (header->sws_magic != SWS_MAGIC)
        return COG_EIO;

    header->sws_state = 1;  /* SAVING */
    header->sws_epoch++;
    header->sws_state = 0;  /* ACTIVE */

    return COG_OK;
}

int sws_restore(struct sched_entity *entity) {
    if (!entity || !entity->sws)
        return COG_EINVAL;

    struct semantic_working_set_header *header =
        (struct semantic_working_set_header *)entity->sws;

    if (header->sws_magic != SWS_MAGIC)
        return COG_EIO;

    if (header->sws_version != SWS_VERSION)
        return COG_EPROTO;

    header->sws_state = 2;  /* RESTORING */
    header->sws_state = 0;  /* ACTIVE */

    return COG_OK;
}

/* ========================================================================
 * Relevance Field
 * ======================================================================== */

float sws_compute_relevance(const float *page_embedding,
                            const float *context_embedding,
                            uint32_t dim) {
    if (!page_embedding || !context_embedding || dim == 0)
        return 0.0f;

    float dot = dot_product(page_embedding, context_embedding, dim);
    float page_norm = sqrtf(dot_product(page_embedding, page_embedding, dim));
    float context_norm = sqrtf(dot_product(context_embedding, context_embedding, dim));

    if (page_norm < 1e-8f || context_norm < 1e-8f)
        return 0.0f;

    float cosine = dot / (page_norm * context_norm);
    return 1.0f / (1.0f + expf(-cosine));  /* sigmoid */
}

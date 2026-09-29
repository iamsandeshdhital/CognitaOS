/**
 * @file test_semantic_memory.c
 * @brief Unit tests for semantic memory manager
 */

#include <cognita/kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

/* ========================================================================
 * Test: SWS Create / Destroy
 * ======================================================================== */

static void test_sws_create_destroy(void) {
    printf("  test_sws_create_destroy... ");

    struct semantic_working_set *sws = sws_create(1, SWS_DEFAULT_DIM);
    assert(sws != NULL);

    struct semantic_working_set_header *header =
        (struct semantic_working_set_header *)sws;
    assert(header->sws_magic == SWS_MAGIC);
    assert(header->sws_entity_id == 1);
    assert(header->sws_max_dim == SWS_DEFAULT_DIM);

    sws_destroy(sws);

    printf("PASS\n");
}

/* ========================================================================
 * Test: SWS Save / Restore
 * ======================================================================== */

static void test_sws_save_restore(void) {
    printf("  test_sws_save_restore... ");

    struct semantic_working_set *sws = sws_create(42, SWS_DEFAULT_DIM);
    assert(sws != NULL);

    struct sched_entity entity = { .sws = sws };

    assert(sws_save(&entity) == COG_OK);
    assert(sws_restore(&entity) == COG_OK);

    sws_destroy(sws);

    printf("PASS\n");
}

/* ========================================================================
 * Test: Relevance Computation
 * ======================================================================== */

static void test_relevance_computation(void) {
    printf("  test_relevance_computation... ");

    float page_emb[768];
    float context_emb[768];

    /* Identical embeddings should have high relevance */
    for (uint32_t i = 0; i < 768; i++) {
        page_emb[i] = 1.0f;
        context_emb[i] = 1.0f;
    }

    float rel = sws_compute_relevance(page_emb, context_emb, 768);
    assert(rel > 0.9f);

    /* Orthogonal embeddings should have low relevance */
    for (uint32_t i = 0; i < 768; i++) {
        page_emb[i] = (i < 384) ? 1.0f : 0.0f;
        context_emb[i] = (i < 384) ? 0.0f : 1.0f;
    }

    rel = sws_compute_relevance(page_emb, context_emb, 768);
    assert(rel < 0.1f);

    printf("PASS\n");
}

/* ========================================================================
 * Test: Invalid SWS
 * ======================================================================== */

static void test_invalid_sws(void) {
    printf("  test_invalid_sws... ");

    struct sched_entity entity = { .sws = NULL };
    assert(sws_save(&entity) == COG_EINVAL);
    assert(sws_restore(&entity) == COG_EINVAL);

    printf("PASS\n");
}

/* ========================================================================
 * Main
 * ======================================================================== */

int main(void) {
    printf("Semantic Memory Unit Tests\n");
    printf("===========================\n");

    test_sws_create_destroy();
    test_sws_save_restore();
    test_relevance_computation();
    test_invalid_sws();

    printf("\nAll tests passed!\n");
    return 0;
}

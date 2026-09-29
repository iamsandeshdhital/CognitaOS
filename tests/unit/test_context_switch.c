/**
 * @file test_context_switch.c
 * @brief Unit tests for context switching
 */

#include <cognita/kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* ========================================================================
 * Test: Basic Context Switch
 * ======================================================================== */

static void test_basic_switch(void) {
    printf("  test_basic_switch... ");

    struct sched_entity *a = (struct sched_entity *)calloc(1, sizeof(*a));
    struct sched_entity *b = (struct sched_entity *)calloc(1, sizeof(*a));

    a->sws = sws_create(1, SWS_DEFAULT_DIM);
    b->sws = sws_create(2, SWS_DEFAULT_DIM);

    int ret = context_switch(a, b, SWITCH_FLAG_HOT);
    assert(ret == COG_OK);

    ret = context_switch(b, a, SWITCH_FLAG_HOT);
    assert(ret == COG_OK);

    sws_destroy(a->sws);
    sws_destroy(b->sws);
    free(a);
    free(b);

    printf("PASS\n");
}

/* ========================================================================
 * Test: No-op Switch
 * ======================================================================== */

static void test_noop_switch(void) {
    printf("  test_noop_switch... ");

    struct sched_entity *a = (struct sched_entity *)calloc(1, sizeof(*a));
    a->sws = sws_create(1, SWS_DEFAULT_DIM);

    /* Switching to self without FORCE flag should be a no-op */
    int ret = context_switch(a, a, SWITCH_FLAG_HOT);
    assert(ret == COG_OK);

    /* Switching to self with FORCE flag should succeed */
    ret = context_switch(a, a, SWITCH_FLAG_FORCE);
    assert(ret == COG_OK);

    sws_destroy(a->sws);
    free(a);

    printf("PASS\n");
}

/* ========================================================================
 * Test: Invalid Entities
 * ======================================================================== */

static void test_invalid_entities(void) {
    printf("  test_invalid_entities... ");

    struct sched_entity *a = (struct sched_entity *)calloc(1, sizeof(*a));

    /* NULL prev */
    assert(context_switch(NULL, a, SWITCH_FLAG_HOT) == COG_EINVAL);

    /* NULL next */
    assert(context_switch(a, NULL, SWITCH_FLAG_HOT) == COG_EINVAL);

    free(a);

    printf("PASS\n");
}

/* ========================================================================
 * Main
 * ======================================================================== */

int main(void) {
    printf("Context Switch Unit Tests\n");
    printf("==========================\n");

    test_basic_switch();
    test_noop_switch();
    test_invalid_entities();

    printf("\nAll tests passed!\n");
    return 0;
}

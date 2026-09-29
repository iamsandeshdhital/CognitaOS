/**
 * @file test_attention_scheduler.c
 * @brief Unit tests for the attention scheduler
 */

#include <cognita/kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

/* ========================================================================
 * Test: Basic Scheduling
 * ======================================================================== */

static void test_basic_scheduling(void) {
    printf("  test_basic_scheduling... ");

    float query[MODEL_DIM];
    float keys[MAX_ENTITIES * MODEL_DIM];
    float values[MAX_ENTITIES * MODEL_DIM];

    for (uint32_t i = 0; i < MODEL_DIM; i++) {
        query[i] = 0.01f;
    }

    for (uint32_t i = 0; i < MAX_ENTITIES * MODEL_DIM; i++) {
        keys[i] = 0.01f;
        values[i] = 0.5f;
    }

    struct attention_schedule_output output;
    int ret = attention_schedule_compute(query, keys, values, 10, &output);

    assert(ret == COG_OK);
    assert(output.allocation_count == 10);
    assert(output.total_system_utility > 0.0f);
    assert(output.predicted_energy_joules > 0.0f);

    printf("PASS\n");
}

/* ========================================================================
 * Test: Single Entity
 * ======================================================================== */

static void test_single_entity(void) {
    printf("  test_single_entity... ");

    float query[MODEL_DIM];
    float keys[MODEL_DIM];
    float values[MODEL_DIM];

    for (uint32_t i = 0; i < MODEL_DIM; i++) {
        query[i] = 1.0f;
        keys[i] = 1.0f;
        values[i] = 0.5f;
    }

    struct attention_schedule_output output;
    int ret = attention_schedule_compute(query, keys, values, 1, &output);

    assert(ret == COG_OK);
    assert(output.allocation_count == 1);
    assert(output.allocations[0].ea_attention_weight > 0.99f);

    printf("PASS\n");
}

/* ========================================================================
 * Test: Invalid Input
 * ======================================================================== */

static void test_invalid_input(void) {
    printf("  test_invalid_input... ");

    struct attention_schedule_output output;

    /* NULL query */
    assert(attention_schedule_compute(NULL, NULL, NULL, 0, &output) == COG_EINVAL);

    /* Zero entities */
    float query[MODEL_DIM] = {0};
    float keys[MODEL_DIM] = {0};
    float values[MODEL_DIM] = {0};
    assert(attention_schedule_compute(query, keys, values, 0, &output) == COG_EINVAL);

    /* Too many entities */
    assert(attention_schedule_compute(query, keys, values, MAX_ENTITIES + 1, &output) == COG_EINVAL);

    printf("PASS\n");
}

/* ========================================================================
 * Test: Attention Weight Sum
 * ======================================================================== */

static void test_attention_weight_sum(void) {
    printf("  test_attention_weight_sum... ");

    float query[MODEL_DIM];
    float keys[MAX_ENTITIES * MODEL_DIM];
    float values[MAX_ENTITIES * MODEL_DIM];

    for (uint32_t i = 0; i < MODEL_DIM; i++) query[i] = 0.01f;
    for (uint32_t i = 0; i < MAX_ENTITIES * MODEL_DIM; i++) {
        keys[i] = 0.01f;
        values[i] = 0.5f;
    }

    struct attention_schedule_output output;
    attention_schedule_compute(query, keys, values, 100, &output);

    float sum = 0.0f;
    for (uint32_t i = 0; i < output.allocation_count; i++) {
        sum += output.allocations[i].ea_attention_weight;
    }

    /* Softmax weights should sum to 1.0 */
    assert(fabsf(sum - 1.0f) < 0.001f);

    printf("PASS\n");
}

/* ========================================================================
 * Main
 * ======================================================================== */

int main(void) {
    printf("Attention Scheduler Unit Tests\n");
    printf("==============================\n");

    test_basic_scheduling();
    test_single_entity();
    test_invalid_input();
    test_attention_weight_sum();

    printf("\nAll tests passed!\n");
    return 0;
}

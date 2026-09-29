/**
 * @file test_intent_compiler.c
 * @brief Unit tests for the intent compiler
 */

#include <cognita/compiler.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* ========================================================================
 * Test: Lexical Attention
 * ======================================================================== */

static void test_lexical_attention(void) {
    printf("  test_lexical_attention... ");

    float *embeddings = NULL;
    float *weights = NULL;
    uint32_t token_count = 0;

    int ret = cir_lexical_attention("hello world test", &embeddings, &weights, &token_count);
    assert(ret == COG_OK);
    assert(token_count == 3);
    assert(embeddings != NULL);
    assert(weights != NULL);

    /* Weights should sum to 1.0 */
    float sum = 0.0f;
    for (uint32_t i = 0; i < token_count; i++) sum += weights[i];
    assert(sum > 0.99f && sum < 1.01f);

    free(embeddings);
    free(weights);

    printf("PASS\n");
}

/* ========================================================================
 * Test: Goal Decomposition
 * ======================================================================== */

static void test_goal_decomposition(void) {
    printf("  test_goal_decomposition... ");

    struct cognitair_program program;
    int ret = cir_goal_decompose((void *)1, &program);
    assert(ret == COG_OK);
    assert(program.node_count > 0);
    assert(program.edge_count > 0);

    printf("PASS\n");
}

/* ========================================================================
 * Test: Verification
 * ======================================================================== */

static void test_verification(void) {
    printf("  test_verification... ");

    struct cognitair_program program;
    cir_goal_decompose((void *)1, &program);

    int ret = cir_verify(&program);
    assert(ret == COG_OK);

    printf("PASS\n");
}

/* ========================================================================
 * Test: Cycle Detection
 * ======================================================================== */

static void test_cycle_detection(void) {
    printf("  test_cycle_detection... ");

    struct cognitair_program program = {0};
    program.node_count = 2;
    program.edge_count = 1;
    program.nodes[0].cn_id = 0;
    program.nodes[1].cn_id = 1;
    program.edges[0].ce_from_node = 0;
    program.edges[0].ce_to_node = 0;  /* self-loop = cycle */

    int ret = cir_verify(&program);
    assert(ret == COG_E_CHAIN_CYCLE);

    printf("PASS\n");
}

/* ========================================================================
 * Main
 * ======================================================================== */

int main(void) {
    printf("Intent Compiler Unit Tests\n");
    printf("===========================\n");

    test_lexical_attention();
    test_goal_decomposition();
    test_verification();
    test_cycle_detection();

    printf("\nAll tests passed!\n");
    return 0;
}

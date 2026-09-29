/**
 * @file test_capability_chain.c
 * @brief Integration tests for capability chaining
 */

#include <cognita/runtime.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* ========================================================================
 * Test: Capability Lookup
 * ======================================================================== */

static void test_capability_lookup(void) {
    printf("  test_capability_lookup... ");

    struct capability *cap = capability_lookup("store.semantic_query");
    assert(cap != NULL);
    assert(strcmp(cap->cap_name, "store.semantic_query") == 0);

    cap = capability_lookup("nonexistent.capability");
    assert(cap == NULL);

    printf("PASS\n");
}

/* ========================================================================
 * Test: Capability Discovery
 * ======================================================================== */

static void test_capability_discovery(void) {
    printf("  test_capability_discovery... ");

    struct capability *results = NULL;
    size_t count = 0;

    int ret = cog_capability_discover("email send", &results, &count);
    assert(ret == COG_OK);
    assert(count > 0);
    assert(results != NULL);

    free(results);

    printf("PASS\n");
}

/* ========================================================================
 * Test: Capability Invoke
 * ======================================================================== */

static void test_capability_invoke(void) {
    printf("  test_capability_invoke... ");

    struct capability *cap = capability_lookup("email.send");
    assert(cap != NULL);

    void *result = NULL;
    size_t result_len = 0;

    int ret = capability_invoke(cap, NULL, 0, &result, &result_len);
    assert(ret == COG_OK);

    printf("PASS\n");
}

/* ========================================================================
 * Main
 * ======================================================================== */

int main(void) {
    printf("Capability Chain Integration Tests\n");
    printf("===================================\n");

    test_capability_lookup();
    test_capability_discovery();
    test_capability_invoke();

    printf("\nAll tests passed!\n");
    return 0;
}

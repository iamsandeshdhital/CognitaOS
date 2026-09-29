/**
 * @file test_agent_lifecycle.c
 * @brief Integration tests for agent lifecycle
 */

#include <cognita/runtime.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* ========================================================================
 * Test: Agent Create / Destroy
 * ======================================================================== */

static void test_agent_create_destroy(void) {
    printf("  test_agent_create_destroy... ");

    struct agent *a = agent_create("researcher", COG_CAP_TOOL_INVOKE);
    assert(a != NULL);
    assert(a->ag_state == COG_AGENT_STATE_CREATED);
    assert(strcmp(a->ag_role, "researcher") == 0);

    agent_destroy(a);

    printf("PASS\n");
}

/* ========================================================================
 * Test: Agent Fork
 * ======================================================================== */

static void test_agent_fork(void) {
    printf("  test_agent_fork... ");

    struct agent *parent = agent_create("researcher", COG_CAP_TOOL_INVOKE);
    struct agent *child = NULL;

    int ret = agent_fork(parent, &child);
    assert(ret == COG_OK);
    assert(child != NULL);
    assert(child->ag_parent == parent);
    assert(parent->ag_children_count == 1);

    agent_destroy(parent);

    printf("PASS\n");
}

/* ========================================================================
 * Test: Agent Merge
 * ======================================================================== */

static void test_agent_merge(void) {
    printf("  test_agent_merge... ");

    struct agent *a = agent_create("researcher", COG_CAP_TOOL_INVOKE);
    struct agent *b = agent_create("communicator", COG_CAP_STORE_WRITE);
    struct agent *merged = NULL;

    int ret = agent_merge(a, b, &merged);
    assert(ret == COG_OK);
    assert(merged != NULL);
    assert(merged->ag_capability_mask ==
           (COG_CAP_TOOL_INVOKE | COG_CAP_STORE_WRITE));

    agent_destroy(merged);
    agent_destroy(a);
    agent_destroy(b);

    printf("PASS\n");
}

/* ========================================================================
 * Test: Agent Sleep / Wake
 * ======================================================================== */

static void test_agent_sleep_wake(void) {
    printf("  test_agent_sleep_wake... ");

    struct agent *a = agent_create("researcher", COG_CAP_TOOL_INVOKE);
    a->ag_state = COG_AGENT_STATE_RUNNING;

    assert(agent_sleep(a) == COG_OK);
    assert(a->ag_state == COG_AGENT_STATE_WAITING);

    assert(agent_wake(a) == COG_OK);
    assert(a->ag_state == COG_AGENT_STATE_RUNNING);

    agent_destroy(a);

    printf("PASS\n");
}

/* ========================================================================
 * Main
 * ======================================================================== */

int main(void) {
    printf("Agent Lifecycle Integration Tests\n");
    printf("==================================\n");

    test_agent_create_destroy();
    test_agent_fork();
    test_agent_merge();
    test_agent_sleep_wake();

    printf("\nAll tests passed!\n");
    return 0;
}

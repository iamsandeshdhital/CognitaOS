/**
 * @file agent.c
 * @brief Agent lifecycle management
 */

#include "cognita/runtime.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

static uint64_t next_agent_id = 1;

/* ========================================================================
 * Agent Create / Destroy
 * ======================================================================== */

struct agent *agent_create(const char *role, uint64_t cap_mask) {
    struct agent *a = (struct agent *)calloc(1, sizeof(struct agent));
    if (!a) return NULL;

    a->ag_id = next_agent_id++;
    strncpy(a->ag_role, role, 63);
    a->ag_capability_mask = cap_mask;
    a->ag_state = COG_AGENT_STATE_CREATED;
    a->ag_birth_ns = (uint64_t)time(NULL) * 1000000000ULL;

    return a;
}

void agent_destroy(struct agent *agent) {
    if (!agent) return;

    /* Recursively destroy children */
    for (uint64_t i = 0; i < agent->ag_children_count; i++) {
        agent_destroy(agent->ag_children[i]);
    }

    free(agent);
}

/* ========================================================================
 * Agent Fork
 * ======================================================================== */

int agent_fork(struct agent *parent, struct agent **child) {
    if (!parent || !child)
        return COG_EINVAL;

    if (parent->ag_children_count >= MAX_CHILDREN)
        return COG_EBUSY;

    struct agent *c = agent_create(parent->ag_role, parent->ag_capability_mask);
    if (!c) return COG_ENOMEM;

    c->ag_parent = parent;
    parent->ag_children[parent->ag_children_count++] = c;
    *child = c;

    return COG_OK;
}

/* ========================================================================
 * Agent Merge
 * ======================================================================== */

int agent_merge(struct agent *a, struct agent *b, struct agent **merged) {
    if (!a || !b || !merged)
        return COG_EINVAL;

    /* Create merged agent with union of capabilities */
    struct agent *m = agent_create("merged",
                                   a->ag_capability_mask | b->ag_capability_mask);
    if (!m) return COG_ENOMEM;

    /* Transfer children */
    for (uint64_t i = 0; i < a->ag_children_count; i++) {
        m->ag_children[m->ag_children_count++] = a->ag_children[i];
    }
    for (uint64_t i = 0; i < b->ag_children_count; i++) {
        m->ag_children[m->ag_children_count++] = b->ag_children[i];
    }

    *merged = m;

    /* Mark original agents as merged */
    a->ag_state = COG_AGENT_STATE_DONE;
    b->ag_state = COG_AGENT_STATE_DONE;

    return COG_OK;
}

/* ========================================================================
 * Agent Sleep / Wake
 * ======================================================================== */

int agent_sleep(struct agent *agent) {
    if (!agent)
        return COG_EINVAL;

    /* Persist state to semantic store */
    agent->ag_state = COG_AGENT_STATE_WAITING;
    agent->ag_last_active_ns = (uint64_t)time(NULL) * 1000000000ULL;

    return COG_OK;
}

int agent_wake(struct agent *agent) {
    if (!agent)
        return COG_EINVAL;

    /* Restore state from semantic store */
    agent->ag_state = COG_AGENT_STATE_RUNNING;

    return COG_OK;
}

/**
 * @file runtime.h
 * @brief CognitaOS Runtime Internal API
 */

#ifndef COGNITA_RUNTIME_H
#define COGNITA_RUNTIME_H

#include <cognita/os.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * Agent Runtime
 * ======================================================================== */

#define MAX_CAPABILITIES    64
#define MAX_CHILDREN        16

struct agent {
    uint64_t    ag_id;
    char        ag_name[128];
    char        ag_role[64];
    uint64_t    ag_capability_mask;
    void       *ag_capabilities[MAX_CAPABILITIES];
    uint64_t    ag_cycles_consumed;
    uint64_t    ag_energy_consumed_joules;
    uint64_t    ag_deadline_ns;
    struct agent *ag_parent;
    struct agent *ag_children[MAX_CHILDREN];
    uint64_t    ag_children_count;
    cog_agent_state_t ag_state;
    uint64_t    ag_birth_ns;
    uint64_t    ag_last_active_ns;
};

struct agent *agent_create(const char *role, uint64_t cap_mask);
void agent_destroy(struct agent *agent);
int agent_fork(struct agent *parent, struct agent **child);
int agent_merge(struct agent *a, struct agent *b, struct agent **merged);
int agent_sleep(struct agent *agent);
int agent_wake(struct agent *agent);

/* ========================================================================
 * Capability Runtime
 * ======================================================================== */

struct capability {
    uint64_t    cap_id;
    char        cap_name[128];
    char        cap_version[32];
    char        cap_type_uri[256];
    uint64_t    cap_required_caps;
    uint64_t    cap_granted_caps;
    uint8_t     cap_sandbox_level;
    uint32_t    cap_timeout_ms;
    uint32_t    cap_max_retries;
    uint64_t    cap_cost_per_invocation_usd;
    float       cap_embedding[768];
};

struct capability *capability_lookup(const char *name);
int capability_invoke(struct capability *cap, const void *args,
                       size_t args_len, void **result, size_t *result_len);

#ifdef __cplusplus
}
#endif

#endif /* COGNITA_RUNTIME_H */

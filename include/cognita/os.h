/**
 * @file os.h
 * @brief CognitaOS Public API
 *
 * This is the primary user-space header for CognitaOS.
 * Include this file to access all CognitaOS functionality.
 */

#ifndef COGNITA_OS_H
#define COGNITA_OS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * Version and Magic
 * ======================================================================== */

#define COG_MAGIC           0x434F474E  /* "COGN" */
#define COG_API_VERSION     3
#define COG_VERSION_MAJOR   2
#define COG_VERSION_MINOR   0
#define COG_VERSION_PATCH   0

/* ========================================================================
 * Error Codes
 * ======================================================================== */

#define COG_OK                  0
#define COG_EINVAL              (-22)
#define COG_EACCES              (-13)
#define COG_ENOMEM              (-12)
#define COG_EBUSY               (-16)
#define COG_ENOENT              (-2)
#define COG_EEXIST              (-17)
#define COG_EAGAIN              (-11)
#define COG_ECANCELED           (-125)
#define COG_EDEADLK             (-35)
#define COG_EPROTO              (-71)

#define COG_E_SCHEMA_VIOLATION  (-200)
#define COG_E_CAP_DENIED        (-201)
#define COG_E_QUOTA_EXCEEDED    (-202)
#define COG_E_INTENT_REJECTED   (-203)
#define COG_E_AGENT_NOT_FOUND   (-204)
#define COG_E_TOOL_NOT_FOUND   (-205)
#define COG_E_STORE_CORRUPT     (-206)
#define COG_E_HW_UNAVAILABLE    (-207)
#define COG_E_CHAIN_CYCLE       (-208)
#define COG_E_PROVENANCE_FAIL   (-209)

/* ========================================================================
 * Capability Flags
 * ======================================================================== */

#define COG_CAP_INTENT_SUBMIT   (1ULL << 0)
#define COG_CAP_MEM_WRITE       (1ULL << 1)
#define COG_CAP_TOOL_INVOKE     (1ULL << 2)
#define COG_CAP_AGENT_SPAWN     (1ULL << 3)
#define COG_CAP_STORE_WRITE     (1ULL << 4)
#define COG_CAP_HW_SUBMIT       (1ULL << 5)
#define COG_CAP_AUDIT_READ      (1ULL << 6)

/* ========================================================================
 * Types
 * ======================================================================== */

typedef uint64_t cog_handle_t;
#define COG_HANDLE_INVALID ((cog_handle_t)0)

typedef enum {
    COG_PRIORITY_BACKGROUND = 0,
    COG_PRIORITY_NORMAL     = 1,
    COG_PRIORITY_HIGH       = 2,
    COG_PRIORITY_CRITICAL   = 3,
} cog_priority_t;

typedef enum {
    COG_MODALITY_TEXT       = 0,
    COG_MODALITY_IMAGE      = 1,
    COG_MODALITY_AUDIO      = 2,
    COG_MODALITY_VIDEO      = 3,
    COG_MODALITY_MULTIMODAL = 4,
} cog_modality_t;

typedef enum {
    COG_HW_CPU              = 0,
    COG_HW_GPU              = 1,
    COG_HW_NPU              = 2,
    COG_HW_FPGA             = 3,
    COG_HW_QPU              = 4,
} cog_hw_kind_t;

typedef enum {
    COG_AGENT_STATE_CREATED = 0,
    COG_AGENT_STATE_RUNNING = 1,
    COG_AGENT_STATE_WAITING = 2,
    COG_AGENT_STATE_DONE    = 3,
    COG_AGENT_STATE_FAILED  = 4,
    COG_AGENT_STATE_KILLED  = 5,
} cog_agent_state_t;

typedef enum {
    COG_EDGE_SEQUENTIAL     = 0,
    COG_EDGE_PARALLEL       = 1,
    COG_EDGE_CONDITIONAL    = 2,
    COG_EDGE_FALLBACK       = 3,
} cog_edge_kind_t;

/* ========================================================================
 * Structures
 * ======================================================================== */

struct cog_audit_receipt {
    uint64_t    receipt_id;
    uint64_t    intent_id;
    uint64_t    timestamp_ns;
    uint8_t     prev_hash[32];
    uint8_t     payload_hash[32];
    uint8_t     signature[64];
};

struct cog_data_ref {
    cog_modality_t      modality;
    uint64_t            data_handle;
    const char         *mime_type;
};

struct cog_intent_expr {
    char            ie_type_uri[256];
    uint8_t        *ie_body;
    size_t          ie_body_len;
    uint64_t        ie_parent_intent_id;
    uint64_t        ie_trace_id;
    uint64_t        ie_deadline_ns;
    uint64_t        ie_max_cost_usd;
    uint64_t        ie_required_caps;
    cog_modality_t  ie_modality;
    struct cog_data_ref *ie_attachments;
    size_t          ie_attachment_count;
};

struct cog_intent_result {
    int         ir_status;
    uint64_t    ir_intent_id;
    uint8_t    *ir_output;
    size_t      ir_output_len;
    uint64_t    ir_execution_time_us;
    uint64_t    ir_cost_usd;
    uint64_t    ir_energy_joules;
    struct cog_audit_receipt ir_receipt;
};

struct cog_memory_result {
    uint64_t    object_id;
    float       similarity;
    uint64_t    size_bytes;
    const char *type_uri;
    uint8_t    *data;
    size_t      data_len;
    struct cog_audit_receipt access_receipt;
};

/* ========================================================================
 * Intent API
 * ======================================================================== */

int cog_intent(const struct cog_intent_expr *expr,
               struct cog_intent_result *result);

/* ========================================================================
 * Capability API
 * ======================================================================== */

struct capability;
struct cog_chain_handle;

int cog_capability_discover(const char *semantic_query,
                            struct capability **results,
                            size_t *result_count);

int cog_capability_invoke(struct capability *cap,
                          const uint8_t *args, size_t args_len,
                          uint8_t **result, size_t *result_len);

int cog_capability_chain(struct capability **caps, size_t cap_count,
                         void *edges, size_t edge_count,
                         struct cog_chain_handle **chain_handle);

int cog_chain_execute(struct cog_chain_handle *chain,
                      struct cog_intent_result *result);

/* ========================================================================
 * Agent API
 * ======================================================================== */

struct cog_agent_handle;

int cog_agent_spawn(const char *role, uint64_t cap_mask,
                    struct cog_agent_handle **agent);

int cog_agent_send(struct cog_agent_handle *agent,
                   const char *message,
                   struct cog_intent_result *result);

int cog_agent_recv(struct cog_agent_handle *agent,
                   struct cog_intent_result *result);

int cog_agent_fork(struct cog_agent_handle *parent,
                   struct cog_agent_handle **child);

int cog_agent_merge(struct cog_agent_handle *agent_a,
                    struct cog_agent_handle *agent_b,
                    struct cog_agent_handle **merged);

int cog_agent_sleep(struct cog_agent_handle *agent);

int cog_agent_wake(struct cog_agent_handle *agent);

/* ========================================================================
 * Memory API
 * ======================================================================== */

int cog_memory_attend(const float *query_embedding, size_t dim,
                      struct cog_memory_result **results,
                      size_t *result_count);

int cog_memory_commit(const float *key_embedding, size_t dim,
                      const void *data, size_t data_len,
                      uint64_t *object_id);

int cog_memory_forget(uint64_t object_id);

int cog_memory_associate(uint64_t from_id, uint64_t to_id,
                         const char *relation_type, float weight);

/* ========================================================================
 * Hardware API
 * ======================================================================== */

struct cog_hw_handle;

int cog_hw_allocate(cog_hw_kind_t kind, uint64_t units,
                    struct cog_hw_handle **hw);

int cog_hw_submit(struct cog_hw_handle *hw, const char *kernel,
                  const void *args, size_t args_len,
                  uint64_t *work_item_id);

int cog_hw_sync(struct cog_hw_handle *hw, uint64_t work_item_id,
                uint64_t timeout_ms, void **result, size_t *result_len);

/* ========================================================================
 * Utility API
 * ======================================================================== */

const char *cog_strerror(int err);
uint64_t cog_version(void);
const char *cog_version_string(void);

#ifdef __cplusplus
}
#endif

#endif /* COGNITA_OS_H */

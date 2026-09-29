# CognitaOS API Reference

## Overview

The CognitaOS API is intentionally minimal. The entire user-space interface consists of a small set of functions organized into five categories:

| Category | Entry Point | Description |
|----------|-------------|-------------|
| **Intent** | `cog_intent()` | Express a desired outcome |
| **Capability** | `cog_capability_*()` | Discover and invoke capabilities |
| **Agent** | `cog_agent_*()` | Manage agent lifecycle |
| **Memory** | `cog_memory_*()` | Attention-based memory operations |
| **Hardware** | `cog_hw_*()` | Compute crystal management |

---

## Intent API

### `cog_intent`

The primary entry point. Express an intent; the system compiles, verifies, schedules, and executes it.

```c
int cog_intent(const struct cog_intent_expr *expr,
               struct cog_intent_result *result);
```

**Parameters:**
- `expr` — The intent expression (type, body, constraints)
- `result` — Output: execution result, timing, cost, energy

**Returns:** `COG_OK` on success, negative error code on failure.

**Example:**
```c
struct cog_intent_expr expr = {
    .ie_type_uri = "cognita/Summarize/v1",
    .ie_body = serialized_args,
    .ie_body_len = serialized_len,
    .ie_deadline_ns = 5000000000,
    .ie_max_cost_usd = 5000,
};

struct cog_intent_result result;
int ret = cog_intent(&expr, &result);
```

---

## Capability API

### `cog_capability_discover`

Discover capabilities by semantic query.

```c
int cog_capability_discover(const char *semantic_query,
                            struct capability **results,
                            size_t *result_count);
```

### `cog_capability_invoke`

Invoke a capability directly.

```c
int cog_capability_invoke(struct capability *cap,
                          const uint8_t *args, size_t args_len,
                          uint8_t **result, size_t *result_len);
```

### `cog_capability_chain`

Compose capabilities into a dataflow DAG.

```c
int cog_capability_chain(struct capability **caps, size_t cap_count,
                         struct cog_edge *edges, size_t edge_count,
                         struct cog_chain_handle **chain_handle);
```

### `cog_chain_execute`

Execute a capability chain.

```c
int cog_chain_execute(struct cog_chain_handle *chain,
                      struct cog_intent_result *result);
```

---

## Agent API

### `cog_agent_spawn`

Create a new agent.

```c
int cog_agent_spawn(const char *role, uint64_t cap_mask,
                    struct cog_agent_handle **agent);
```

### `cog_agent_send`

Send a message to an agent.

```c
int cog_agent_send(struct cog_agent_handle *agent,
                   const char *message,
                   struct cog_intent_result *result);
```

### `cog_agent_recv`

Receive from an agent (blocking).

```c
int cog_agent_recv(struct cog_agent_handle *agent,
                   struct cog_intent_result *result);
```

### `cog_agent_fork`

Fork an agent (split into two with shared memory).

```c
int cog_agent_fork(struct cog_agent_handle *parent,
                   struct cog_agent_handle **child);
```

### `cog_agent_merge`

Merge two agents into one.

```c
int cog_agent_merge(struct cog_agent_handle *agent_a,
                    struct cog_agent_handle *agent_b,
                    struct cog_agent_handle **merged);
```

### `cog_agent_sleep`

Persist agent state and release resources.

```c
int cog_agent_sleep(struct cog_agent_handle *agent);
```

### `cog_agent_wake`

Restore agent state and reallocate resources.

```c
int cog_agent_wake(struct cog_agent_handle *agent);
```

---

## Memory API

### `cog_memory_attend`

Read from memory by attention query.

```c
int cog_memory_attend(const float *query_embedding, size_t dim,
                      struct cog_memory_result **results,
                      size_t *result_count);
```

### `cog_memory_commit`

Write to memory.

```c
int cog_memory_commit(const float *key_embedding, size_t dim,
                      const void *data, size_t data_len,
                      uint64_t *object_id);
```

### `cog_memory_forget`

Evict from memory.

```c
int cog_memory_forget(uint64_t object_id);
```

### `cog_memory_associate`

Create a typed, weighted association between two memory objects.

```c
int cog_memory_associate(uint64_t from_id, uint64_t to_id,
                         const char *relation_type, float weight);
```

---

## Hardware API

### `cog_hw_allocate`

Allocate compute crystals.

```c
int cog_hw_allocate(cog_hw_kind_t kind, uint64_t units,
                    struct cog_hw_handle **hw);
```

### `cog_hw_submit`

Submit work to a crystal.

```c
int cog_hw_submit(struct cog_hw_handle *hw, const char *kernel,
                  const void *args, size_t args_len,
                  uint64_t *work_item_id);
```

### `cog_hw_sync`

Synchronize with a work item.

```c
int cog_hw_sync(struct cog_hw_handle *hw, uint64_t work_item_id,
                uint64_t timeout_ms, void **result, size_t *result_len);
```

---

## Error Codes

| Code | Name | Description |
|------|------|-------------|
| 0 | `COG_OK` | Success |
| -22 | `COG_EINVAL` | Invalid argument |
| -13 | `COG_EACCES` | Capability denied |
| -12 | `COG_ENOMEM` | Out of memory |
| -16 | `COG_EBUSY` | Resource busy |
| -2 | `COG_ENOENT` | No such object |
| -17 | `COG_EEXIST` | Object already exists |
| -11 | `COG_EAGAIN` | Try again |
| -125 | `COG_ECANCELED` | Operation canceled |
| -200 | `COG_E_SCHEMA_VIOLATION` | Schema validation failed |
| -201 | `COG_E_CAP_DENIED` | Capability not held |
| -202 | `COG_E_QUOTA_EXCEEDED` | Quota exceeded |
| -203 | `COG_E_INTENT_REJECTED` | Intent violates policy |
| -204 | `COG_E_AGENT_NOT_FOUND` | Agent handle invalid |
| -205 | `COG_E_TOOL_NOT_FOUND` | Tool handle invalid |
| -206 | `COG_E_STORE_CORRUPT` | Store integrity failure |
| -207 | `COG_E_HW_UNAVAILABLE` | Hardware unavailable |
| -208 | `COG_E_CHAIN_CYCLE` | Chain contains cycle |
| -209 | `COG_E_PROVENANCE_FAIL` | Provenance verification failed |

---

## Type Definitions

### `cog_handle_t`

Opaque handle for kernel objects.

```c
typedef uint64_t cog_handle_t;
#define COG_HANDLE_INVALID ((cog_handle_t)0)
```

### `cog_priority_t`

```c
typedef enum {
    COG_PRIORITY_BACKGROUND = 0,
    COG_PRIORITY_NORMAL     = 1,
    COG_PRIORITY_HIGH       = 2,
    COG_PRIORITY_CRITICAL   = 3,
} cog_priority_t;
```

### `cog_modality_t`

```c
typedef enum {
    COG_MODALITY_TEXT       = 0,
    COG_MODALITY_IMAGE      = 1,
    COG_MODALITY_AUDIO      = 2,
    COG_MODALITY_VIDEO      = 3,
    COG_MODALITY_MULTIMODAL = 4,
} cog_modality_t;
```

### `cog_hw_kind_t`

```c
typedef enum {
    COG_HW_CPU              = 0,
    COG_HW_GPU              = 1,
    COG_HW_NPU              = 2,
    COG_HW_FPGA             = 3,
    COG_HW_QPU              = 4,
} cog_hw_kind_t;
```

### `cog_agent_state_t`

```c
typedef enum {
    COG_AGENT_STATE_CREATED = 0,
    COG_AGENT_STATE_RUNNING = 1,
    COG_AGENT_STATE_WAITING = 2,
    COG_AGENT_STATE_DONE    = 3,
    COG_AGENT_STATE_FAILED  = 4,
    COG_AGENT_STATE_KILLED  = 5,
} cog_agent_state_t;
```

# Kernel Design

## The Differentiable Kernel

The CognitaOS kernel is not a fixed program. It is a differentiable computation graph whose parameters are continuously optimized for the workload it serves.

### Architecture

```
+------------------------------------------------------------------+
|                    THE DIFFERENTIAL KERNEL                        |
|                                                                  |
|   +-------------+    +-------------+    +-------------+          |
|   |  Attention   |    |  Semantic   |    |  Temporal   |          |
|   |  Scheduler  |    |  Memory     |    |  Resource   |          |
|   |  (Q* net)   |    |  Manager    |    |  Fabric     |          |
|   +------+------+    +------+------+    +------+------+          |
|          |                  |                  |                  |
|          +----------------+------------------+                  |
|                             |                                    |
|                             v                                    |
|                    +----------------+                            |
|                    |  Unified       |                            |
|                    |  Gradient      |                            |
|                    |  Engine        |                            |
|                    +--------+-------+                            |
|                             |                                    |
|                             v                                    |
|                    +----------------+                            |
|                    |  Kernel Weight |                            |
|                    |  Store         |                            |
|                    +----------------+                            |
+------------------------------------------------------------------+
```

## The Attention Scheduler

### Single-Pass Attention

At every scheduling quantum (250μs), the kernel computes:

```
Attention(Q, K, V) = softmax(QK^T / sqrt(d_k)) V
```

Where:
- **Q** = system context query (user intent, energy state, thermal state, deadlines)
- **K** = entity keys (semantic state, resource needs, urgency, energy profile)
- **V** = entity values (allocation decisions)

### Output

The output is a complete allocation matrix:

```c
struct attention_schedule_output {
    struct entity_allocation {
        uint64_t    ea_entity_id;
        uint8_t     ea_core_affinity_mask;
        uint8_t     ea_npu_partition;
        uint16_t    ea_gpu_sm_count;
        uint32_t    ea_memory_pages;
        uint64_t    ea_cycles_allocated;
        uint8_t     ea_power_state;
        float       ea_attention_weight;
    } allocations[MAX_ENTITIES];

    uint32_t    allocation_count;
    float       total_system_utility;
    float       predicted_energy_joules;
    uint64_t    predicted_deadline_misses;
};
```

### Transformer Architecture

The RL scheduler is a small transformer (2M parameters) that runs on the NPU:

```c
struct rl_scheduler_transformer {
    uint32_t    rst_head_count;         /* 8 attention heads */
    uint32_t    rst_model_dim;          /* 512 */
    uint32_t    rst_ffn_dim;            /* 2048 */
    uint32_t    rst_layer_count;        /* 6 layers */
};
```

Inference time: < 5μs per scheduling decision.

## The Semantic Memory Manager

### The Relevance Field

Memory pages exist on a continuum from "pinned in SRAM" to "evicted to storage":

```
SRAM <---> L2 <---> DRAM <---> NPU HBM <---> GPU HBM <---> CXL
  |          |          |           |              |          |
pinned     hot        warm        cold         frozen     evicted
(R~1.0)   (R~0.8)   (R~0.5)    (R~0.2)      (R~0.0)    (R<0.0)
```

The relevance of each page is a learned function:

```
R(page_i, context) = sigmoid(w^T · [embed(page_i); embed(context); access_pattern_i])
```

### Attention-Based Access

Every memory read is an attention query:

```c
function ATTEND(query_embedding, field, top_k):
    scores = query_embedding @ field.keys.T / sqrt(d_k)
    top_indices = argtopk(scores, top_k)
    output = softmax(scores[top_indices]) @ field.values[top_indices]
    return output
```

There are no page faults. There are no cache misses. There is only relevance-weighted recall.

## The Temporal Resource Fabric

Multi-horizon scheduling across five time scales:

| Horizon | Scale | What's Scheduled |
|---------|-------|-----------------|
| Nanosecond | 1-100ns | Gate-level power gating |
| Microsecond | 1-100μs | Context switches, TLB invalidation |
| Millisecond | 1-100ms | Agent execution, tool invocation |
| Second | 1-100s | Agent lifecycle, memory compaction |
| Minute | 1-100min | Model retraining, kernel evolution |

## The Symbolic Reasoning Core

Every neural decision is accompanied by a proof obligation:

```
Neural decision: "Switch to agent A, allocate 4 NPU slices"
Proof obligation: "Given state S, this does not violate I_mem, I_energy,
                   I_deadline, I_security"
```

The symbolic reasoner uses:
- **SMT solvers** (Z3/CVC5) for constraint checking
- **Theorem provers** (Lean 4) for complex invariants
- **Model checking** for temporal properties
- **Causal reasoning** for counterfactual analysis

## Kernel Evolution

The kernel can modify its own code through a controlled process:

1. **Observe** — Monitor performance metrics
2. **Propose** — Meta-learner suggests modifications
3. **Verify** — Symbolic reasoner proves safety
4. **Shadow** — Run in parallel with production kernel
5. **Evaluate** — Compare shadow vs. production performance
6. **Commit** — Atomically deploy if better
7. **Rollback** — Automatic rollback if regression detected

### Kernel Genome

```c
struct kernel_genome {
    uint64_t    kg_version;
    uint8_t     kg_merkle_root[32];
    uint64_t    kg_parent_version;

    struct kg_scheduling {
        float   kgs_attention_temperature;
        float   kgs_switch_cost_weight;
        float   kgs_energy_weight;
        float   kgs_fairness_weight;
        float   kgs_drift_threshold;
        uint32_t kgs_quantum_us;
    } scheduling;

    struct kg_memory {
        float   kgm_relevance_decay_rate;
        float   kgm_learning_rate;
        float   kgm_compression_threshold;
        float   kgm_eviction_watermark;
    } memory;

    struct kg_security {
        float   kgs_proof_timeout_ms;
        float   kgs_max_delegation_depth;
        uint32_t kgs_qkd_key_refresh_ms;
    } security;
};
```

## Context Switching

Context switching is attention reallocation, not state save/restore:

```
BEFORE:  Agent A: 70% attention, Agent B: 20% attention
AFTER:   Agent A: 10% attention, Agent B: 80% attention
```

The "save" is persisting attention head weights to the memory field.
The "restore" is re-inflating attention head weights from the memory field.

Switch latency: < 20μs (no NPU/GPU migration), < 50μs (with migration).

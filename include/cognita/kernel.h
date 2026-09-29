/**
 * @file kernel.h
 * @brief CognitaOS Kernel Internal API
 *
 * This header is for kernel-internal use. User programs should use os.h.
 */

#ifndef COGNITA_KERNEL_H
#define COGNITA_KERNEL_H

#include <cognita/os.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * Attention Scheduler
 * ======================================================================== */

#define MAX_ENTITIES        4096
#define MAX_ATTENTION_HEADS 8
#define MODEL_DIM           512
#define SCHEDULING_QUANTUM_US 250

struct entity_allocation {
    uint64_t    ea_entity_id;
    uint8_t     ea_core_affinity_mask;
    uint8_t     ea_npu_partition;
    uint16_t    ea_gpu_sm_count;
    uint32_t    ea_memory_pages;
    uint64_t    ea_cycles_allocated;
    uint8_t     ea_power_state;
    float       ea_attention_weight;
};

struct attention_schedule_output {
    struct entity_allocation allocations[MAX_ENTITIES];
    uint32_t    allocation_count;
    float       total_system_utility;
    float       predicted_energy_joules;
    uint64_t    predicted_deadline_misses;
};

int attention_schedule_compute(const float *query,
                               const float *keys,
                               const float *values,
                               uint32_t num_entities,
                               struct attention_schedule_output *output);

/* ========================================================================
 * Semantic Memory
 * ======================================================================== */

#define SWS_MAX_DIM         1024
#define SWS_MAX_ENTRIES     4096
#define SWS_DEFAULT_DIM     768

struct sws_entry {
    uint64_t    se_token_id;
    uint64_t    se_sequence;
    uint32_t    se_epoch;
    uint8_t     se_type;
    uint8_t     se_compression;
    uint8_t     se_flags;
    uint8_t     _pad0;
    uint16_t    se_dim;
    uint16_t    se_alloc_dim;
    float       se_vector[];
};

#define SEMANTIC_TYPE_INTENT       0x01
#define SEMANTIC_TYPE_TOOL_STATE   0x02
#define SEMANTIC_TYPE_CONVERSATION 0x03
#define SEMANTIC_TYPE_ENV_SENSOR   0x04
#define SEMANTIC_TYPE_PLAN         0x05
#define SEMANTIC_TYPE_EPISODIC     0x06
#define SEMANTIC_TYPE_ATTENTION    0x07

#define SWS_FLAG_PINNED         0x01
#define SWS_FLAG_DIRTY          0x02
#define SWS_FLAG_COMPRESSED     0x04
#define SWS_FLAG_REFERENCED     0x08

struct semantic_working_set;

struct semantic_working_set *sws_create(uint64_t entity_id, uint32_t max_dim);
void sws_destroy(struct semantic_working_set *sws);
int sws_save(struct sched_entity *entity);
int sws_restore(struct sched_entity *entity);

/* ========================================================================
 * Context Switch
 * ======================================================================== */

#define SWITCH_FLAG_HOT         0x01
#define SWITCH_FLAG_FULL        0x02
#define SWITCH_FLAG_FORCE       0x04

struct sched_entity;

int context_switch(struct sched_entity *prev,
                   struct sched_entity *next,
                   uint32_t flags);

/* ========================================================================
 * Kernel Evolution
 * ======================================================================== */

struct kernel_genome;

struct kernel_genome *kernel_genome_current(void);
int kernel_evolve_propose(struct kernel_genome *proposal);
int kernel_evolve_verify(const struct kernel_genome *proposal);
int kernel_evolve_commit(struct kernel_genome *proposal);
int kernel_evolve_rollback(uint64_t version);

#ifdef __cplusplus
}
#endif

#endif /* COGNITA_KERNEL_H */

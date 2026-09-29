/**
 * @file context_switch.c
 * @brief Attention-reallocation context switching
 */

#include "cognita/kernel.h"
#include <time.h>

/* ========================================================================
 * Cycle Counter
 * ======================================================================== */

#if defined(__x86_64__) || defined(__i386__)
#include <x86intrin.h>
static inline uint64_t rdtsc(void) {
    return __rdtsc();
}
#elif defined(__aarch64__)
static inline uint64_t rdtsc(void) {
    uint64_t val;
    asm volatile("mrs %0, cntvct_el0" : "=r"(val));
    return val;
}
#else
static inline uint64_t rdtsc(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}
#endif

/* ========================================================================
 * Context Switch
 * ======================================================================== */

int context_switch(struct sched_entity *prev,
                   struct sched_entity *next,
                   uint32_t flags)
{
    if (!prev || !next)
        return COG_EINVAL;

    if (prev == next && !(flags & SWITCH_FLAG_FORCE))
        return COG_OK;

    uint64_t t_start = rdtsc();

    /* S2: Pre-switch hook */
    /* Notify NPU/GPU drivers (stub) */

    /* S3: SWS Save (hot path) */
    if (prev->sws) {
        sws_save(prev);
    }

    uint64_t t_save = rdtsc();

    /* S4: NPU/GPU context migration (cold path) */
    if (flags & SWITCH_FLAG_FULL) {
        /* DMA NPU state, invalidate GPU page tables (stub) */
    }

    uint64_t t_migrate = rdtsc();

    /* Scheduler barrier: hardware context switch */
    /* arch_context_switch(prev, next) — registers, PC, SP, page tables */

    /* S5: NPU/GPU context restore (cold path) */
    if (flags & SWITCH_FLAG_FULL) {
        /* Restore NPU state, rebuild GPU page tables (stub) */
    }

    /* S6: SWS Restore (hot path) */
    if (next->sws) {
        sws_restore(next);
    }

    uint64_t t_restore = rdtsc();

    /* S7: Vector cache invalidation */
    /* invlpgb_flush_tag(prev->vec_cache_tag) */
    /* vec_cache_warm_entity(next) */

    /* S8: Post-switch hook */
    /* Resume semantic clock, notify drivers, update RL metrics */

    /* Latency budget check */
    uint64_t total_cycles = rdtsc() - t_start;
    (void)total_cycles;  /* used for RL scheduler feedback */

    return COG_OK;
}

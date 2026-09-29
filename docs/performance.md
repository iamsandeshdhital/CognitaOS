# CognitaOS Performance

## Targets

### Latency

| Operation | Traditional OS | CognitaOS | Improvement |
|-----------|---------------|-----------|-------------|
| Context switch | 1-10 μs | < 20 μs | Comparable (with full semantic state) |
| Memory access (hot) | 100 ns | 50 ns | 2x |
| Memory access (cold) | 10 ms | 100 μs | 100x |
| Intent compilation | N/A | < 5 ms | New capability |
| Agent spawn | 1-10 ms | < 100 μs | 10-100x |
| Agent fork | 1-10 ms | < 50 μs | 20-200x |
| Tool invocation | 1-10 μs | < 5 μs | 2-2x |
| Semantic query | 1-100 ms | < 1 ms | 10-100x |
| Kernel evolution | N/A | < 100 ms | New capability |

### Throughput

| Metric | Traditional OS | CognitaOS | Improvement |
|--------|---------------|-----------|-------------|
| Context switches/sec/core | 100K-1M | 500K | 5-10x |
| Concurrent agents | 10K | 1M | 100x |
| Semantic queries/sec | 10K | 1M | 100x |
| Memory bandwidth | 100 GB/s | 10 TB/s | 100x |
| Inter-crystal bandwidth | 10 GB/s | 64 TB/s | 6400x |

### Energy

| Metric | Traditional OS | CognitaOS | Improvement |
|--------|---------------|-----------|-------------|
| Idle power | 10-100 W | < 1 W | 10-100x |
| Performance per watt | 1x | 100-1000x | 100-1000x |
| Energy per context switch | 100 nJ | 1 nJ | 100x |
| Energy per semantic query | 100 μJ | 100 nJ | 1000x |

## Benchmarks

### Context Switch Benchmark

```c
/* tests/benchmarks/bench_context_switch.c */
#include <cognita/bench.h>

BENCHMARK(context_switch, hot_path) {
    struct sched_entity *a = sched_entity_create("A");
    struct sched_entity *b = sched_entity_create("B");

    for (int i = 0; i < 100000; i++) {
        context_switch(a, b, SWITCH_FLAG_HOT);
        context_switch(b, a, SWITCH_FLAG_HOT);
    }

    sched_entity_destroy(a);
    sched_entity_destroy(b);
}

/* Expected: < 20μs per switch on NPU-equipped system */
```

### Semantic Query Benchmark

```c
/* tests/benchmarks/bench_semantic_query.c */
#include <cognita/bench.h>

BENCHMARK(semantic_query, hnsw_1m) {
    struct memory_field *field = memory_field_create();
    memory_field_populate(field, 1000000);  /* 1M objects */

    float query[768];
    random_embedding(query, 768);

    for (int i = 0; i < 10000; i++) {
        struct cog_memory_result *results;
        size_t count;
        cog_memory_attend(query, 768, &results, &count);
        cog_memory_result_free(results, count);
    }

    memory_field_destroy(field);
}

/* Expected: < 1ms per query on 1M object field */
```

### Intent Compilation Benchmark

```c
/* tests/benchmarks/bench_intent_compile.c */
#include <cognita/bench.h>

BENCHMARK(intent_compile, complex_query) {
    const char *intent = "Summarize my Q3 reports and email the CFO";

    for (int i = 0; i < 1000; i++) {
        struct cog_intent_expr expr = {
            .ie_type_uri = "cognita/Summarize/v1",
            .ie_body = (void *)intent,
            .ie_body_len = strlen(intent),
        };
        struct cog_intent_result result;
        cog_intent(&expr, &result);
    }
}

/* Expected: < 5ms per compilation */
```

## Profiling

### Built-in Profiler

```c
#include <cognita/profiler.h>

PROFILE_START(my_operation);
/* ... code to profile ... */
PROFILE_END(my_operation);

/* Output:
 * my_operation: 1234 μs (p50: 1200, p99: 1500, p999: 2000)
 *   - attention_schedule: 5 μs
 *   - semantic_memory: 50 μs
 *   - capability_invoke: 1100 μs
 *   - verification: 79 μs
 */
```

### Energy Profiler

```c
#include <cognita/energy.h);

ENERGY_START(my_operation);
/* ... code ... */
struct energy_report *report = ENERGY_END(my_operation);

printf("Energy: %lu nJ\n", report->total_nj);
printf("  CPU:  %lu nJ\n", report->cpu_nj);
printf("  NPU:  %lu nJ\n", report->npu_nj);
printf("  DRAM: %lu nJ\n", report->dram_nj);
```

## Optimization Tips

1. **Use capability chains** — Composing capabilities into a chain is faster than invoking them individually
2. **Pin hot memory** — Frequently accessed memory should be pinned in the hot field
3. **Batch intents** — Multiple related intents can be batched into a single compilation
4. **Use agent sleep/wake** — Sleeping agents consume zero energy; waking is faster than spawning
5. **Prefer NPU for tensor operations** — The attention scheduler automatically prefers NPU for tensor ops, but explicit hints help

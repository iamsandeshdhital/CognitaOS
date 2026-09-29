/**
 * @file bench_context_switch.c
 * @brief Context switch performance benchmark
 */

#include <cognita/kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ITERATIONS 100000

static double get_time_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1e9 + (double)ts.tv_nsec;
}

int main(void) {
    printf("Context Switch Benchmark\n");
    printf("========================\n");
    printf("Iterations: %d\n\n", ITERATIONS);

    struct sched_entity *a = (struct sched_entity *)calloc(1, sizeof(*a));
    struct sched_entity *b = (struct sched_entity *)calloc(1, sizeof(*a));
    a->sws = sws_create(1, SWS_DEFAULT_DIM);
    b->sws = sws_create(2, SWS_DEFAULT_DIM);

    /* Warmup */
    for (int i = 0; i < 1000; i++) {
        context_switch(a, b, SWITCH_FLAG_HOT);
        context_switch(b, a, SWITCH_FLAG_HOT);
    }

    /* Benchmark */
    double start = get_time_ns();
    for (int i = 0; i < ITERATIONS; i++) {
        context_switch(a, b, SWITCH_FLAG_HOT);
        context_switch(b, a, SWITCH_FLAG_HOT);
    }
    double end = get_time_ns();

    double total_ns = end - start;
    double per_switch_ns = total_ns / (ITERATIONS * 2.0);

    printf("Total time: %.2f ms\n", total_ns / 1e6);
    printf("Per switch: %.2f ns (%.3f us)\n", per_switch_ns, per_switch_ns / 1000.0);
    printf("Switches/sec: %.0f\n", 1e9 / per_switch_ns);

    if (per_switch_ns < 20000.0) {
        printf("\nPASS: Switch latency < 20 us\n");
    } else {
        printf("\nWARN: Switch latency >= 20 us\n");
    }

    sws_destroy(a->sws);
    sws_destroy(b->sws);
    free(a);
    free(b);

    return 0;
}

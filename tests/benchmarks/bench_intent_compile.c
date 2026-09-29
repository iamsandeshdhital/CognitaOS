/**
 * @file bench_intent_compile.c
 * @brief Intent compilation performance benchmark
 */

#include <cognita/os.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ITERATIONS 1000

static double get_time_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1e9 + (double)ts.tv_nsec;
}

int main(void) {
    printf("Intent Compilation Benchmark\n");
    printf("=============================\n");
    printf("Iterations: %d\n\n", ITERATIONS);

    const char *intent = "Summarize my Q3 reports and email the CFO";

    /* Warmup */
    for (int i = 0; i < 10; i++) {
        struct cog_intent_expr expr = {
            .ie_type_uri = "cognita/Summarize/v1",
            .ie_body = (void *)intent,
            .ie_body_len = strlen(intent),
        };
        struct cog_intent_result result;
        cog_intent(&expr, &result);
    }

    /* Benchmark */
    double start = get_time_ns();
    for (int i = 0; i < ITERATIONS; i++) {
        struct cog_intent_expr expr = {
            .ie_type_uri = "cognita/Summarize/v1",
            .ie_body = (void *)intent,
            .ie_body_len = strlen(intent),
        };
        struct cog_intent_result result;
        cog_intent(&expr, &result);
    }
    double end = get_time_ns();

    double total_ns = end - start;
    double per_compile_ns = total_ns / ITERATIONS;

    printf("Total time: %.2f ms\n", total_ns / 1e6);
    printf("Per compilation: %.2f ns (%.3f us)\n", per_compile_ns, per_compile_ns / 1000.0);
    printf("Compilations/sec: %.0f\n", 1e9 / per_compile_ns);

    if (per_compile_ns < 5000000.0) {
        printf("\nPASS: Compilation latency < 5 ms\n");
    } else {
        printf("\nWARN: Compilation latency >= 5 ms\n");
    }

    return 0;
}

/**
 * @file bench_semantic_query.c
 * @brief Semantic query performance benchmark
 */

#include <cognita/os.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define ITERATIONS 10000
#define DIM 768

static double get_time_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1e9 + (double)ts.tv_nsec;
}

static void random_embedding(float *emb, uint32_t dim) {
    for (uint32_t i = 0; i < dim; i++) {
        emb[i] = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
    }
}

int main(void) {
    printf("Semantic Query Benchmark\n");
    printf("========================\n");
    printf("Iterations: %d\n", ITERATIONS);
    printf("Dimension: %d\n\n", DIM);

    float query[DIM];
    random_embedding(query, DIM);

    /* Warmup */
    for (int i = 0; i < 100; i++) {
        struct cog_memory_result *results = NULL;
        size_t count = 0;
        cog_memory_attend(query, DIM, &results, &count);
    }

    /* Benchmark */
    double start = get_time_ns();
    for (int i = 0; i < ITERATIONS; i++) {
        struct cog_memory_result *results = NULL;
        size_t count = 0;
        cog_memory_attend(query, DIM, &results, &count);
    }
    double end = get_time_ns();

    double total_ns = end - start;
    double per_query_ns = total_ns / ITERATIONS;

    printf("Total time: %.2f ms\n", total_ns / 1e6);
    printf("Per query: %.2f ns (%.3f us)\n", per_query_ns, per_query_ns / 1000.0);
    printf("Queries/sec: %.0f\n", 1e9 / per_query_ns);

    if (per_query_ns < 1000000.0) {
        printf("\nPASS: Query latency < 1 ms\n");
    } else {
        printf("\nWARN: Query latency >= 1 ms\n");
    }

    return 0;
}

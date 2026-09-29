/**
 * @file compiler.h
 * @brief CognitaOS Intent Compiler Internal API
 */

#ifndef COGNITA_COMPILER_H
#define COGNITA_COMPILER_H

#include <cognita/os.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * CognitaIR — Intermediate Representation
 * ======================================================================== */

#define CIR_MAX_NODES       256
#define CIR_MAX_EDGES       512

struct cir_node {
    uint64_t    cn_id;
    char        cn_capability[128];
    uint64_t    cn_input_count;
    uint64_t    cn_output_count;
    uint8_t     cn_preferred_crystal;
    uint64_t    cn_estimated_cycles;
    float       cn_estimated_energy_joules;
};

struct cir_edge {
    uint64_t    ce_from_node;
    uint64_t    ce_to_node;
    char        ce_type[64];
    uint8_t     ce_schema_hash[32];
};

struct cognitair_program {
    struct cir_node nodes[CIR_MAX_NODES];
    struct cir_edge edges[CIR_MAX_EDGES];
    uint32_t node_count;
    uint32_t edge_count;
};

/* ========================================================================
 * Compiler Stages
 * ======================================================================== */

int cir_lexical_attention(const char *text, float **embeddings,
                          float **weights, uint32_t *token_count);

int cir_semantic_parse(const float *embeddings, uint32_t token_count,
                       void **amr_graph);

int cir_goal_decompose(void *amr_graph, struct cognitair_program *program);

int cir_capability_resolve(struct cognitair_program *program);

int cir_resource_plan(struct cognitair_program *program);

int cir_verify(struct cognitair_program *program);

int cir_codegen(struct cognitair_program *program, void **binary,
                size_t *binary_len);

#ifdef __cplusplus
}
#endif

#endif /* COGNITA_COMPILER_H */

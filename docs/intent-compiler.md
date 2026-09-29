# The Intent Compiler

## Overview

The Intent Compiler is the heart of CognitaOS. It transforms natural language into verified, optimized, hardware-specific execution plans through a seven-stage pipeline.

```
User Intent
    |
    v
[1] Lexical Attention     -- Tokenize, embed, attention-weight
    |
    v
[2] Semantic Parsing      -- Abstract Meaning Representation
    |
    v
[3] Goal Decomposition    -- DAG of sub-goals
    |
    v
[4] Capability Resolution -- Map sub-goals to capabilities
    |
    v
[5] Resource Planning     -- Assign capabilities to crystals
    |
    v
[6] Verification          -- Formal proof of safety/correctness
    |
    v
[7] Code Generation       -- Hardware-specific binary
    |
    v
Execution Plan
```

## Stage 1: Lexical Attention

The input text is tokenized and embedded using a lightweight transformer. Each token receives an attention weight based on its relevance to the overall intent.

```c
struct lexical_attention_output {
    float      *token_embeddings;    /* [num_tokens, embedding_dim] */
    float      *token_weights;       /* [num_tokens] */
    uint32_t    token_count;
    uint32_t    embedding_dim;
};
```

**Example:**
```
Input:  "Summarize my Q3 reports and email the CFO"
Tokens: ["Summarize", "my", "Q3", "reports", "and", "email", "the", "CFO"]
Weights: [0.35,       0.05, 0.20, 0.25,     0.02, 0.28,   0.03, 0.30]
```

High-weight tokens ("Summarize", "Q3", "reports", "email", "CFO") drive the semantic parse.

## Stage 2: Semantic Parsing

The token embeddings are parsed into an Abstract Meaning Representation (AMR) graph:

```
(summarize-01
    :ARG0 (report
        :mod (quarter
            :ord 3))
    :ARG1 (email-01
        :ARG0 (person
            :mod CFO)))
```

This is produced by a neural semantic parser trained on intent-AMR pairs, with symbolic constraints ensuring well-formedness.

## Stage 3: Goal Decomposition

The AMR graph is transformed into a Directed Acyclic Graph (DAG) of sub-goals:

```
[DAG]
  find_reports -> read_reports -> summarize -> compose_email -> send_email
```

Each node is a sub-goal with:
- A semantic type
- Input/output type constraints
- Estimated cost and latency

## Stage 4: Capability Resolution

Each sub-goal is mapped to a capability by querying the capability index:

```c
struct capability_query {
    float       cq_embedding[768];   /* sub-goal embedding */
    uint32_t    cq_top_k;            /* max results */
    float       cq_min_similarity;   /* relevance threshold */
};

struct capability_match {
    struct capability *cm_cap;
    float              cm_similarity;
};
```

The capability index is an HNSW graph over capability embeddings, enabling sub-millisecond semantic search.

## Stage 5: Resource Planning

Each capability invocation is assigned to a compute crystal based on:

| Capability Type | Preferred Crystal | Reason |
|----------------|-------------------|--------|
| `store.semantic_query` | NPU | Embedding search is tensor operation |
| `pdf.parse` | NPU | Layout analysis uses computer vision |
| `llm.infer` | NPU | Transformer inference |
| `email.send` | CPU | Network I/O, control flow |
| `template.render` | CPU | String manipulation |

The resource planner solves a constraint satisfaction problem:

```
Minimize:   total_energy + lambda * total_latency
Subject to:
    - Each capability assigned to exactly one crystal
    - Crystal capacity constraints
    - Data dependency constraints (zero-copy where possible)
    - Deadline constraints
```

## Stage 6: Verification

The execution plan is formally verified against safety invariants:

| Invariant | Checker | Description |
|-----------|---------|-------------|
| `I_mem` | SMT solver | No memory safety violations |
| `I_energy` | SMT solver | Total energy within budget |
| `I_deadline` | SMT solver | All deadlines can be met |
| `I_security` | Theorem prover | No capability escalation |
| `I_causal` | Model checker | No causal ordering violations |

If any proof obligation cannot be discharged, the plan is rejected and a conservative fallback is used.

## Stage 7: Code Generation

The verified plan is compiled to hardware-specific code:

- **NPU kernels** → PTX/ROCm with tensor operation fusion
- **CPU code** → RISC-V with capability annotations
- **Cross-crystal data movement** → Zero-copy DMA descriptors

The output is a CognitaIR program:

```c
struct cognitair_program {
    struct cir_node nodes[MAX_NODES];
    struct cir_edge edges[MAX_EDGES];
    uint32_t node_count;
    uint32_t edge_count;
};
```

## Optimization Passes

The Intent Compiler applies several optimization passes:

| Pass | Description |
|------|-------------|
| **Fusion** | Merge consecutive operations on same crystal |
| **Pipelining** | Overlap independent operations across crystals |
| **Memoization** | Cache repeated sub-results |
| **Dead Code Elimination** | Remove unreachable nodes |
| **Capability Coarsening** | Replace fine-grained capabilities with coarse-grained equivalents when safe |

## Error Recovery

If any stage fails, the compiler falls back to a simpler strategy:

| Failure | Fallback |
|---------|----------|
| Semantic parse failure | Keyword matching against capability index |
| Capability not found | Decompose into simpler sub-goals |
| Verification failure | Use conservative (rule-based) execution plan |
| Resource planning failure | Serialize all operations on CPU |

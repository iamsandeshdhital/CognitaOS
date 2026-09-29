# Getting Started with CognitaOS

## Installation

### From Source

```bash
# Clone the repository
git clone https://github.com/iamsandeshdhital/CognitaOS.git
cd CognitaOS

# Create build directory
mkdir build && cd build

# Configure
cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release

# Build
ninja

# Run tests
ctest --output-on-failure
```

### Using Docker

```bash
docker build -t cognitaos .
docker run -it cognitaos
```

## Your First Intent

```c
#include <cognita/os.h>
#include <stdio.h>

int main() {
    /* Express an intent */
    struct cog_intent_expr expr = {
        .ie_type_uri = "cognita/HelloWorld/v1",
        .ie_body = NULL,
        .ie_body_len = 0,
        .ie_deadline_ns = 1000000000,  /* 1 second */
        .ie_max_cost_usd = 100,         /* $0.0001 */
        .ie_modality = COG_MODALITY_TEXT,
    };

    struct cog_intent_result result;
    int ret = cog_intent(&expr, &result);

    if (ret == COG_OK) {
        printf("Intent completed in %lu us\n", result.ir_execution_time_us);
        printf("Cost: $%.6f\n", result.ir_cost_usd / 1000000.0f);
        printf("Energy: %lu nJ\n", result.ir_energy_joules);
    } else {
        printf("Intent failed: %d\n", ret);
    }

    return ret;
}
```

Compile and run:

```bash
gcc -o hello_intent hello_intent.c -lcognita -L./build/lib -I./include
LD_LIBRARY_PATH=./build/lib ./hello_intent
```

## Core Concepts

### 1. Intent

An intent is a typed, schema-validated declaration of a desired outcome:

```c
struct cog_intent_expr {
    char        ie_type_uri[256];    /* semantic type */
    uint8_t    *ie_body;             /* Protobuf-serialized body */
    size_t      ie_body_len;
    uint64_t    ie_deadline_ns;
    uint64_t    ie_max_cost_usd;
    cog_modality_t ie_modality;
};
```

### 2. Capability

A capability is a typed, schema-validated function:

```c
struct capability {
    uint64_t    cap_id;
    char        cap_name[128];       /* e.g., "pdf.summarize" */
    char        cap_version[32];
    uint64_t    cap_required_caps;
    uint8_t     cap_sandbox_level;
};
```

### 3. Agent

An agent is a persistent, stateful execution context:

```c
struct agent {
    uint64_t    ag_id;
    char        ag_name[128];
    char        ag_role[64];
    uint64_t    ag_capability_mask;
    cog_agent_state_t ag_state;
};
```

### 4. Memory

Memory is accessed by attention, not by address:

```c
/* Attend to memory (read) */
struct cog_memory_result *results;
size_t result_count;
cog_memory_attend(query_embedding, 768, &results, &result_count);

/* Commit to memory (write) */
uint64_t object_id;
cog_memory_commit(key_embedding, 768, data, data_len, &object_id);
```

## Examples

See the `examples/` directory for complete, runnable examples:

| Example | Description |
|---------|-------------|
| `hello_intent.c` | Minimal intent submission |
| `agent_orchestration.c` | Spawn and coordinate multiple agents |
| `semantic_storage.c` | Store and query semantic data |
| `capability_chain.c` | Compose capabilities into a pipeline |

## Next Steps

- Read the [API Reference](api-reference.md) for complete API documentation
- Read the [Intent Compiler](intent-compiler.md) guide
- Read the [Kernel Design](kernel-design.md) document
- Join the [GitHub Discussions](https://github.com/iamsandeshdhital/CognitaOS/discussions)

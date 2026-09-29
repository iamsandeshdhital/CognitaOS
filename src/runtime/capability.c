/**
 * @file capability.c
 * @brief Capability discovery and invocation
 */

#include "cognita/runtime.h"
#include <string.h>
#include <stdlib.h>

/* ========================================================================
 * Capability Registry (stub)
 * ======================================================================== */

static struct capability capability_registry[] = {
    {
        .cap_id = 1,
        .cap_name = "store.semantic_query",
        .cap_version = "1.0.0",
        .cap_type_uri = "cognita/StoreQuery/v1",
        .cap_required_caps = COG_CAP_STORE_WRITE,
        .cap_sandbox_level = 1,
        .cap_timeout_ms = 5000,
        .cap_max_retries = 3,
        .cap_cost_per_invocation_usd = 10,
    },
    {
        .cap_id = 2,
        .cap_name = "llm.infer",
        .cap_version = "2.0.0",
        .cap_type_uri = "cognita/LLMInference/v1",
        .cap_required_caps = COG_CAP_TOOL_INVOKE,
        .cap_sandbox_level = 1,
        .cap_timeout_ms = 30000,
        .cap_max_retries = 2,
        .cap_cost_per_invocation_usd = 500,
    },
    {
        .cap_id = 3,
        .cap_name = "email.send",
        .cap_version = "1.0.0",
        .cap_type_uri = "cognita/EmailSend/v1",
        .cap_required_caps = COG_CAP_TOOL_INVOKE,
        .cap_sandbox_level = 2,
        .cap_timeout_ms = 10000,
        .cap_max_retries = 3,
        .cap_cost_per_invocation_usd = 50,
    },
    {
        .cap_id = 4,
        .cap_name = "pdf.parse",
        .cap_version = "1.0.0",
        .cap_type_uri = "cognita/PDFParse/v1",
        .cap_required_caps = COG_CAP_TOOL_INVOKE,
        .cap_sandbox_level = 1,
        .cap_timeout_ms = 15000,
        .cap_max_retries = 2,
        .cap_cost_per_invocation_usd = 200,
    },
};

static const uint32_t capability_count =
    sizeof(capability_registry) / sizeof(capability_registry[0]);

/* ========================================================================
 * Capability Lookup
 * ======================================================================== */

struct capability *capability_lookup(const char *name) {
    if (!name) return NULL;

    for (uint32_t i = 0; i < capability_count; i++) {
        if (strcmp(capability_registry[i].cap_name, name) == 0)
            return &capability_registry[i];
    }

    return NULL;
}

/* ========================================================================
 * Capability Invoke
 * ======================================================================== */

int capability_invoke(struct capability *cap, const void *args,
                       size_t args_len, void **result, size_t *result_len)
{
    if (!cap)
        return COG_E_TOOL_NOT_FOUND;

    if (!result || !result_len)
        return COG_EINVAL;

    /* In production, this dispatches to the appropriate executor */
    /* For now, return a stub result */
    *result = (void *)1;
    *result_len = 0;

    return COG_OK;
}

/* ========================================================================
 * Capability Discovery
 * ======================================================================== */

int cog_capability_discover(const char *semantic_query,
                            struct capability **results,
                            size_t *result_count)
{
    if (!semantic_query || !results || !result_count)
        return COG_EINVAL;

    /* In production, this queries the HNSW capability index */
    /* For now, return all capabilities */
    *results = (struct capability *)malloc(
        capability_count * sizeof(struct capability));
    if (!*results) return COG_ENOMEM;

    memcpy(*results, capability_registry,
           capability_count * sizeof(struct capability));
    *result_count = capability_count;

    return COG_OK;
}

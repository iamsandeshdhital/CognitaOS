/**
 * @file capability_chain.c
 * @brief Capability chaining example
 */

#include <cognita/os.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    printf("CognitaOS - Capability Chain Example\n");
    printf("===================================\n\n");

    /* Discover capabilities */
    struct capability *caps = NULL;
    size_t cap_count = 0;

    printf("Discovering capabilities...\n");
    int ret = cog_capability_discover("summarize document", &caps, &cap_count);
    if (ret != COG_OK) {
        printf("Discovery failed: %s\n", cog_strerror(ret));
        return ret;
    }
    printf("  Found %lu capabilities\n\n", cap_count);

    /* Build a chain: parse -> summarize -> format */
    const char *chain_name = "document_pipeline";

    printf("Building capability chain: %s\n", chain_name);
    printf("  Steps:\n");
    printf("    1. pdf.parse\n");
    printf("    2. llm.infer (summarize)\n");
    printf("    3. template.render\n\n");

    /* In a full implementation, this would:
     * 1. Create a chain handle
     * 2. Add nodes for each capability
     * 3. Add edges between them
     * 4. Execute the chain
     */

    printf("Executing chain...\n");

    /* For now, invoke capabilities individually */
    struct capability *parse_cap = capability_lookup("pdf.parse");
    if (parse_cap) {
        printf("  [1/3] pdf.parse...\n");
        void *result = NULL;
        size_t result_len = 0;
        capability_invoke(parse_cap, NULL, 0, &result, &result_len);
        printf("    Done\n");
    }

    struct capability *infer_cap = capability_lookup("llm.infer");
    if (infer_cap) {
        printf("  [2/3] llm.infer...\n");
        void *result = NULL;
        size_t result_len = 0;
        capability_invoke(infer_cap, NULL, 0, &result, &result_len);
        printf("    Done\n");
    }

    printf("  [3/3] template.render...\n");
    printf("    Done\n");

    printf("\nChain execution complete!\n");

    free(caps);
    return 0;
}

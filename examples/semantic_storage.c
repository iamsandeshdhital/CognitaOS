/**
 * @file semantic_storage.c
 * @brief Semantic storage example
 */

#include <cognita/os.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {
    printf("CognitaOS - Semantic Storage Example\n");
    printf("====================================\n\n");

    /* Commit some data to semantic storage */
    float key1[768] = {0};
    key1[0] = 1.0f;  /* simple embedding */

    const char *doc1 = "Q3 Revenue Report: Revenue increased 15% YoY";
    uint64_t id1 = 0;

    printf("Storing: \"%s\"\n", doc1);
    int ret = cog_memory_commit(key1, 768, doc1, strlen(doc1), &id1);
    if (ret == COG_OK) {
        printf("  Stored with object ID: %lu\n", id1);
    }

    float key2[768] = {0};
    key2[1] = 1.0f;

    const char *doc2 = "Q3 Customer Satisfaction: NPS score improved to 72";
    uint64_t id2 = 0;

    printf("\nStoring: \"%s\"\n", doc2);
    ret = cog_memory_commit(key2, 768, doc2, strlen(doc2), &id2);
    if (ret == COG_OK) {
        printf("  Stored with object ID: %lu\n", id2);
    }

    /* Associate the two documents */
    printf("\nAssociating documents...\n");
    ret = cog_memory_associate(id1, id2, "related_to", 0.8f);
    if (ret == COG_OK) {
        printf("  Associated %lu -> %lu\n", id1, id2);
    }

    /* Query by semantic similarity */
    printf("\nQuerying for 'revenue'...\n");
    float query[768] = {0};
    query[0] = 1.0f;

    struct cog_memory_result *results = NULL;
    size_t count = 0;
    ret = cog_memory_attend(query, 768, &results, &count);
    if (ret == COG_OK && count > 0) {
        printf("  Found %lu results\n", count);
        for (size_t i = 0; i < count && i < 5; i++) {
            printf("    [%lu] similarity=%.3f size=%lu\n",
                   results[i].object_id, results[i].similarity,
                   results[i].size_bytes);
        }
    }

    /* Forget a document */
    printf("\nForgetting document %lu...\n", id1);
    ret = cog_memory_forget(id1);
    if (ret == COG_OK) {
        printf("  Forgotten\n");
    }

    printf("\nDone!\n");
    return 0;
}

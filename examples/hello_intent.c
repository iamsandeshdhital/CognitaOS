/**
 * @file hello_intent.c
 * @brief Minimal CognitaOS intent example
 */

#include <cognita/os.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    printf("CognitaOS - Hello Intent Example\n");
    printf("================================\n\n");

    const char *intent_text = "Hello, CognitaOS!";

    struct cog_intent_expr expr = {
        .ie_type_uri = "cognita/HelloWorld/v1",
        .ie_body = (void *)intent_text,
        .ie_body_len = strlen(intent_text),
        .ie_deadline_ns = 1000000000,
        .ie_max_cost_usd = 100,
        .ie_modality = COG_MODALITY_TEXT,
    };

    printf("Submitting intent: \"%s\"\n", intent_text);

    struct cog_intent_result result;
    int ret = cog_intent(&expr, &result);

    if (ret == COG_OK) {
        printf("\nIntent completed successfully!\n");
        printf("  Intent ID: %lu\n", result.ir_intent_id);
        printf("  Execution time: %lu us\n", result.ir_execution_time_us);
        printf("  Cost: $%.6f\n", result.ir_cost_usd / 1000000.0f);
        printf("  Energy: %lu nJ\n", result.ir_energy_joules);
    } else {
        printf("\nIntent failed with error: %d (%s)\n", ret, cog_strerror(ret));
    }

    return ret;
}

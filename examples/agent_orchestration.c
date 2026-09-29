/**
 * @file agent_orchestration.c
 * @brief Multi-agent orchestration example
 */

#include <cognita/os.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    printf("CognitaOS - Agent Orchestration Example\n");
    printf("=======================================\n\n");

    /* Spawn a researcher agent */
    struct cog_agent_handle *researcher = NULL;
    int ret = cog_agent_spawn("researcher",
                              COG_CAP_MEM_WRITE | COG_CAP_TOOL_INVOKE | COG_CAP_STORE_WRITE,
                              &researcher);
    if (ret != COG_OK) {
        printf("Failed to spawn researcher: %s\n", cog_strerror(ret));
        return ret;
    }
    printf("Spawned researcher agent\n");

    /* Spawn a communicator agent */
    struct cog_agent_handle *communicator = NULL;
    ret = cog_agent_spawn("communicator",
                          COG_CAP_TOOL_INVOKE,
                          &communicator);
    if (ret != COG_OK) {
        printf("Failed to spawn communicator: %s\n", cog_strerror(ret));
        return ret;
    }
    printf("Spawned communicator agent\n");

    /* Send task to researcher */
    printf("\nSending task to researcher...\n");
    struct cog_intent_result result;
    ret = cog_agent_send(researcher,
                         "Find and summarize Q3 financial reports",
                         &result);
    if (ret == COG_OK) {
        printf("Researcher completed in %lu us\n", result.ir_execution_time_us);
    }

    /* Send task to communicator */
    printf("\nSending task to communicator...\n");
    ret = cog_agent_send(communicator,
                         "Email the summary to the CFO",
                         &result);
    if (ret == COG_OK) {
        printf("Communicator completed in %lu us\n", result.ir_execution_time_us);
    }

    /* Put agents to sleep */
    printf("\nPutting agents to sleep...\n");
    cog_agent_sleep(researcher);
    cog_agent_sleep(communicator);

    printf("\nDone!\n");
    return 0;
}

#include "common.h"
#include "stack.h"
#include "processor.h"

int main() {
    stack_t stack = {};
    processor_t processor = {};
    initStack(&stack, 10);

    /*
    int commands[MAX_COMMANDS] = {};
    size_t commandsCount = 0;
    error_t callResult = parseCommands(BYTECODE_PR_PATH, commands, &commandsCount);
    if(callResult != SUCCESS) {
        stackDestroy(&stack);
        return callResult;
    }
    */

    int commands[MAX_COMMANDS] = {};
    size_t commandsCount = 0;
    error_t callResult = parseCommands(BYTECODE_PATH, commands, &commandsCount);
    if(callResult != SUCCESS) {
        stackDestroy(&stack);
        return callResult;
    }

    initProcessor(&processor, &stack, commands, commandsCount);

    runCmnds(&processor);

    destroyProcessor(&processor);
}

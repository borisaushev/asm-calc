#include "common.h"
#include "stack.h"
#include "processor.h"

int main() {
    stack_t valuesStack = {};
    initStack(&valuesStack, 10);
    stack_t callStack = {};
    initStack(&callStack, 10);

    processor_t processor = {};
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
        stackDestroy(&valuesStack);
        return callResult;
    }

    initProcessor(&processor, &valuesStack, commands, commandsCount, &callStack);

    runCmnds(&processor);

    destroyProcessor(&processor);
}

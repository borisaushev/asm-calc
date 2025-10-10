#include "common.h"
#include "stack.h"
#include "processor.h"

/*
int main() {
    stack_t stack = {};
    processor_t processor = {};
    initStack(&stack, 10);

    int commands[MAX_COMMANDS] = {};

    size_t commandsCount = 0;
    error_t callResult = parseCommands(BYTECODE_PR_PATH, commands, &commandsCount);
    if(callResult != SUCCESS) {
        stackDestroy(&stack);
        return callResult;
    }

    assert(commands);
    DPRINTF("parsed commands: \n")
    for (size_t i = 0; i < commandsCount; i++) {
        DPRINTF("\t%i\n", commands[i]);
    }

    initProcessor(&processor, &stack, commands, commandsCount);

    runCmnds(&processor);

    destroyProcessor(&processor);
}
*/



int main() {
    stack_t stack = {};

    initStack(&stack, -5);
    printStack(&stack);

    // initStack(&stack, 5);
    // element_t fake_cnr1 = CANARRAY;
    //
    // stack.cnr1 = &fake_cnr1;
    // *(stack.array - 1) = 10;
    //
    // printStack(&stack);
    //
    // stackDestroy(&stack);
}

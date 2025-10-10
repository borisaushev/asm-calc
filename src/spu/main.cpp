#include "common.h"
#include "stack.h"
#include "processor.h"

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



/*
#include "common.h"
#include "stack.h"
#include "processor.h"

int main() {
    stack_t stack = {};
    // processor_t processor = {};

    // initStack(&stack, -5);

    // printStack(&stack);
    // stack.elementCount = stack.capacity / 2;
    // *(stack.cnr1-2) = NULL;
    // *(stack.cnr1-1) = NULL;
    // stackDestroy(&stack);
    // DPRINTF("Stack destroyed\n");

    initStack(&stack, 5);

    stackPush(&stack, 5);

    int value = 0;
    stackPop(&stack, &value);
    printf("value = %d\n", value);

    // stack.array = (element_t*) 100000000;
    // stack.capacity = MAX_REASONABLE_CAPACITY - 2;
    // stack.elementCount = MAX_REASONABLE_CAPACITY - 1;
    // stackPush(&stack, 10);
    // stackPush(&stack, 10);
    element_t* fake_cnr1 = (element_t*)malloc(sizeof(element_t));
    *fake_cnr1 = CANARRAY;

    stack.cnr1 = fake_cnr1;
    *(stack.array - 1) = 10;
    stackPop(&stack, &value);

    printf("value = %d\n", value);
    stackPop(&stack, &value);
    printf("value = %d\n", value);

    printStack(&stack);

    stackDestroy(&stack);

    // int commands[MAX_COMMANDS] = {};
    //
    // size_t commandsCount = 0;
    // error_info_t callResult = parseInts(BYTECODE_PR_PATH, commands, &commandsCount);
    // if(callResult.err_code != SUCCESS) {
    //     PRINTERR("ERROR [%s:%d]: %s (code %d)\n", callResult.file, callResult.line, callResult.msg, callResult.err_code);
    //     stackDestroy(&stack);
    //     return callResult.err_code;
    // }
    //
    // assert(commands);
    // DPRINTF("parsed commands: \n")
    // for (size_t i = 0; i < commandsCount; i++) {
    //     DPRINTF("\t%i\n", commands[i]);
    // }
    //
    // initProcessor(&processor, &stack, commands, commandsCount);
    //
    // runCmnds(&processor);
    //
    // destroyProcessor(&processor);
}
*/
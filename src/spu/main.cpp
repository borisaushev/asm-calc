#include "common.h"
#include "stack.h"
#include "processor.h"

int main() {
    stack_t stack = {};
    processor_t processor = {};
    initStack(&stack, 10);

    int commands[MAX_COMMANDS] = {};

    size_t commandsCount = 0;
    error_info_t callResult = parseInts(BYTECODE_PR_PATH, commands, &commandsCount);
    if(callResult.err_code != SUCCESS) {
        PRINTERR("ERROR [%s:%d]: %s (code %d)\n", callResult.file, callResult.line, callResult.msg, callResult.err_code);
        stackDestroy(&stack);
        return callResult.err_code;
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

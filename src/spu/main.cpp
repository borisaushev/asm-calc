#include "common.h"
#include "stack.h"
#include "processor.h"

int main() {
    stack_t stack = {};
    initStack(&stack, 10);

    int count = -1;
    int* commands = {};
    SAFE_CALL(parseInts(BYTECODE_PR_PATH, &commands, &count));
    assert(commands);
    assert(count >= 0);

    DPRINTF("parsed commands: \n")
    for (int i = 0; i < count; i++) {
        DPRINTF("\t%i\n", commands[i]);
    }

    runCmnds(&stack, count, commands);

    free(commands);
    stackDestroy(&stack);
}

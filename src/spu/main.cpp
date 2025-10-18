#include "common.h"
#include "stack.h"
#include "processor.h"

int main() {
    stack_t valuesStack = {};
    initStack(&valuesStack, STACK_BASE_SIZE);

    stack_t callStack = {};
    initStack(&callStack, STACK_BASE_SIZE);

    processor_t processor = {};
    initProcessor(&processor, &valuesStack, &callStack);

    error_t callResult = parseCommands(BYTECODE_PATH, &processor);
    if(callResult != SUCCESS) {
        destroyProcessor(&processor);
        return callResult;
    }

    callResult = verifyProcessor(&processor);
    if (callResult != SUCCESS) {
        #ifdef DEBUG
            dumpProcessor(&processor);
            PRINTERR("Error while parsing commands");
        #endif

        destroyProcessor(&processor);
        return callResult;
    }

    runCmnds(&processor);

    destroyProcessor(&processor);
}

//TODO

#if 0

#include "common.h"
#include "stack.h"
#include "processor.h"

int main() {
stack_t valuesStack = {};
initStack(&valuesStack, STACK_BASE_SIZE);

stack_t callStack = {};
initStack(&callStack, STACK_BASE_SIZE);

processor_t processor = {};
initProcessor(&processor, &valuesStack, &callStack);

__TRY {
error_t callResult = parseCommands(BYTECODE_PATH, &processor);
if(callResult != SUCCESS) __FAIL (1);

callResult = verifyProcessor(&processor);
if (callResult != SUCCESS) __FAIL (2);

runCmnds(&processor);
}

__CLEANUP {
destroyProcessor(&processor);
}

return __RESULT;

}
#endif

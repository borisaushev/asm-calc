#include "processor_str.h"

#include "processor.h"



error_info_t initProcessor(processor_t* processor, stack_t* stack, int commands[MAX_COMMANDS], size_t commandsCount) {
    assert(processor);
    assert(commands);
    assert(stack);

    STACK_VALID(stack);
    processor->stack = stack;
    for (size_t i = 0; i < commandsCount; i++) {
        processor->commands[i] = commands[i];
    }
    for (size_t i = commandsCount; i < MAX_COMMANDS; i++) {
        processor->commands[i] = 0;
    }
    processor->curI = 0;
    for (int i = 0; i < REGISTER_SIZE; i++) {
        processor->registerArr[i] = POISON;
    }
    processor->commandsCount = commandsCount;

    return {SUCCESS};
}

error_info_t verifyProcessor(processor_t* processor) {
    assert(processor);

    if (processor->stack == NULL) {
        RETURN_ERR(NULL_PTR, "stack ptr is null");
    }
    STACK_VALID(processor->stack);

    for (int i = 0; i < MAX_COMMANDS; i++) {
        if (processor->commands[i] < 0 || processor->commands[i] > MAX_COMMANDS) {
            RETURN_ERR(NULL_PTR, "command index out of range");
        }
    }
    if (processor->curI > MAX_COMMANDS) {
        RETURN_ERR(NULL_PTR, "command index out of range");
    }
    if (processor->commandsCount > MAX_COMMANDS) {
        RETURN_ERR(NULL_PTR, "reasonable commands count exceeded");
    }

    return {SUCCESS};
}

void printProcessor(processor_t *processor, FILE* dumpFile) {
    fprintf(dumpFile, "Processor dump:\n");
    fprintf(dumpFile, "curI: %llu, commands count: %llu\n", processor->curI, processor->commandsCount);

    fprintf(dumpFile, "commands: ");
    for (size_t i = 0; i < processor->commandsCount; i++) {
        if (i == processor->curI) {
            fprintf(dumpFile, "(%d) ", processor->commands[i]);
        }
        else {
            fprintf(dumpFile, "%d ", processor->commands[i]);
        }
    }
    fprintf(dumpFile, "\n");

    fprintf(dumpFile, "register arr: ");
    for (int i = 0; i < REGISTER_SIZE; i++) {
        if (processor->registerArr[i] != POISON) {
            fprintf(dumpFile, "[%i]: %d, ", i, processor->registerArr[i]);
        }
    }
    fprintf(dumpFile, "\n");

    stackDump(processor->stack, __FILE__, __LINE__, __FUNCTION__, validateStack(processor->stack));
    fprintStack(dumpFile, processor->stack);

    fflush(dumpFile);
}

void DPrintProcessor(processor_t *processor) {
    DPRINTF("Processor dump:\n");
    DPRINTF("curI: %llu, commands count: %llu\n", processor->curI, processor->commandsCount);

    DPRINTF("commands: [");
    for (size_t i = 0; i < processor->commandsCount; i++) {
        if (i == processor->curI) {
            DPRINTF("(%d), ", processor->commands[i]);
        }
        else {
            DPRINTF("%d, ", processor->commands[i]);
        }
    }
    DPRINTF("]\n");

    DPRINTF("register arr: ");
    for (int i = 0; i < REGISTER_SIZE; i++) {
        if (processor->registerArr[i] != POISON) {
            DPRINTF("[%i]: %d,  ", i, processor->registerArr[i]);
        }
    }
    DPRINTF("\n");

    stackDump(processor->stack, __FILE__, __LINE__, __FUNCTION__, validateStack(processor->stack));
    DPrintStack(processor->stack);
}

void dumpProcessor(processor_t* processor) {
    assert(processor);

    static FILE* dumpFile = NULL;
    if (dumpFile == NULL) {
        dumpFile = fopen(PROC_LOG_FILE, "w");
    }
    printProcessor(processor, dumpFile);
}

void destroyProcessor(processor_t* processor) {
    assert(processor);
    assert(processor->stack);
    stackDestroy(processor->stack);
}


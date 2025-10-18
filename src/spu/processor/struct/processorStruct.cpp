#include "processorStruct.h"

#include "processor.h"


error_t initProcessor(processor_t* processor) {
    assert(processor);

    initStack(&processor->valuesStack, STACK_BASE_SIZE);
    initStack(&processor->callStack, STACK_BASE_SIZE);

    processor->CP = 0;

    for (int i = 0; i < REGISTER_SIZE; i++) {
        processor->registerArr[i] = POISON;
    }
    for (int i = 0; i < MAX_COMMANDS; i++) {
        processor->commands[i] = POISON;
    }
    processor->commandsCount = 0;

    processor->RAM = (int*) calloc(RAM_SIZE, sizeof(int));

    return SUCCESS;
}

error_t verifyProcessor(processor_t* processor) {
    if (processor == NULL) {
        RETURN_ERR(NULL_PTR, "processor is NULL");
    }

    dumpProcessor(processor);
    STACK_VALID(&processor->valuesStack);
    STACK_VALID(&processor->callStack);

    if (processor->RAM == NULL) {
        RETURN_ERR(NULL_PTR, "ram pointer is null");
    }

    if (processor->commandsCount > MAX_COMMANDS) {
        RETURN_ERR(NULL_PTR, "reasonable commands count exceeded");
    }
    if (processor->CP > MAX_COMMANDS) {
        RETURN_ERR(NULL_PTR, "current command index out of range");
    }

    return SUCCESS;
}

void dumpProcessor(processor_t* processor, FILE* dumpFile) {
    assert(processor);

    fprintf(dumpFile, "Processor dump:\n");
    fprintf(dumpFile, "curI: %llu, commands count: %llu\n", processor->CP, processor->commandsCount);

    fprintf(dumpFile, "commands: ");
    for (size_t i = 0; i < processor->commandsCount; i++) {
        if (i == processor->CP) {
            fprintf(dumpFile, "(%d) ", processor->commands[i]);
        }
        else {
            fprintf(dumpFile, "%d ", processor->commands[i]);
        }
    }
    fprintf(dumpFile, "\n");

    fprintf(dumpFile, "register arr: ");
    int count = 0;
    for (int i = 0; i < REGISTER_SIZE; i++) {
        if (processor->registerArr[i] != POISON) {
            fprintf(dumpFile, "[%i]: %d, ", i, processor->registerArr[i]);
            count++;
        }
    }
    if (count == 0) {
        fprintf(dumpFile, "[empty]");
    }
    fprintf(dumpFile, "\n");

    fprintf(dumpFile, "values stack:\n");
    stackDumpStream(&processor->valuesStack, validateStack(&processor->valuesStack), dumpFile,
        __FILE__, __LINE__, __FUNCTION__);

    fprintf(dumpFile, "call stack:\n");
    stackDumpStream(&processor->callStack, validateStack(&processor->callStack), dumpFile,
        __FILE__, __LINE__, __FUNCTION__);

    fprintf(dumpFile, "ram:\n");
    fprintf(dumpFile, "{ ");
    for (int i = 0; i < RAM_SIZE; i++) {
        fprintf(dumpFile, "[%i]: %d, ", i, processor->RAM[i]);
    }
    fprintf(dumpFile, " }\n");

    fflush(dumpFile);
}

void DPrintProcessor(processor_t* processor) {
    assert(processor);

    DPRINTF("Processor:\n");
    DPRINTF("curI: %llu, commands count: %llu\n", processor->CP, processor->commandsCount);

    DPRINTF("commands: [");
    for (size_t i = 0; i < processor->commandsCount; i++) {
        if (i == processor->CP) {
            DPRINTF("(%d), ", processor->commands[i]);
        }
        else {
            DPRINTF("%d, ", processor->commands[i]);
        }
    }
    DPRINTF("]\n");

    DPRINTF("register arr: ");
    int count = 0;
    for (int i = 0; i < REGISTER_SIZE; i++) {
        if (processor->registerArr[i] != POISON) {
            DPRINTF("[%i]: %d, ", i, processor->registerArr[i]);
            count++;
        }
    }
    if (count == 0) {
        DPRINTF("[empty]");
    }
    DPRINTF("\n");

    DPRINTF("values stack:\n");
    DPrintStack(&processor->valuesStack);

    DPRINTF("call stack:\n");
    DPrintStack(&processor->callStack);
}

void dumpProcessor(processor_t* processor) {
    assert(processor);
    static FILE* dumpFile = NULL;
    if (dumpFile == NULL) {
        dumpFile = fopen(PROC_LOG_FILE, "wa");
    }
    dumpProcessor(processor, dumpFile);
}

void destroyProcessor(processor_t* processor) {
    assert(processor);
    assert(processor->RAM);

    stackDestroy(&processor->valuesStack);
    stackDestroy(&processor->callStack);
    free(processor->RAM);
}


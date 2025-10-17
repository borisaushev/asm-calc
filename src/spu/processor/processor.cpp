#include "processor.h"

#include "compiler.h"
#include "line_reader.h"
#include "stack.h"
#include "commands/processorСommands.h"
#include "struct/processorStruct.h"

error_t parseCommands(const char* filename, processor_t* processor) {
    assert(filename);

    FILE* file = fopen(filename, "r");
    if (!file) {
        RETURN_ERR(FILE_NOT_READABLE, "could not open file");
    }

    int signatura = -1, version = -1;
    fread(&signatura, sizeof(int), 1, file);
    fread(&version, sizeof(int), 1, file);
    if (signatura != SIGNATURA_BYTE) {
        RETURN_ERR(INVALID_INPUT, "INVALID BYTE CODE SIGNATURE");
    }
    if (version != VERSION) {
        RETURN_ERR(INVALID_INPUT, "INVALID BYTE CODE VERSION");
    }

    processor->commandsCount = fread(processor->commands, sizeof(int), MAX_COMMANDS, file);
    if (processor->commandsCount > MAX_COMMANDS) {
        RETURN_ERR(INVALID_INPUT, "MAX COMMANDS NUMBER EXCEEDED");
    }

    fclose(file);
    return SUCCESS;
}

static error_t runCommand(processor_t* processor, int *found, int *stopped) {
    SAFE_CALL(verifyProcessor(processor));

    *found = 0;
    *stopped = 0;
    for (int i = 0; i < COMMANDS_COUNT; i++) {
        processorCmdInfo_t curCommand = PROCESSOR_COMMANDS_INFO[i];
        if (processor->commands[processor->CP] == curCommand.command) {
            *found = 1;
            if (curCommand.command == HLT) {
                *stopped = 1;
                return SUCCESS;
            }
            SAFE_CALL(curCommand.func(processor));
            break;
        }
    }

    return SUCCESS;
}

error_t runCmnds(processor_t* processor) {
    SAFE_CALL(verifyProcessor(processor));

    #ifdef DEBUG
        DPrintProcessor(processor);
        SAFE_CALL(verifyProcessor(processor));
    #endif

    int curCmnd = -1;
    int line = 1;
    for (; curCmnd != HLT && processor->CP < processor->commandsCount; (processor->CP)++, line++) {
        curCmnd = processor->commands[processor->CP];
        int found = 0;
        int stopped = 0;

        error_t result = runCommand(processor, &found, &stopped);
        if (result != SUCCESS) {
            PRINTERR("'%d' at index:%d\n", curCmnd, processor->CP);
            return result;
        }

        if (!found) {
            PRINTERR("UNKNOWN COMMAND '%d' at %s:%d\n", curCmnd, ASM_SRC_PATH, line + 1);
            RETURN_ERR(INVALID_INPUT, "unknown command");
        }
        if (stopped) {
            break;
        }

        #ifdef DEBUG
            dumpProcessor(processor);
            SAFE_CALL(verifyProcessor(processor));
            DPrintProcessor(processor);
            getchar();
        #endif
    }

    printf("end of the run\n");

    return SUCCESS;
}

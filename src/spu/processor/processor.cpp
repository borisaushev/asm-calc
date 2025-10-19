#include "processor.h"

#include "compiler.h"
#include "line_reader.h"
#include "stack.h"
#include "commands/processorCommands.h"
#include "struct/processorStruct.h"

error_t parseCommands(const char* filename, processor_t* processor) {
    assert(filename);

    FILE* file = fopen(filename, "r");
    _TX_TRY {
        if (!file) {
            FAIL_ERR(FILE_NOT_READABLE, "COULD NOT OPEN FILE");
        }

        int signatura = -1, version = -1;
        fread(&signatura, sizeof(int), 1, file);
        fread(&version, sizeof(int), 1, file);
        if (signatura != SIGNATURA_BYTE) {
            FAIL_ERR(INVALID_INPUT, "INVALID BYTE CODE SIGNATURE");
        }
        if (version != VERSION) {
            FAIL_ERR(INVALID_INPUT, "INVALID BYTE CODE VERSION");
        }

        processor->commandsCount = fread(processor->commands, sizeof(int), MAX_COMMANDS, file);
        if (processor->commandsCount > MAX_COMMANDS) {
            FAIL_ERR(INVALID_INPUT, "MAX COMMANDS NUMBER EXCEEDED");
        }
    } _TX_ENDTRY
    _TX_CATCH {
    }
    _TX_FINALLY {
        fclose(file);
    }

    return SUCCESS;
}

static error_t runCommand(processor_t* processor, int *stopped) {
    #ifdef DEBUG
        SAFE_CALL(verifyProcessor(processor));
    #endif

    if (processor->commands[processor->CP] < 0 || processor->commands[processor->CP] >= MAX_COMMANDS) {
        RETURN_ERR(INVALID_INPUT, "invalid command number");
    }

    *stopped = 0;
    processorCmdInfo_t curCommand = PROCESSOR_COMMANDS_INFO[processor->commands[processor->CP]];
    if (curCommand.command == HLT) {
        *stopped = 1;
        return SUCCESS;
    }
    SAFE_CALL(curCommand.function(processor));

    return SUCCESS;
}

error_t verifyProcessorCommandsArray() {
    for (int i = 0; i < COMMANDS_COUNT; i++) {
        if (PROCESSOR_COMMANDS_INFO[i].command != i) {
            RETURN_ERR(INVALID_INPUT, "processor commands array is invalid");
        }
    }

    return SUCCESS;
}

error_t runCommands(processor_t* processor) {
    SAFE_CALL(verifyProcessorCommandsArray());

    #ifdef DEBUG
        SAFE_CALL(verifyProcessor(processor));
        DPrintProcessor(processor);
    #endif

    int curCommand = -1;
    for (; curCommand != HLT && processor->CP < processor->commandsCount; (processor->CP)++) {
        curCommand = processor->commands[processor->CP];
        int stopped = 0;

        error_t result = runCommand(processor, &stopped);
        if (result != SUCCESS) {
            PRINTERR("'%d' at index:%llu\n", curCommand, processor->CP);
            return result;
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

#include "processor.h"

#include "line_reader.h"
#include "stack.h"
#include "commands/commands.h"
#include "struct/processorStruct.h"

error_t parseCommands(const char* filename, int arr[MAX_COMMANDS], size_t* commandsCount) {
    assert(filename);
    assert(arr);
    assert(commandsCount);


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

    *commandsCount = fread(arr, sizeof(int), MAX_COMMANDS + 2, file);

    fclose(file);
    return SUCCESS;
}

error_t runCmnds(processor_t* processor) {
    assert(processor);
    assert(processor->valuesStack);

#ifdef DEBUG
    DPrintProcessor(processor);
    SAFE_CALL(verifyProcessor(processor));
#endif

    int curCmnd = -1;
    int line = 1;
    for (; curCmnd != HLT && processor->CP < MAX_COMMANDS && processor->CP < processor->commandsCount; (processor->CP)++, line++) {
        curCmnd = processor->commands[processor->CP];

        switch (curCmnd) {
            case ADD: {
                SAFE_CALL(spuAdd(processor))
                break;
            }
            case SUB: {
                SAFE_CALL(spuSub(processor))
                break;
            }
            case MUL: {
                SAFE_CALL(spuMul(processor))
                break;
            }
            case DIV: {
                SAFE_CALL(spuDiv(processor));
                break;
            }
            case SQRT: {
                SAFE_CALL(spuSqrt(processor));
                break;
            }
            case OUT: {
                SAFE_CALL(spuOut(processor));
                break;
            }
            case PUSH: {
                SAFE_CALL(spuPush(processor));
                break;
            }
            case IN: {
                SAFE_CALL(spuIn(processor));
                break;
            }
            case PUSHREG: {
                SAFE_CALL(spuPushReg(processor));
                break;
            }
            case POPREG: {
                SAFE_CALL(spuPopReg(processor));
                break;
            }
            case JMP: {
                SAFE_CALL(spuJmp(processor));
                break;
            }
            case JB: {
                SAFE_CALL(spuJb(processor));
                break;
            }
            case JBE: {
                SAFE_CALL(spuJbe(processor));
                break;
            }
            case JA: {
                SAFE_CALL(spuJa(processor));
                break;
            }
            case JAE: {
                SAFE_CALL(spuJae(processor));
                break;
            }
            case JE: {
                SAFE_CALL(spuJe(processor));
                break;
            }
            case JNE: {
                SAFE_CALL(spuJne(processor));
                break;
            }
            case CALL: {
                SAFE_CALL(spuCall(processor));
                break;
            }
            case RET: {
                SAFE_CALL(spuRet(processor));
                break;
            }
            case HLT: {
                break;
            }
            default: {
                PRINTERR("UNKNOWN COMMAND '%d' at %s:%d\n", curCmnd, ASM_SRC_PATH, line + 1);
                RETURN_ERR(INVALID_INPUT, "unknown command");
            }
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

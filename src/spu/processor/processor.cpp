#include "processor.h"

#include "line_reader.h"
#include "stack.h"
#include "struct/processorStruct.h"

//TODO mb fix lambdas and replace with defines
error_info_t verifySignature(FILE *file) {
    assert(file);
    char buf[10] = {};
    if (fscanf(file, "%s", buf) != 1 || strcmp(buf, SIGNATURA) != 0) {
        RETURN_ERR(INVALID_INPUT, "INVALID BYTE CODE SIGNATURE");
    }
    int version = -1;
    if (fscanf(file, " V: %d\n", &version) != 1 || version != VERSION) {
        RETURN_ERR(INVALID_INPUT, "INVALID BYTE CODE VERSION");
    }

    return {SUCCESS};
}

error_info_t parseInts(const char* filename, int arr[MAX_COMMANDS], size_t* commandsCount) {
    assert(filename);
    assert(arr);

    FILE* file = fopen(filename, "r");
    if (!file) {
        RETURN_ERR(FILE_NOT_READABLE, "could not open file");
    }

    SAFE_CALL(verifySignature(file));

    size_t count = 0;
    int num = -1;
    while (fscanf(file, "%d", &num) == 1) {
        if (count > MAX_COMMANDS) {
            return {INVALID_INPUT, "TOO MUCH COMMANDS!!!!"};
        }
        arr[count++] = num;
    }
    *commandsCount = count;

    fclose(file);
    return {SUCCESS};
}

error_info_t funcOfTwo(stack_t* stack, int (*func) (int a, int b), const char* const funcName) {
    assert(stack);
    assert(func);
    assert(funcName);

    int v1 = POISON, v2 = POISON;
    SAFE_CALL(stackPop(stack, &v1));
    SAFE_CALL(stackPop(stack, &v2));
    if (v1 == POISON || v2 == POISON) {
        RETURN_ERR(INVALID_INPUT, "stack is empty");
    }

    int val = func(v1, v2);
    printf("%s", funcName);
    printf(": %d\n", val);
    SAFE_CALL(stackPush(stack, val));

    return {SUCCESS};
}

error_info_t runCmnds(processor_t* processor) {
    assert(processor);
    assert(processor->stack);
    verifyProcessor(processor);

    int curCmnd = -1;
    int line = 1;
    for (; curCmnd != HLT && processor->curI < MAX_COMMANDS && processor->curI < processor->commandsCount; (processor->curI)++, line++) {

        curCmnd = processor->commands[processor->curI];

        #ifdef DEBUG
            dumpProcessor(processor);
            DPrintProcessor(processor);
        #endif

        switch (curCmnd) {
            case ADD: {
                funcOfTwo(processor->stack, [] (int a, int b) -> int { return a + b; }, "ADD");
                break;
            }
            case SUB: {
                funcOfTwo(processor->stack, [] (int a, int b) -> int { return b - a; }, "SUB");
                break;
            }
            case MUL: {
                funcOfTwo(processor->stack, [] (int a, int b) -> int { return a * b; }, "MUL");
                break;
            }
            case DIV: {
                int v1 = POISON, v2 = POISON;
                SAFE_CALL(stackPop(processor->stack, &v1));
                SAFE_CALL(stackPop(processor->stack, &v2));

                int val = v2 / v1;
                if (v1 == 0) {
                    RETURN_ERR(INVALID_INPUT, "division by zero");
                }
                printf("DIV: %d\n", val);
                SAFE_CALL(stackPush(processor->stack, val));
                break;
            }
            case OUT: {
                int v = POISON;
                SAFE_CALL(stackPop(processor->stack, &v));

                printf("OUT: %d\n", v);
                break;
            }
            case PUSH: {
                int v = processor->commands[++processor->curI];
                SAFE_CALL(stackPush(processor->stack, v));

                printf("PUSH: %d\n", v);
                break;
            }
            case PUSHREG: {
                int reg = processor->commands[++processor->curI];
                if (reg >= REGISTER_SIZE || reg < 0) {
                    RETURN_ERR(INVALID_INPUT, "reg value out of range");
                }
                if (processor->registerArr[reg] == POISON) {
                    RETURN_ERR(INVALID_INPUT, "register is not initialized");
                }
                SAFE_CALL(stackPush(processor->stack, processor->registerArr[reg]));
                printf("PUSHREG: reg: %d, val: %d\n", reg, processor->registerArr[reg]);
                break;
            }
            case POPREG: {
                int reg = processor->commands[++processor->curI];
                if (reg >= REGISTER_SIZE || reg < 0) {
                    RETURN_ERR(INVALID_INPUT, "reg value out of range");
                }

                SAFE_CALL(stackPop(processor->stack, &(processor->registerArr[reg])));
                printf("POPREG: reg: %d, val: %d\n", reg, processor->registerArr[reg]);
                break;
            }
            case CP: {
                int reg = processor->commands[++processor->curI];
                if (reg >= REGISTER_SIZE || reg < 0) {
                    RETURN_ERR(INVALID_INPUT, "reg value out of range");
                }

                processor->registerArr[reg] = (int) processor->curI;
                printf("CP: reg: %d, val: %d\n", reg, processor->registerArr[reg]);
                break;
            }
            case JMP: {
                int reg = processor->commands[++processor->curI];
                if (reg >= REGISTER_SIZE || reg < 0) {
                    RETURN_ERR(INVALID_INPUT, "reg value out of range");
                }

                int jmpIndex = processor->registerArr[reg];
                if (jmpIndex < 0 || jmpIndex >= (int) processor->commandsCount) {
                    RETURN_ERR(INVALID_INPUT, "jmp index out of range");
                }

                processor->curI = (size_t) jmpIndex;
                printf("JMP: reg: %d, val: %d\n", reg, jmpIndex);
                break;
            }
            case HLT: {
                break;
            }
            default: {
                PRINTERR("UNKNOWN COMMAND '%d' at %s:%d\n", curCmnd, BYTECODE_PR_PATH, line + 1);
                RETURN_ERR(INVALID_INPUT, "unknown command");
            }
        }

        #ifdef DEBUG
            getchar();
        #endif
    }

    printf("end of the run\n");

    return {SUCCESS};
}
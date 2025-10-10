#include "processor.h"

#include "line_reader.h"
#include "stack.h"
#include "struct/processorStruct.h"

//TODO mb fix lambdas and replace with defines
error_t verifySignature(FILE *file) {
    assert(file);
    char buf[10] = {};
    if (fscanf(file, "%s", buf) != 1 || strcmp(buf, SIGNATURA) != 0) {
        RETURN_ERR(INVALID_INPUT, "INVALID BYTE CODE SIGNATURE");
    }
    int version = -1;
    if (fscanf(file, " V: %d\n", &version) != 1 || version != VERSION) {
        RETURN_ERR(INVALID_INPUT, "INVALID BYTE CODE VERSION");
    }

    return SUCCESS;
}

error_t parseCommands(const char* filename, int arr[MAX_COMMANDS], size_t* commandsCount) {
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
            RETURN_ERR(INVALID_INPUT, "TOO MUCH COMMANDS!!!!");
        }
        arr[count++] = num;
    }
    *commandsCount = count;

    fclose(file);
    return SUCCESS;
}

error_t funcOfTwo(stack_t* stack, int (*func) (int a, int b), const char* const funcName) {
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
    DPRINTF("%s", funcName);
    DPRINTF(": %d\n", val);
    SAFE_CALL(stackPush(stack, val));

    return SUCCESS;
}

error_t getJmpIndexAndVals(processor_t *processor, int* index, int* v1, int* v2) {
    *index = processor->commands[++processor->CP] - 1;
    if (*index < 0 || (size_t) *index >= processor->commandsCount ) {
        RETURN_ERR(INVALID_INPUT, "index out of range");
    }

    *v1 = -1;
    *v2 = -1;
    SAFE_CALL(stackPop(processor->stack, v1));
    SAFE_CALL(stackPop(processor->stack, v2));

    DPRINTF("parsed jmp index: %d, v1: %d, v2: %d\n", *index, *v1, *v2)

    return SUCCESS;
}

error_t runCmnds(processor_t* processor) {
    assert(processor);
    assert(processor->stack);

    DPrintProcessor(processor);
    SAFE_CALL(verifyProcessor(processor));

    int curCmnd = -1;
    int line = 1;
    for (; curCmnd != HLT && processor->CP < MAX_COMMANDS && processor->CP < processor->commandsCount; (processor->CP)++, line++) {
        curCmnd = processor->commands[processor->CP];

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
                DPRINTF("DIV: %d\n", val);
                SAFE_CALL(stackPush(processor->stack, val));
                break;
            }
            case SQRT: {
                int v1 = POISON;
                SAFE_CALL(stackPop(processor->stack, &v1));

                if (v1 < 0) {
                    RETURN_ERR(INVALID_INPUT, "negative number passed to sqrt");
                }
                int val = (int) sqrt(v1);

                DPRINTF("SQRT: %d\n", val);
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
                int v = processor->commands[++processor->CP];
                SAFE_CALL(stackPush(processor->stack, v));

                DPRINTF("PUSH: %d\n", v);
                break;
            }
            case IN: {
                printf("input val: ");
                int inp = POISON;
                if (scanf("%d", &inp) != 1) {
                    RETURN_ERR(INVALID_INPUT, "invalid input");
                }
                printf("\n");

                DPRINTF("INPUT: %d\n", inp);
                SAFE_CALL(stackPush(processor->stack, inp));
                break;
            }
            case PUSHREG: {
                int reg = processor->commands[++processor->CP];
                if (reg >= REGISTER_SIZE || reg < 0) {
                    RETURN_ERR(INVALID_INPUT, "reg value out of range");
                }
                if (processor->registerArr[reg] == POISON) {
                    RETURN_ERR(INVALID_INPUT, "register is not initialized");
                }
                SAFE_CALL(stackPush(processor->stack, processor->registerArr[reg]));
                DPRINTF("PUSHREG: reg: %d, val: %d\n", reg, processor->registerArr[reg]);
                break;
            }
            case POPREG: {
                int reg = processor->commands[++processor->CP];
                if (reg >= REGISTER_SIZE || reg < 0) {
                    RETURN_ERR(INVALID_INPUT, "reg value out of range");
                }

                SAFE_CALL(stackPop(processor->stack, &(processor->registerArr[reg])));
                DPRINTF("POPREG: reg: %d, val: %d\n", reg, processor->registerArr[reg]);
                break;
            }
            case CP: {
                int reg = processor->commands[++processor->CP];
                if (reg >= REGISTER_SIZE || reg < 0) {
                    RETURN_ERR(INVALID_INPUT, "reg value out of range");
                }

                processor->registerArr[reg] = (int) processor->CP;
                DPRINTF("CP: reg: %d, val: %d\n", reg, processor->registerArr[reg]);
                break;
            }
            case JMP: {
                int reg = processor->commands[++processor->CP];
                if (reg >= REGISTER_SIZE || reg < 0) {
                    RETURN_ERR(INVALID_INPUT, "reg value out of range");
                }

                int jmpIndex = processor->registerArr[reg];
                if (jmpIndex < 0 || jmpIndex >= (int) processor->commandsCount) {
                    RETURN_ERR(INVALID_INPUT, "jmp index out of range");
                }

                processor->CP = (size_t) jmpIndex;
                DPRINTF("JMP: reg: %d, val: %d\n", reg, jmpIndex);
                break;
            }
            case JB: {
                int index = -1, v1 = -1, v2 = -1;
                SAFE_CALL(getJmpIndexAndVals(processor, &index, &v1, &v2));

                if (v2 < v1) {
                    DPRINTF("Jump form: %llu, to %d\n", processor->CP, index);
                    processor->CP = (size_t) index;
                }
                DPRINTF("JB: index: %d, v1: %d, v2: %d\n", index, v1, v2);
                break;
            }
            case JBE: {
                int index = -1, v1 = -1, v2 = -1;
                SAFE_CALL(getJmpIndexAndVals(processor, &index, &v1, &v2));

                if (v2 <= v1) {
                    DPRINTF("Jump form: %llu, to %d\n", processor->CP, index);
                    processor->CP = (size_t) index;
                }
                DPRINTF("JBE: index: %d, v1: %d, v2: %d\n", index, v1, v2);
                break;
            }
            case JA: {
                int index = -1, v1 = -1, v2 = -1;
                SAFE_CALL(getJmpIndexAndVals(processor, &index, &v1, &v2));

                if (v2 > v1) {
                    DPRINTF("Jump form: %llu, to %d\n", processor->CP, index);
                    processor->CP = (size_t) index;
                }
                DPRINTF("JA: index: %d, v1: %d, v2: %d\n", index, v1, v2);
                break;
            }
            case JAE: {
                int index = -1, v1 = -1, v2 = -1;
                SAFE_CALL(getJmpIndexAndVals(processor, &index, &v1, &v2));

                if (v2 >= v1) {
                    DPRINTF("Jump form: %llu, to %d\n", processor->CP, index);
                    processor->CP = (size_t) index;
                }
                DPRINTF("JAE: index: %d, v1: %d, v2: %d\n", index, v1, v2);
                break;
            }
            case JE: {
                int index = -1, v1 = -1, v2 = -1;
                SAFE_CALL(getJmpIndexAndVals(processor, &index, &v1, &v2));

                if (v2 == v1) {
                    DPRINTF("Jump form: %llu, to %d\n", processor->CP, index);
                    processor->CP = (size_t) index;
                }
                DPRINTF("JE: index: %d, v1: %d, v2: %d\n", index, v1, v2);
                break;
            }
            case JNE: {
                int index = -1, v1 = -1, v2 = -1;
                SAFE_CALL(getJmpIndexAndVals(processor, &index, &v1, &v2));

                if (v2 != v1) {
                    DPRINTF("Jump form: %llu, to %d\n", processor->CP, index);
                    processor->CP = (size_t) index;
                }
                DPRINTF("JNE: index: %d, v1: %d, v2: %d\n", index, v1, v2);
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
            dumpProcessor(processor);
            SAFE_CALL(verifyProcessor(processor));
            DPrintProcessor(processor);
            getchar();
        #endif
    }

    printf("end of the run\n");

    return SUCCESS;
}

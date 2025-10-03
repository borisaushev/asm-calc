#include "processor.h"

#include "line_reader.h"
#include "stack.h"

error_info_t verifySignature(FILE *file) {
    char buf[10] = {};
    if (fscanf(file, "%s", &buf) != 1 || strcmp(buf, SIGNATURA) != 0) {
        return {INVALID_INPUT, "INVALID BYTE CODE SIGNATURE"};
    }
    int version = -1;
    if (fscanf(file, " V: %d\n", &version) != 1 || version != VERSION) {
        return {INVALID_INPUT, "INVALID BYTE CODE VERSION"};
    }

    return {SUCCESS};
}

error_info_t parseInts(const char* filename, int** ptr, int* count) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return {FILE_NOT_READABLE, "could not open file"};
    }

    SAFE_CALL(verifySignature(file));

    *count = 0;
    int num;
    while (fscanf(file, "%d", &num) == 1) {
        *ptr = (int*)realloc(*ptr, (*count + 1) * sizeof(int));
        (*ptr)[(*count)++] = num;
    }

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
        return {INVALID_INPUT, "stack is empty"};
    }

    int val = func(v1, v2);
    printf("%s", funcName);
    printf(": %d\n", val);
    SAFE_CALL(stackPush(stack, val));

    return {SUCCESS};
}

error_info_t runCmnds(stack_t* stack, const int* commands, int count) {
    int i = 0;
    int curCmnd = -1;
    int line = 0;
    while (curCmnd != HLT && i < count) {
        line++;
        curCmnd = commands[i];
        DPRINTF("current command: '%d'\n", curCmnd);
        switch (curCmnd) {
            case ADD: {
                funcOfTwo(stack, [] (int a, int b) -> int { return a + b; }, "ADD");
                break;
            }
            case SUB: {
                funcOfTwo(stack, [] (int a, int b) -> int { return b - a; }, "SUB");
                break;
            }
            case MUL: {
                funcOfTwo(stack, [] (int a, int b) -> int { return a * b; }, "MUL");
                break;
            }
            case DIV: {
                int v1 = POISON, v2 = POISON;
                SAFE_CALL(stackPop(stack, &v1));
                SAFE_CALL(stackPop(stack, &v2));

                int val = v2 / v1;
                if (v1 == 0) {
                    return {INVALID_INPUT, "division by zero"};
                }
                printf("DIV: %d\n", val);
                SAFE_CALL(stackPush(stack, val));
                break;
            }
            case OUT: {
                int v = POISON;
                SAFE_CALL(stackPop(stack, &v));

                printf("OUT: %d\n", v);
                break;
            }
            case PUSH: {
                int v = commands[++i];
                SAFE_CALL(stackPush(stack, v));

                printf("PUSH: %d\n", v);
                break;
            }
            case HLT: {
                break;
            }
            default: {
                PRINTERR("UNKNOWN COMMAND '%d' at %s:%d\n", curCmnd, BYTECODE_PR_PATH, line + 1);
                return {INVALID_INPUT, "unknown command"};
            }
        }
        i++;
        #ifdef DEBUG
            getchar();
        #endif
    }

    printf("end of the run\n");

    return {SUCCESS};
}
#include "compiler.h"

#define PRINT_ASM_LINE_ERR() \
    BEGIN \
    PRINTERR("AT LINE %d\nin file: ", i + 1); \
    PRINTERR(ASM_SRC_PATH); \
    PRINTERR(":%d\n\n", i+1); \
    END

error_t openFiles(FILE *&targetPr, FILE *&targetStreamRough) {
    targetPr = fopen(BYTECODE_PR_PATH, "wb");
    targetStreamRough = fopen(BYTECODE_PATH, "wb");

    if (!targetStreamRough || !targetPr) {
        RETURN_ERR(FILE_NOT_FOUND, "could not open file");
    }

    return SUCCESS;
}

static error_t addCmnd(FILE *targetPr, size_t arrIndex, int* rough, command_t command, char* line, int i) {
    char trash[50] = {};
    if(strlen(line) > 50 || sscanf(line, "%s", trash) == 1) {
        PRINT_ASM_LINE_ERR();
        RETURN_ERR(INVALID_INPUT, "unexpected parameter");
    }

    fprintf(targetPr, "%d\n", command);
    rough[arrIndex] = command;
    return SUCCESS;
}

static error_t getRegVal(int i, char *line, char* regVal) {
    char trash[50] = {};
    int scanCount = sscanf(line, " %cX %s", regVal, trash);
    if (scanCount != 1) {
        PRINT_ASM_LINE_ERR();
        RETURN_ERR(INVALID_INPUT, "invalid register value");
    }

    return SUCCESS;
}

static error_t modifyReg(FILE *targetPr, size_t* arrIndex, int* rough, int i, char *line, command_t command) {
    char regVal = 0;
    SAFE_CALL(getRegVal(i, line, &regVal));
    const int regInt = regVal - 'A';

    DPRINTF("regVal: %c\n", regVal);
    DPRINTF("regInt: %d\n", regInt);
    if (!isalpha(regVal) || regInt >= REGISTER_SIZE || regInt < 0) {
        PRINT_ASM_LINE_ERR();
        RETURN_ERR(INVALID_INPUT, "invalid register value");
    }

    fprintf(targetPr, "%d %d\n", command, regInt);

    rough[*arrIndex] = command;
    *arrIndex += 1;
    rough[*arrIndex] = regInt;

    return SUCCESS;
}

static error_t pushCmndAndValue(FILE *targetPr, const command_t command, int rough[MAX_COMMANDS],
                                size_t* arrIndex, const int i, const char *line) {
    int pushVal = POISON;

    char trash[50] = {};
    if (sscanf(line, " %d %s", &pushVal, trash) != 1) {
        PRINT_ASM_LINE_ERR();
        RETURN_ERR(INVALID_INPUT, "invalid input value");
    }
    DPRINTF("input value: %d\n", pushVal);

    fprintf(targetPr, "%d %d\n", command, pushVal);

    rough[*arrIndex] = command;
    *arrIndex += 1;
    rough[*arrIndex] = pushVal;

    return SUCCESS;
}

static error_t jump(FILE *targetPr, const command_t command, int rough[MAX_COMMANDS],
                    size_t* arrIndex, const int i, const char *line, int labels[MAX_LABELS]) {
    int jmpVal = POISON;

    char trash[50] = {};
    if (sscanf(line, " %d %s", &jmpVal, trash) == 1 && jmpVal != POISON) {
        DPRINTF("input value: %d\n", jmpVal);

        fprintf(targetPr, "%d %d\n", command, jmpVal);

        rough[*arrIndex] = command;
        *arrIndex += 1;
        rough[*arrIndex] = jmpVal;

        return SUCCESS;
    }

    int label = -1;
    if (sscanf(line, " :%d %s", &label, trash) == 1 && label >= 0 && label <= MAX_LABELS) {
        DPRINTF("label: %d\n", label);
        DPRINTF("label val: %d\n", labels[label]);

        fprintf(targetPr, "%d %d\n", command, labels[label]);

        rough[*arrIndex] = command;
        *arrIndex += 1;
        rough[*arrIndex] = labels[label];

        return SUCCESS;
    }

    PRINT_ASM_LINE_ERR();
    RETURN_ERR(INVALID_INPUT, "invalid input value");
}

char* findFirstSymb(char* str) {
    for (; *str != '\0' && isspace(*str); str++) {
    }
    return str;
}

error_t substituteCommands(pointer_array_buf_t* text, FILE* targetPr, int labels[10], int rough[1024], char cmnd[100], size_t* size) {
    size_t arrIndex = 0;
    for (int i = 0; i < text->lines_count; i++) {
        char* line = strupr(findFirstSymb(text->pointer_arr[i].ptr));
        char* commentIndex = strchr(line, ';'); // коменты ингнорим
        if (commentIndex != NULL) {
            *commentIndex = '\0';
        }
        if (line[0] == '\0') {
            continue;
        }

        int charsRead = -1;
        if (sscanf(line, "%s%n", cmnd, &charsRead) != 1) {
            PRINT_ASM_LINE_ERR();
            RETURN_ERR(INVALID_INPUT, "invalid command");
        }
        DPRINTF("read line[%d]: '%s' and command: '%s'\n", i+1, line, cmnd);
        line = line+charsRead;

        if (strcmp(cmnd, "ADD") == 0) {
            SAFE_CALL(addCmnd(targetPr, arrIndex, rough, ADD, line, i));
        }
        else if (strcmp(cmnd, "SUB") == 0) {
            SAFE_CALL(addCmnd(targetPr, arrIndex, rough, SUB, line, i));
        }
        else if (strcmp(cmnd, "MUL") == 0) {
            SAFE_CALL(addCmnd(targetPr, arrIndex, rough, MUL, line, i));
        }
        else if (strcmp(cmnd, "DIV") == 0) {
            SAFE_CALL(addCmnd(targetPr, arrIndex, rough, DIV, line, i));
        }
        else if (strcmp(cmnd, "SQRT") == 0) {
            SAFE_CALL(addCmnd(targetPr, arrIndex, rough, SQRT, line, i));
        }
        else if (strcmp(cmnd, "OUT") == 0) {
            SAFE_CALL(addCmnd(targetPr, arrIndex, rough, OUT, line, i));
        }
        else if (strcmp(cmnd, "PUSH") == 0) {
            SAFE_CALL(pushCmndAndValue(targetPr, PUSH, rough, &arrIndex, i, line));
        }
        else if (strcmp(cmnd, "IN") == 0) {
            SAFE_CALL(addCmnd(targetPr, arrIndex, rough, IN, line, i));
        }
        else if (strcmp(cmnd, "PUSHREG") == 0) {
            SAFE_CALL(modifyReg(targetPr, &arrIndex, rough, i, line, PUSHREG));
        }
        else if (strcmp(cmnd, "POPREG") == 0) {
            SAFE_CALL(modifyReg(targetPr, &arrIndex, rough, i, line, POPREG));
        }
        else if (strcmp(cmnd, "CP") == 0) {
            SAFE_CALL(modifyReg(targetPr, &arrIndex, rough, i, line, CP));
        }
        else if (strcmp(cmnd, "JMP") == 0) {
            SAFE_CALL(modifyReg(targetPr, &arrIndex, rough, i, line, JMP));
        }
        else if (strcmp(cmnd, "JB") == 0) {
            SAFE_CALL(jump(targetPr, JB, rough, &arrIndex, i, line, labels));
        }
        else if (strcmp(cmnd, "JBE") == 0) {
            SAFE_CALL(jump(targetPr, JBE, rough, &arrIndex, i, line, labels));
        }
        else if (strcmp(cmnd, "JA") == 0) {
            SAFE_CALL(jump(targetPr, JA, rough, &arrIndex, i, line, labels));
        }
        else if (strcmp(cmnd, "JAE") == 0) {
            SAFE_CALL(jump(targetPr, JAE, rough, &arrIndex, i, line, labels));
        }
        else if (strcmp(cmnd, "JE") == 0) {
            SAFE_CALL(jump(targetPr, JE, rough, &arrIndex, i, line, labels));
        }
        else if (strcmp(cmnd, "JNE") == 0) {
            SAFE_CALL(jump(targetPr, JNE, rough, &arrIndex, i, line, labels));
        }
        else if (cmnd[0] == ':') {
            int label = -1;
            char trash[50] = {};
            if (sscanf(cmnd, ":%d %s", &label, trash) != 1 || label < 0 || label >= MAX_LABELS) {
                RETURN_ERR(INVALID_INPUT, "invalid label");
            }

            DPRINTF("read label: '%d', with arr index: '%llu'\n", label, arrIndex);
            labels[label] = (int) arrIndex;
            continue;
        }
        else if (strcmp(cmnd, "HLT") == 0) {
            SAFE_CALL(addCmnd(targetPr, arrIndex, rough, HLT, line, i));
        }
        else {
            PRINT_ASM_LINE_ERR();
            RETURN_ERR(INVALID_INPUT, "invalid command");
        }

        arrIndex++;
    }
    if (strcmp(cmnd, "HLT") != 0) {
        RETURN_ERR(INVALID_INPUT, "PROGRAMM IS NOT FINITE, POSSIBLE TIME CURVATURE OF SPACE AND TIME");
    }

    *size = arrIndex;
    return SUCCESS;
}

error_t compile(pointer_array_buf_t* text) {
    assert(text);

    FILE *targetPr;
    FILE *targetStreamRough;
    SAFE_CALL(openFiles(targetPr, targetStreamRough));

    fprintf(targetPr, "%s V: %d\n", SIGNATURA, VERSION);
    DPRINTF("lines count: %d\n", text->lines_count);

    int labels[MAX_LABELS] = {};

    int rough[MAX_COMMANDS] = {};

    char cmnd[MAX_COMMAND_LENGTH] = {};

    size_t size = 0;
    SAFE_CALL(substituteCommands(text, stdout, labels, rough, cmnd, &size));
    SAFE_CALL(substituteCommands(text, targetPr, labels, rough, cmnd, &size));

    DPRINTF("the compiler meets its destiny\n");

    fwrite(rough, sizeof(int), size, targetStreamRough);

    fclose(targetPr);
    fclose(targetStreamRough);

    return SUCCESS;
}

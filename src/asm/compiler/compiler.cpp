#include "compiler.h"

#define PRINT_ASM_LINE_ERR() \
    BEGIN \
    PRINTERR("AT LINE %d\nin file: ", i + 1); \
    PRINTERR(ASM_SRC_PATH); \
    PRINTERR(":%d\n\n", i+1); \
    END

error_t openFiles(FILE *&targetPr, FILE *&targetStreamBytes) {
    targetPr = fopen(BYTECODE_PR_PATH, "wb");
    targetStreamBytes = fopen(BYTECODE_PATH, "wb");

    if (!targetStreamBytes || !targetPr) {
        RETURN_ERR(FILE_NOT_FOUND, "could not open file");
    }

    return SUCCESS;
}

static error_t addCmnd(FILE *targetPr, size_t arrIndex, int* commandsArr, command_t command, char* line, int i) {
    size_t len = strlen(line);
    for (size_t ch = 0; ch < len; ch++) {
        if (!isspace(line[ch])) {
            PRINT_ASM_LINE_ERR();
            RETURN_ERR(INVALID_INPUT, "unexpected parameter");
        }
    }

    fprintf(targetPr, "%d\n", command);
    commandsArr[arrIndex] = command;
    return SUCCESS;
}

static error_t getRegVal(int i, char *line, char* regVal) {
    int read = 0;
    size_t len = strlen(line);
    int scanCount = sscanf(line, " %cX%n", regVal, &read);
    for (size_t ch = 0; (size_t) read + ch < len; ch++) {
        if (!isspace(line[(size_t) read + ch])) {
            PRINT_ASM_LINE_ERR();
            RETURN_ERR(INVALID_INPUT, "unexpected parameter");
        }
    }

    if (scanCount != 1) {
        PRINT_ASM_LINE_ERR();
        RETURN_ERR(INVALID_INPUT, "invalid register value");
    }

    return SUCCESS;
}

static error_t modifyReg(FILE *targetPr, size_t* arrIndex, int* commandsArr, int i, char *line, command_t command) {
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

    commandsArr[*arrIndex] = command;
    *arrIndex += 1;
    commandsArr[*arrIndex] = regInt;

    return SUCCESS;
}

static error_t pushCmndAndValue(FILE *targetPr, const command_t command, int commandsArr[MAX_COMMANDS],
                                size_t* arrIndex, const int i, const char *line) {
    int pushVal = POISON;

    int read = 0;
    int scanCount = sscanf(line, " %d%n", &pushVal, &read);
    size_t len = strlen(line);
    for (size_t ch = 0; (size_t) read + ch < len; ch++) {
        if (!isspace(line[(size_t) read + ch])) {
            PRINT_ASM_LINE_ERR();
            RETURN_ERR(INVALID_INPUT, "unexpected parameter");
        }
    }
    if (scanCount != 1) {
        PRINT_ASM_LINE_ERR();
        RETURN_ERR(INVALID_INPUT, "invalid input value");
    }

    DPRINTF("input value: %d\n", pushVal);

    fprintf(targetPr, "%d %d\n", command, pushVal);

    commandsArr[*arrIndex] = command;
    *arrIndex += 1;
    commandsArr[*arrIndex] = pushVal;

    return SUCCESS;
}

static error_t jump(FILE *targetPr, const command_t command, int commandsArr[MAX_COMMANDS],
                    size_t* arrIndex, const int i, const char *line, int labels[MAX_LABELS], size_t* unknownLabels) {
    int jmpVal = POISON;
    int read = 0;
    int scanCount = sscanf(line, " %d%n", &jmpVal, &read);
    if (scanCount == 1) {
        size_t len = strlen(line);
        for (size_t ch = 0; (size_t) read + ch < len; ch++) {
            if (!isspace(line[(size_t) read + ch])) {
                PRINT_ASM_LINE_ERR();
                RETURN_ERR(INVALID_INPUT, "unexpected parameter");
            }
        }
        DPRINTF("input value: %d\n", jmpVal);

        fprintf(targetPr, "%d %d\n", command, jmpVal);

        commandsArr[*arrIndex] = command;
        *arrIndex += 1;
        commandsArr[*arrIndex] = jmpVal;

        return SUCCESS;
    }

    int label = -1;
    read = 0;
    scanCount = sscanf(line, " :%d%n", &label, &read);
    if (scanCount == 1 && label >= 0 && label <= MAX_LABELS) {
        size_t len = strlen(line);
        for (size_t ch = 0; (size_t) read + ch < len; ch++) {
            if (!isspace(line[(size_t) read + ch])) {
                PRINT_ASM_LINE_ERR();
                RETURN_ERR(INVALID_INPUT, "unexpected parameter");
            }
        }
        DPRINTF("label: %d\n", label);
        DPRINTF("label val: %d\n", labels[label]);

        fprintf(targetPr, "%d %d\n", command, labels[label]);

        commandsArr[*arrIndex] = command;
        *arrIndex += 1;
        commandsArr[*arrIndex] = labels[label];

        if (labels[label] == -1) {
            *unknownLabels += 1;
        }

        return SUCCESS;
    }

    PRINT_ASM_LINE_ERR();
    RETURN_ERR(INVALID_INPUT, "invalid input value");
}

static char* findFirstSymb(char* str) {
    for (; *str != '\0' && isspace(*str); str++) {
    }
    return str;
}

error_t writeCommands(pointer_array_buf_t* text, FILE* targetPr, int labels[MAX_LABELS],
                      int commandsArr[MAX_COMMANDS], size_t* size, size_t* unknownLabels) {
    size_t arrIndex = 0;
    char cmnd[MAX_COMMAND_LENGTH] = {};
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
            SAFE_CALL(addCmnd(targetPr, arrIndex, commandsArr, ADD, line, i));
        }
        else if (strcmp(cmnd, "SUB") == 0) {
            SAFE_CALL(addCmnd(targetPr, arrIndex, commandsArr, SUB, line, i));
        }
        else if (strcmp(cmnd, "MUL") == 0) {
            SAFE_CALL(addCmnd(targetPr, arrIndex, commandsArr, MUL, line, i));
        }
        else if (strcmp(cmnd, "DIV") == 0) {
            SAFE_CALL(addCmnd(targetPr, arrIndex, commandsArr, DIV, line, i));
        }
        else if (strcmp(cmnd, "SQRT") == 0) {
            SAFE_CALL(addCmnd(targetPr, arrIndex, commandsArr, SQRT, line, i));
        }
        else if (strcmp(cmnd, "OUT") == 0) {
            SAFE_CALL(addCmnd(targetPr, arrIndex, commandsArr, OUT, line, i));
        }
        else if (strcmp(cmnd, "PUSH") == 0) {
            SAFE_CALL(pushCmndAndValue(targetPr, PUSH, commandsArr, &arrIndex, i, line));
        }
        else if (strcmp(cmnd, "IN") == 0) {
            SAFE_CALL(addCmnd(targetPr, arrIndex, commandsArr, IN, line, i));
        }
        else if (strcmp(cmnd, "PUSHREG") == 0) {
            SAFE_CALL(modifyReg(targetPr, &arrIndex, commandsArr, i, line, PUSHREG));
        }
        else if (strcmp(cmnd, "POPREG") == 0) {
            SAFE_CALL(modifyReg(targetPr, &arrIndex, commandsArr, i, line, POPREG));
        }
        else if (strcmp(cmnd, "CP") == 0) {
            SAFE_CALL(modifyReg(targetPr, &arrIndex, commandsArr, i, line, CP));
        }
        else if (strcmp(cmnd, "JMP") == 0) {
            SAFE_CALL(modifyReg(targetPr, &arrIndex, commandsArr, i, line, JMP));
        }
        else if (strcmp(cmnd, "JB") == 0) {
            SAFE_CALL(jump(targetPr, JB, commandsArr, &arrIndex, i, line, labels, unknownLabels));
        }
        else if (strcmp(cmnd, "JBE") == 0) {
            SAFE_CALL(jump(targetPr, JBE, commandsArr, &arrIndex, i, line, labels, unknownLabels));
        }
        else if (strcmp(cmnd, "JA") == 0) {
            SAFE_CALL(jump(targetPr, JA, commandsArr, &arrIndex, i, line, labels, unknownLabels));
        }
        else if (strcmp(cmnd, "JAE") == 0) {
            SAFE_CALL(jump(targetPr, JAE, commandsArr, &arrIndex, i, line, labels, unknownLabels));
        }
        else if (strcmp(cmnd, "JE") == 0) {
            SAFE_CALL(jump(targetPr, JE, commandsArr, &arrIndex, i, line, labels, unknownLabels));
        }
        else if (strcmp(cmnd, "JNE") == 0) {
            SAFE_CALL(jump(targetPr, JNE, commandsArr, &arrIndex, i, line, labels, unknownLabels));
        }
        else if (cmnd[0] == ':') {
            int label = -1, read = 0;
            size_t len = strlen(cmnd);
            int scanCount = sscanf(cmnd, " :%d%n", &label, &read);
            if (scanCount == 1 && label >= 0 && label < MAX_LABELS) {
                for (size_t ch = 0; (size_t) read + ch < len; ch++) {
                    if (!isspace(cmnd[(size_t) read + ch])) {
                        PRINT_ASM_LINE_ERR();
                        RETURN_ERR(INVALID_INPUT, "unexpected parameter");
                    }
                }
                DPRINTF("read label: '%d', with arr index: '%llu'\n", label, arrIndex);
                labels[label] = (int) arrIndex;
                continue;
            }

            PRINT_ASM_LINE_ERR();
            RETURN_ERR(INVALID_INPUT, "invalid label");
        }
        else if (strcmp(cmnd, "HLT") == 0) {
            SAFE_CALL(addCmnd(targetPr, arrIndex, commandsArr, HLT, line, i));
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
    FILE *targetStreamBytes;
    SAFE_CALL(openFiles(targetPr, targetStreamBytes));

    fprintf(targetPr, "%s V: %d\n", SIGNATURA, VERSION);
    DPRINTF("lines count: %d\n", text->lines_count);

    int labels[MAX_LABELS] = {};
    for (int i = 0; i < MAX_LABELS; i++) {
        labels[i] = -1;
    }
    int commandsArr[MAX_COMMANDS + 2] = {};
    commandsArr[0] = SIGNATURA_BYTE;
    commandsArr[1] = VERSION;

    size_t size = 0, unknownLabels = 0;
    SAFE_CALL(writeCommands(text, stdout, labels, commandsArr + 2, &size, &unknownLabels));
    if (unknownLabels != 0) {
        SAFE_CALL(writeCommands(text, targetPr, labels, commandsArr + 2, &size, &unknownLabels));
    }
    unknownLabels = 0;
    if (unknownLabels != 0) {
        RETURN_ERR(INVALID_INPUT, "unknown labels");
    }

    DPRINTF("the compiler meets its destiny\n");

    fwrite(commandsArr, sizeof(int), size + 2, targetStreamBytes);

    fclose(targetPr);
    fclose(targetStreamBytes);

    return SUCCESS;
}

#include "compiler.h"

#define PRINT_ASM_LINE_ERR() \
    BEGIN \
    PRINTERR("AT LINE %d\nin file: ", compilerInfo->i + 1); \
    PRINTERR(ASM_SRC_PATH); \
    PRINTERR(":%d\n\n", compilerInfo->i+1); \
    END

error_t openFiles(FILE *&targetPr, FILE *&targetStreamBytes) {
    targetPr = fopen(BYTECODE_PR_PATH, "wb");
    targetStreamBytes = fopen(BYTECODE_PATH, "wb");

    if (!targetStreamBytes || !targetPr) {
        RETURN_ERR(FILE_NOT_FOUND, "could not open file");
    }

    return SUCCESS;
}

static error_t addCmnd(compilerInfo_t* compilerInfo) {
    fprintf(compilerInfo->targetPr, "%03llu    ", compilerInfo->arrIndex);
    size_t len = strlen(compilerInfo->line);
    for (size_t ch = 0; ch < len; ch++) {
        if (!isspace(compilerInfo->line[ch])) {
            PRINT_ASM_LINE_ERR();
            RETURN_ERR(INVALID_INPUT, "unexpected parameter");
        }
    }

    fprintf(compilerInfo->targetPr, "%0.8d    ", compilerInfo->command);
    compilerInfo->commandsArr[compilerInfo->arrIndex] = compilerInfo->command;
    return SUCCESS;
}

static error_t getRegVal(compilerInfo_t* compilerInfo) {
    int read = 0;
    size_t len = strlen(compilerInfo->line);
    int scanCount = sscanf(compilerInfo->line, " %cX%n", &(compilerInfo->regVal), &read);
    for (size_t ch = 0; (size_t) read + ch < len; ch++) {
        if (!isspace(compilerInfo->line[(size_t) read + ch])) {
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

static error_t modifyReg(compilerInfo_t* compilerInfo) {
    fprintf(compilerInfo->targetPr, "%03llu    ", compilerInfo->arrIndex);

    SAFE_CALL(getRegVal(compilerInfo));
    const int regInt = compilerInfo->regVal - 'A';

    DPRINTF("regVal: %c\n", compilerInfo->regVal);
    DPRINTF("regInt: %d\n", regInt);
    if (!isalpha(compilerInfo->regVal) || regInt >= REGISTER_SIZE || regInt < 0) {
        PRINT_ASM_LINE_ERR();
        RETURN_ERR(INVALID_INPUT, "invalid register value");
    }

    fprintf(compilerInfo->targetPr, "%0.3d  %0.3d    ", compilerInfo->command, regInt);

    compilerInfo->commandsArr[compilerInfo->arrIndex] = compilerInfo->command;
    compilerInfo->arrIndex += 1;
    compilerInfo->commandsArr[compilerInfo->arrIndex] = regInt;

    return SUCCESS;
}

static error_t pushCmndAndValue(compilerInfo_t* compilerInfo) {
    fprintf(compilerInfo->targetPr, "%03llu    ", compilerInfo->arrIndex);

    int pushVal = POISON;

    int read = 0;
    int scanCount = sscanf(compilerInfo->line, " %d%n", &pushVal, &read);
    size_t len = strlen(compilerInfo->line);
    for (size_t ch = 0; (size_t) read + ch < len; ch++) {
        if (!isspace(compilerInfo->line[(size_t) read + ch])) {
            PRINT_ASM_LINE_ERR();
            RETURN_ERR(INVALID_INPUT, "unexpected parameter");
        }
    }
    if (scanCount != 1) {
        PRINT_ASM_LINE_ERR();
        RETURN_ERR(INVALID_INPUT, "invalid input value");
    }

    DPRINTF("input value: %d\n", pushVal);

    fprintf(compilerInfo->targetPr, "%03d  %03d    ", compilerInfo->command, pushVal);

    compilerInfo->commandsArr[compilerInfo->arrIndex] = compilerInfo->command;
    compilerInfo->arrIndex += 1;
    compilerInfo->commandsArr[compilerInfo->arrIndex] = pushVal;

    return SUCCESS;
}

static error_t jump(compilerInfo_t* compilerInfo) {
    fprintf(compilerInfo->targetPr, "%03llu    ", compilerInfo->arrIndex);

    int jmpVal = POISON;
    int read = 0;
    int scanCount = sscanf(compilerInfo->line, " %d%n", &jmpVal, &read);
    if (scanCount == 1) {
        size_t len = strlen(compilerInfo->line);
        for (size_t ch = 0; (size_t) read + ch < len; ch++) {
            if (!isspace(compilerInfo->line[(size_t) read + ch])) {
                PRINT_ASM_LINE_ERR();
                RETURN_ERR(INVALID_INPUT, "unexpected parameter");
            }
        }
        DPRINTF("input value: %d\n", jmpVal);

        fprintf(compilerInfo->targetPr, "%0.3d  %0.3d    ", compilerInfo->command, jmpVal);

        compilerInfo->commandsArr[compilerInfo->arrIndex] = compilerInfo->command;
        compilerInfo->arrIndex += 1;
        compilerInfo->commandsArr[compilerInfo->arrIndex] = jmpVal;

        return SUCCESS;
    }

    int label = -1;
    read = 0;
    scanCount = sscanf(compilerInfo->line, " :%d%n", &label, &read);
    if (scanCount == 1 && label >= 0 && label <= MAX_LABELS) {
        size_t len = strlen(compilerInfo->line);
        for (size_t ch = 0; (size_t) read + ch < len; ch++) {
            if (!isspace(compilerInfo->line[(size_t) read + ch])) {
                PRINT_ASM_LINE_ERR();
                RETURN_ERR(INVALID_INPUT, "unexpected parameter");
            }
        }
        DPRINTF("label: %d\n", label);
        DPRINTF("label val: %d\n", compilerInfo->labels[label]);

        fprintf(compilerInfo->targetPr, "%0.3d  %0.3d    ", compilerInfo->command, compilerInfo->labels[label]);

        compilerInfo->commandsArr[compilerInfo->arrIndex] = compilerInfo->command;
        compilerInfo->arrIndex += 1;
        compilerInfo->commandsArr[compilerInfo->arrIndex] = compilerInfo->labels[label];

        if (compilerInfo->labels[label] == -1) {
            compilerInfo->unknownLabels += 1;
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

error_t writeCommands(compilerInfo_t* compilerInfo) {
    compilerInfo->arrIndex = 0;
    compilerInfo->i = 0;
    char cmnd[MAX_COMMAND_LENGTH] = {};
    for (; compilerInfo->i < compilerInfo->text->lines_count; compilerInfo->i++) {
        compilerInfo->line = strupr(findFirstSymb(compilerInfo->text->pointer_arr[compilerInfo->i].ptr));
        char* commentIndex = strchr(compilerInfo->line, ';'); // коменты ингнорим
        if (commentIndex != NULL) {
            *commentIndex = '\0';
        }
        if (compilerInfo->line[0] == '\0') {
            continue;
        }

        int charsRead = -1;
        if (sscanf(compilerInfo->line, "%s%n", cmnd, &charsRead) != 1) {
            PRINT_ASM_LINE_ERR();
            RETURN_ERR(INVALID_INPUT, "invalid command");
        }
        DPRINTF("read line[%d]: '%s' and command: '%s'\n", compilerInfo->i+1, compilerInfo->line, cmnd);
        compilerInfo->line = compilerInfo->line+charsRead;

        if (strcmp(cmnd, "ADD") == 0) {
            compilerInfo->command = ADD;
            SAFE_CALL(addCmnd(compilerInfo));
        }
        else if (strcmp(cmnd, "SUB") == 0) {
            compilerInfo->command = SUB;
            SAFE_CALL(addCmnd(compilerInfo));
        }
        else if (strcmp(cmnd, "MUL") == 0) {
            compilerInfo->command = MUL;
            SAFE_CALL(addCmnd(compilerInfo));
        }
        else if (strcmp(cmnd, "DIV") == 0) {
            compilerInfo->command = DIV;
            SAFE_CALL(addCmnd(compilerInfo));
        }
        else if (strcmp(cmnd, "SQRT") == 0) {
            compilerInfo->command = SQRT;
            SAFE_CALL(addCmnd(compilerInfo));
        }
        else if (strcmp(cmnd, "OUT") == 0) {
            compilerInfo->command = OUT;
            SAFE_CALL(addCmnd(compilerInfo));
        }
        else if (strcmp(cmnd, "PUSH") == 0) {
            compilerInfo->command = PUSH;
            SAFE_CALL(pushCmndAndValue(compilerInfo));
        }
        else if (strcmp(cmnd, "IN") == 0) {
            compilerInfo->command = IN;
            SAFE_CALL(addCmnd(compilerInfo));
        }
        else if (strcmp(cmnd, "PUSHREG") == 0) {
            compilerInfo->command = PUSHREG;
            SAFE_CALL(modifyReg(compilerInfo));
        }
        else if (strcmp(cmnd, "POPREG") == 0) {
            compilerInfo->command = POPREG;
            SAFE_CALL(modifyReg(compilerInfo));
        }
        else if (strcmp(cmnd, "JMP") == 0) {
            compilerInfo->command = JMP;
            SAFE_CALL(jump(compilerInfo));
        }
        else if (strcmp(cmnd, "JB") == 0) {
            compilerInfo->command = JB;
            SAFE_CALL(jump(compilerInfo));
        }
        else if (strcmp(cmnd, "JBE") == 0) {
            compilerInfo->command = JBE;
            SAFE_CALL(jump(compilerInfo));
        }
        else if (strcmp(cmnd, "JA") == 0) {
            compilerInfo->command = JA;
            SAFE_CALL(jump(compilerInfo));
        }
        else if (strcmp(cmnd, "JAE") == 0) {
            compilerInfo->command = JAE;
            SAFE_CALL(jump(compilerInfo));
        }
        else if (strcmp(cmnd, "JE") == 0) {
            compilerInfo->command = JE;
            SAFE_CALL(jump(compilerInfo));
        }
        else if (strcmp(cmnd, "JNE") == 0) {
            compilerInfo->command = JNE;
            SAFE_CALL(jump(compilerInfo));
        }
        else if (strcmp(cmnd, "CALL") == 0) {
            compilerInfo->command = CALL;
            SAFE_CALL(jump(compilerInfo));
        }
        else if (strcmp(cmnd, "RET") == 0) {
            compilerInfo->command = RET;
            SAFE_CALL(addCmnd(compilerInfo));
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
                DPRINTF("read label: '%d', with arr index: '%llu'\n", label, compilerInfo->arrIndex);
                compilerInfo->labels[label] = (int) compilerInfo->arrIndex;
                continue;
            }

            PRINT_ASM_LINE_ERR();
            RETURN_ERR(INVALID_INPUT, "invalid label");
        }
        else if (strcmp(cmnd, "HLT") == 0) {
            compilerInfo->command = HLT;
            SAFE_CALL(addCmnd(compilerInfo));
        }
        else {
            PRINT_ASM_LINE_ERR();
            RETURN_ERR(INVALID_INPUT, "invalid command");
        }
        fprintf(compilerInfo->targetPr, "%1.10s\n", cmnd);

        compilerInfo->arrIndex++;
    }
    if (strcmp(cmnd, "HLT") != 0) {
        RETURN_ERR(INVALID_INPUT, "PROGRAMM IS NOT FINITE, POSSIBLE TIME CURVATURE OF SPACE AND TIME");
    }

    compilerInfo->size = compilerInfo->arrIndex;
    return SUCCESS;
}

error_t compile(pointer_array_buf_t* text) {
    assert(text);

    FILE *targetPr;
    FILE *targetStreamBytes;
    SAFE_CALL(openFiles(targetPr, targetStreamBytes));

    fprintf(targetPr, "%s V: %d\n", SIGNATURA, VERSION);
    DPRINTF("lines count: %d\n", text->lines_count);

    compilerInfo_t compilerInfo = {
        .targetPr = targetPr,
        .text = text,
        .command = HLT,
        .commandsArr = {},
        .size = 0,
        .arrIndex = SIGNATURA_SIZE,
        .i = 0,
        .line = NULL,
        .labels = {},
        .unknownLabels = 0,
        .regVal = 'Z'
    };


    for (int i = 0; i < MAX_LABELS; i++) {
        compilerInfo.labels[i] = -1;
    }

    SAFE_CALL(writeCommands(&compilerInfo));
    if (compilerInfo.unknownLabels != 0) {
        compilerInfo.unknownLabels = 0;
        SAFE_CALL(writeCommands(&compilerInfo));
    }
    if (compilerInfo.unknownLabels != 0) {
        RETURN_ERR(INVALID_INPUT, "unknown labels");
    }

    DPRINTF("the compiler meets its destiny\n");

    fwrite(&SIGNATURA_BYTE, sizeof(int), 1, targetStreamBytes);
    fwrite(&VERSION, sizeof(int), 1, targetStreamBytes);
    fwrite(compilerInfo.commandsArr, sizeof(int), compilerInfo.size, targetStreamBytes);

    fclose(targetPr);
    fclose(targetStreamBytes);

    return SUCCESS;
}

#include "compiler.h"

#define PRINT_ASM_LINE_ERR() \
    BEGIN \
    PRINTERR("AT LINE %d\nin file: ", i + 1); \
    PRINTERR(ASM_SRC_PATH); \
    PRINTERR(":%d\n\n", i+1); \
    END

error_info_t openFiles(FILE *&targetPr, FILE *&targetStreamRough) {
    targetPr = fopen(BYTECODE_PR_PATH, "wb");
    targetStreamRough = fopen(BYTECODE_PATH, "wb");

    if (!targetStreamRough || !targetPr) {
        RETURN_ERR(FILE_NOT_FOUND, "could not open file");
    }

    return{SUCCESS};
}

static error_info_t addCmnd(FILE *targetPr, size_t arrIndex, int* rough, command_t command, char* line, int i) {
    char trash[50] = {};
    if(strlen(line) > 50 || sscanf(line, "%s %s", trash, trash) != 1) {
        PRINT_ASM_LINE_ERR();
        RETURN_ERR(INVALID_INPUT, "unexpected parameter");
    }

    fprintf(targetPr, "%d\n", command);
    rough[arrIndex] = command;
    return {SUCCESS};
}

static error_info_t getRegVal(int i, char *line, char* regVal) {
    char trash[50] = {};
    char cmnd[50] = {};
    int scanCount = sscanf(line, "%s %cX %s", cmnd, regVal, trash);
    if (scanCount != 2) {
        PRINT_ASM_LINE_ERR();
        RETURN_ERR(INVALID_INPUT, "invalid register value");
    }
    DPRINTF("comand: %s\n", cmnd);

    return {SUCCESS};
}

static error_info_t modifyReg(FILE *targetPr, size_t &arrIndex, int* rough, int i, char *line, command_t command) {
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

    rough[arrIndex++] = command;
    rough[arrIndex] = regInt;

    return{SUCCESS};
}

static error_info_t pushCmndAndValue(FILE *targetPr, const char* const commandStr, const command_t command, int rough[1024], size_t &arrIndex, const int i, const char *line) {
    int pushVal = POISON;

    char valueRegex[50] = {};
    char trash[50] = {};
    strcat(valueRegex, commandStr);
    strcat(valueRegex, " %d %s");

    int scanCount = sscanf(line, valueRegex, &pushVal, trash);
    if (scanCount != 1) {
        PRINT_ASM_LINE_ERR();
        RETURN_ERR(INVALID_INPUT, "invalid input value");
    }
    DPRINTF("input value: %d\n", pushVal);

    fprintf(targetPr, "%d %d\n", command, pushVal);

    rough[arrIndex++] = command;
    rough[arrIndex] = pushVal;

    return {SUCCESS};
}

error_info_t compile(pointer_array_buf_t* text) {
    assert(text);

    FILE *targetPr;
    FILE *targetStreamRough;
    SAFE_CALL(openFiles(targetPr, targetStreamRough));
    fprintf(targetPr, "%s V: %d\n", SIGNATURA, VERSION);

    int rough[1024] = {};
    size_t arrIndex = 0;

    char cmnd[10] = {};
    DPRINTF("lines count: %d\n", text->lines_count);
    for (int i = 0; i < text->lines_count; i++, arrIndex++) {
        char* line = strupr(text->pointer_arr[i].ptr);
        char* cmntIndex = strchr(line, ';'); // коменты ингнорим
        if (cmntIndex != NULL) {
            *cmntIndex = '\0';
        }
        if (line[0] == '\0') {
            continue;
        }

        if (sscanf(line, "%s", cmnd) != 1) {
            PRINT_ASM_LINE_ERR();
            RETURN_ERR(INVALID_INPUT, "invalid command");
        }
        DPRINTF("read line[%d]: '%s' and command: '%s'\n", i+1, line, cmnd);
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
            SAFE_CALL(pushCmndAndValue(targetPr, "PUSH", PUSH, rough, arrIndex, i, line));
        }
        else if (strcmp(cmnd, "IN") == 0) {
            SAFE_CALL(addCmnd(targetPr, arrIndex, rough, IN, line, i));
        }
        else if (strcmp(cmnd, "PUSHREG") == 0) {
            SAFE_CALL(modifyReg(targetPr, arrIndex, rough, i, line, PUSHREG));
        }
        else if (strcmp(cmnd, "POPREG") == 0) {
            SAFE_CALL(modifyReg(targetPr, arrIndex, rough, i, line, POPREG));
        }
        else if (strcmp(cmnd, "CP") == 0) {
            SAFE_CALL(modifyReg(targetPr, arrIndex, rough, i, line, CP));
        }
        else if (strcmp(cmnd, "JMP") == 0) {
            SAFE_CALL(modifyReg(targetPr, arrIndex, rough, i, line, JMP));
        }
        else if (strcmp(cmnd, "JB") == 0) {
            SAFE_CALL(pushCmndAndValue(targetPr, "JB", JB, rough, arrIndex, i, line));
        }
        else if (strcmp(cmnd, "JBE") == 0) {
            SAFE_CALL(pushCmndAndValue(targetPr, "JBE", JBE, rough, arrIndex, i, line));
        }
        else if (strcmp(cmnd, "JA") == 0) {
            SAFE_CALL(pushCmndAndValue(targetPr, "JA", JA, rough, arrIndex, i, line));
        }
        else if (strcmp(cmnd, "JAE") == 0) {
            SAFE_CALL(pushCmndAndValue(targetPr, "JAE", JAE, rough, arrIndex, i, line));
        }
        else if (strcmp(cmnd, "JE") == 0) {
            SAFE_CALL(pushCmndAndValue(targetPr, "JE", JE, rough, arrIndex, i, line));
        }
        else if (strcmp(cmnd, "JNE") == 0) {
            SAFE_CALL(pushCmndAndValue(targetPr, "JNE", JNE, rough, arrIndex, i, line));
        }
        else if (strcmp(cmnd, "HLT") == 0) {
            SAFE_CALL(addCmnd(targetPr, arrIndex, rough, HLT, line, i));
        }
        else {
            PRINT_ASM_LINE_ERR();
            RETURN_ERR(INVALID_INPUT, "invalid command");
        }
    }
    if (strcmp(cmnd, "HLT") != 0) {
        RETURN_ERR(INVALID_INPUT, "PROGRAMM IS NOT FINITE, POSSIBLE TIME CURVATURE OF SPACE AND TIME");
    }

    DPRINTF("the compiler meets its destiny\n");

    fwrite(rough, sizeof(int), arrIndex, targetStreamRough);

    fclose(targetPr);
    fclose(targetStreamRough);

    return {SUCCESS};
}

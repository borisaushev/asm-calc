#include "compiler.h"

error_info_t openFiles(FILE *&targetPr, FILE *&targetStreamRough) {
    targetPr = fopen(BYTECODE_PR_PATH, "wb");
    targetStreamRough = fopen(BYTECODE_PATH, "wb");

    if (!targetStreamRough || !targetPr) {
        RETURN_ERR(FILE_NOT_FOUND, "could not open file");
    }

    return{SUCCESS};
}

static void addCmnd(FILE *targetPr, size_t arrIndex, int* rough, command_t command) {
    fprintf(targetPr, "%d\n", command);
    rough[arrIndex] = command;
}

static error_info_t getRegVal(int i, char *line, char* regVal) {
    char cmnd[50];
    int scanCount = sscanf(line, "%s %cX", cmnd, regVal);
    if (scanCount != 2) {
        PRINTERR("invalid register value at line %d\n", i);
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
        PRINTERR("invalid register value at line %d\n", i);
        RETURN_ERR(INVALID_INPUT, "invalid register value");
    }

    fprintf(targetPr, "%d %d\n", command, regInt);

    rough[arrIndex++] = command;
    rough[arrIndex] = regInt;

    return{SUCCESS};
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
    for (int i = 0; i < text->lines_count; i++, arrIndex++) {
        char* line = strupr(text->pointer_arr[i].ptr);
        if (line[0] == '\0') {
            continue;
        }
        char* cmntIndex = strchr(line, ';');
        if (cmntIndex != NULL) {
            *cmntIndex = '\0';
        }

        if (sscanf(line, "%s", cmnd) != 1) {
            PRINTERR("COULDN'T PARSE COMMAND AT LINE: %d\n", i+1);
            RETURN_ERR(INVALID_INPUT, "invalid command");
        }
        DPRINTF("read line: '%s' and command: '%s'\n", line, cmnd);
        if (strcmp(cmnd, "ADD") == 0) {
            addCmnd(targetPr, arrIndex, rough, ADD);
        }
        else if (strcmp(cmnd, "SUB") == 0) {
            addCmnd(targetPr, arrIndex, rough, SUB);
        }
        else if (strcmp(cmnd, "MUL") == 0) {
            addCmnd(targetPr, arrIndex, rough, MUL);
        }
        else if (strcmp(cmnd, "DIV") == 0) {
            addCmnd(targetPr, arrIndex, rough, DIV);
        }
        else if (strcmp(cmnd, "OUT") == 0) {
            addCmnd(targetPr, arrIndex, rough, OUT);
        }
        else if (strcmp(cmnd, "PUSH") == 0) {
            int pushVal = POISON;
            int scanCount = sscanf(line, "PUSH %d", &pushVal);
            if (scanCount != 1) {
                PRINTERR("invalid push value at line %d\n", i+1);
                RETURN_ERR(INVALID_INPUT, "invalid push value");
            }
            DPRINTF("pushVal: %d\n", pushVal);

            fprintf(targetPr, "%d %d\n", PUSH, pushVal);

            rough[arrIndex++] = PUSH;
            rough[arrIndex] = pushVal;
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
        else if (strcmp(cmnd, "HLT") == 0) {
            break;
        }
        else {
            PRINTERR("invalid command at line %d\n", i+1);
            RETURN_ERR(INVALID_INPUT, "invalid command");
        }
    }
    if (strcmp(cmnd, "HLT") != 0) {
        PRINTERR("PROGRAMM IS NOT FINITE, POSSIBLE TIME CURVATURE OF SPACE AND TIME\n");
        RETURN_ERR(INVALID_INPUT, "PROGRAMM IS NOT FINITE");
    }

    fprintf(targetPr, "%d\n", HLT);
    DPRINTF("the compiler meets its destiny\n");

    fwrite(rough, sizeof(int), arrIndex, targetStreamRough);

    fclose(targetPr);
    fclose(targetStreamRough);

    return {SUCCESS};
}

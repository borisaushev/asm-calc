#include "compilerСommands.h"

error_t noArgsCommand(compilerInfo_t* compilerInfo) {
    assert(compilerInfo);

    size_t len = strlen(compilerInfo->line);
    for (size_t ch = 0; ch < len; ch++) {
        if (!isspace(compilerInfo->line[ch])) {
            PRINT_ASM_LINE_ERR();
            RETURN_ERR(INVALID_INPUT, "unexpected parameter");
        }
    }

    compilerInfo->commandsArr[compilerInfo->arrIndex] = compilerInfo->command;
    return SUCCESS;
}

error_t getRegVal(compilerInfo_t* compilerInfo) {
    assert(compilerInfo);

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

error_t regParam(compilerInfo_t* compilerInfo) {
    assert(compilerInfo);

    SAFE_CALL(getRegVal(compilerInfo));
    const int regInt = compilerInfo->regVal - 'A';

    DPRINTF("regVal: %c\n", compilerInfo->regVal);
    DPRINTF("regInt: %d\n", regInt);
    if (!isalpha(compilerInfo->regVal) || regInt >= REGISTER_SIZE || regInt < 0) {
        PRINT_ASM_LINE_ERR();
        RETURN_ERR(INVALID_INPUT, "invalid register value");
    }

    compilerInfo->commandsArr[compilerInfo->arrIndex] = compilerInfo->command;
    compilerInfo->arrIndex += 1;
    compilerInfo->commandsArr[compilerInfo->arrIndex] = regInt;

    return SUCCESS;
}

error_t pushCmndAndValue(compilerInfo_t* compilerInfo) {
    assert(compilerInfo);

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

    compilerInfo->commandsArr[compilerInfo->arrIndex] = compilerInfo->command;
    compilerInfo->arrIndex += 1;
    compilerInfo->commandsArr[compilerInfo->arrIndex] = pushVal;

    return SUCCESS;
}

error_t jumpCommand(compilerInfo_t* compilerInfo) {
    assert(compilerInfo);

    int label = -1;
    int read = 0;
    int scanCount = sscanf(compilerInfo->line, " :%d%n", &label, &read);
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

error_t parseLabel(compilerInfo_t *compilerInfo, char cmnd[MAX_COMMAND_LENGTH]) {
    assert(compilerInfo);

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
        return SUCCESS;
    }

    PRINT_ASM_LINE_ERR();
    RETURN_ERR(INVALID_INPUT, "invalid label");
}
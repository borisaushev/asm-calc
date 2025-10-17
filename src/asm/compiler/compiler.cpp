#include "compiler.h"

#include "commands.h"


error_t openFiles(FILE** targetPr, FILE** targetStreamBytes) {
    assert(targetPr);
    assert(targetStreamBytes);

    *targetPr = fopen(BYTECODE_PR_PATH, "wb");
    *targetStreamBytes = fopen(BYTECODE_PATH, "wb");

    if (!*targetStreamBytes || !*targetPr) {
        RETURN_ERR(FILE_NOT_FOUND, "could not open file");
    }

    return SUCCESS;
}

static char* findFirstSymb(char* str) {
    for (; *str != '\0' && isspace(*str); str++) {
    }
    return str;
}

static error_t parseLineAndCommand(compilerInfo_t *compilerInfo, char cmnd[MAX_COMMAND_LENGTH], int* isBlank) {
    assert(compilerInfo);
    assert(isBlank);

    compilerInfo->line = strupr(findFirstSymb(compilerInfo->text->pointer_arr[compilerInfo->i].ptr));
    char* commentIndex = strchr(compilerInfo->line, ';'); // коменты ингнорим
    if (commentIndex != NULL) {
        *commentIndex = '\0';
    }
    if (compilerInfo->line[0] == '\0') {
        *isBlank = 1;
        return SUCCESS;
    }

    *isBlank = 0;
    int charsRead = -1;
    if (sscanf(compilerInfo->line, "%s%n", cmnd, &charsRead) != 1) {
        PRINT_ASM_LINE_ERR();
        RETURN_ERR(INVALID_INPUT, "invalid command");
    }
    DPRINTF("read line[%d]: '%s' and command: '%s'\n", compilerInfo->i+1, compilerInfo->line, cmnd);
    compilerInfo->line = compilerInfo->line+charsRead;

    return SUCCESS;
}

static error_t findCommand(compilerInfo_t *compilerInfo, char cmnd[MAX_COMMAND_LENGTH], int &found) {
    for (int i = 0; i < COMMANDS_COUNT; i++) {
        commandsInfo_t curCmndFunc = commandsFuncs[i];
        if (strcmp(cmnd, curCmndFunc.commandStr) == 0) {
            compilerInfo->command = curCmndFunc.command;
            SAFE_CALL(curCmndFunc.func(compilerInfo));

            compilerInfo->arrIndex++;
            found = 1;
            break;
        }
    }

    return SUCCESS;
}

error_t compile(compilerInfo_t* compilerInfo) {
    assert(compilerInfo);
    compilerInfo->arrIndex = 0;
    compilerInfo->i = 0;
    compilerInfo->unknownLabels = 0;

    char cmnd[MAX_COMMAND_LENGTH] = {};
    for (; compilerInfo->i < compilerInfo->text->lines_count; compilerInfo->i++) {
        int isBlank = 0;
        SAFE_CALL(parseLineAndCommand(compilerInfo, cmnd, &isBlank));
        if (isBlank) {
            continue;
        }
        if (cmnd[0] == ':') {
            SAFE_CALL(parseLabel(compilerInfo, cmnd));
            continue;
        }

        int found = 0;
        SAFE_CALL(findCommand(compilerInfo, cmnd, found));

        if (!found) {
            PRINT_ASM_LINE_ERR();
            RETURN_ERR(INVALID_INPUT, "invalid command");
        }
    }
    if (strcmp(cmnd, "HLT") != 0) {
        RETURN_ERR(INVALID_INPUT, "PROGRAMM IS NOT FINITE, POSSIBLE TIME CURVATURE OF SPACE AND TIME");
    }

    compilerInfo->size = compilerInfo->arrIndex;
    return SUCCESS;
}

static error_t makeListing(compilerInfo_t* compilerInfo) {
    assert(compilerInfo);

    fprintf(compilerInfo->targetPr, "%s V: %d\n", SIGNATURA, VERSION);
    for (size_t i = 0; i < compilerInfo->size; i++) {
        for (int ind = 0; ind < COMMANDS_COUNT; ind++) {
            commandsInfo_t curCmnd = commandsFuncs[ind];

            if (compilerInfo->commandsArr[i] == curCmnd.command) {
                fprintf(compilerInfo->targetPr, "%03llu    ", i);
                if (curCmnd.argc == 0) {
                    fprintf(compilerInfo->targetPr, "%.8d    ", compilerInfo->commandsArr[i]);
                }
                else if (curCmnd.argc == 1) {
                    fprintf(compilerInfo->targetPr, "%.3d  %.3d    ", compilerInfo->commandsArr[i], compilerInfo->commandsArr[i + 1]);
                    i++;
                }
                else {
                    RETURN_ERR(INVALID_INPUT, "invalid argument count");
                }

                fprintf(compilerInfo->targetPr, "%1.10s\n", curCmnd.commandStr);
                break;
            }
        }
    }

    return SUCCESS;
}

error_t compileAsm(pointer_array_buf_t* text) {
    assert(text);

    FILE *targetPr = NULL;
    FILE *targetStreamBytes = NULL;
    SAFE_CALL(openFiles(&targetPr, &targetStreamBytes));

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

    SAFE_CALL(compile(&compilerInfo));
    if (compilerInfo.unknownLabels != 0) {
        compilerInfo.unknownLabels = 0;
        SAFE_CALL(compile(&compilerInfo));

        if (compilerInfo.unknownLabels != 0) {
            RETURN_ERR(INVALID_INPUT, "unknown labels");
        }
    }

    makeListing(&compilerInfo);
    DPRINTF("the compiler meets its destiny\n");

    fwrite(&SIGNATURA_BYTE, sizeof(int), 1, targetStreamBytes);
    fwrite(&VERSION, sizeof(int), 1, targetStreamBytes);
    fwrite(compilerInfo.commandsArr, sizeof(int), compilerInfo.size, targetStreamBytes);

    fclose(targetPr);
    fclose(targetStreamBytes);

    return SUCCESS;
}

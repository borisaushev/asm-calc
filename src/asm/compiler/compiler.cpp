#include "compiler.h"

#include "compilerСommands.h"


error_t openListingAndByteFiles(FILE** targetPr, FILE** targetStreamBytes) {
    assert(targetPr);
    assert(targetStreamBytes);

    *targetPr = fopen(LISTING_PATH, "wb");
    *targetStreamBytes = fopen(BYTECODE_PATH, "wb");

    if (!*targetStreamBytes || !*targetPr) {
        RETURN_ERR(FILE_NOT_FOUND, "could not open file");
    }

    return SUCCESS;
}

static char* findFirstSymb(char* str) {
    assert(str);

    for (; *str != '\0' && isspace(*str); str++) {
    }
    return str;
}

static error_t parseLineAndCommand(compilerInfo_t *compilerInfo, int* isBlank) {
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
    if (sscanf(compilerInfo->line, "%s%n", compilerInfo->curCommand, &charsRead) != 1) {
        PRINT_ASM_LINE_ERR();
        RETURN_ERR(INVALID_INPUT, "invalid command");
    }
    DPRINTF("read line[%d]: '%s' and command: '%s'\n", compilerInfo->i+1, compilerInfo->line, compilerInfo->curCommand);
    compilerInfo->line = compilerInfo->line+charsRead;

    return SUCCESS;
}

static error_t compileCommand(compilerInfo_t *compilerInfo, int* found) {
    assert(compilerInfo);
    assert(found);

    for (int i = 0; i < COMMANDS_COUNT; i++) {
        compilerCmdInfo_t curCommand = COMPILER_COMMANDS_INFO[i];
        if (strcmp(compilerInfo->curCommand, curCommand.commandStr) == 0) {
            compilerInfo->command = curCommand.command;
            SAFE_CALL(curCommand.func(compilerInfo));

            compilerInfo->arrIndex++;
            *found = 1;
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

    for (; compilerInfo->i < compilerInfo->text->lines_count; compilerInfo->i++) {
        int isBlank = 0;
        SAFE_CALL(parseLineAndCommand(compilerInfo, &isBlank));
        if (isBlank) {
            continue;
        }
        if (compilerInfo->curCommand[0] == ':') {
            SAFE_CALL(parseLabel(compilerInfo, compilerInfo->curCommand));
            continue;
        }

        int found = 0;
        SAFE_CALL(compileCommand(compilerInfo, &found));

        if (!found) {
            PRINT_ASM_LINE_ERR();
            RETURN_ERR(INVALID_INPUT, "invalid command");
        }
    }

    compilerInfo->size = compilerInfo->arrIndex;
    return SUCCESS;
}

static error_t makeListing(compilerInfo_t* compilerInfo) {
    assert(compilerInfo);

    fprintf(compilerInfo->listingFile, "%s V: %d\n", SIGNATURA, VERSION);
    for (size_t i = 0; i < compilerInfo->size; i++) {
        for (int ind = 0; ind < COMMANDS_COUNT; ind++) {
            compilerCmdInfo_t curCommand = COMPILER_COMMANDS_INFO[ind];

            if (compilerInfo->commandsArr[i] == curCommand.command) {
                fprintf(compilerInfo->listingFile, "%03llu    ", i);
                if (curCommand.argc == 0) {
                    fprintf(compilerInfo->listingFile, "%.8d    ", compilerInfo->commandsArr[i]);
                }
                else if (curCommand.argc == 1) {
                    fprintf(compilerInfo->listingFile, "%.3d  %.3d    ", compilerInfo->commandsArr[i], compilerInfo->commandsArr[i + 1]);
                    i++;
                }
                else {
                    RETURN_ERR(INVALID_INPUT, "invalid argument count");
                }

                fprintf(compilerInfo->listingFile, "%1.10s\n", curCommand.commandStr);
                break;
            }
        }
    }

    return SUCCESS;
}

void initCompilerInfo(pointer_array_buf_t *text, FILE *listingFile, compilerInfo_t &compilerInfo) {
    compilerInfo.listingFile = listingFile;
    compilerInfo.text = text;
    compilerInfo.command = HLT;
    compilerInfo.size = 0;
    compilerInfo.arrIndex = SIGNATURA_SIZE;
    compilerInfo.i = 0;
    compilerInfo.line = NULL;
    compilerInfo.unknownLabels = 0;
    compilerInfo.regVal = 'Z';
}

error_t compileAsm(pointer_array_buf_t* text) {
    assert(text);

    FILE *listingFile = NULL;
    FILE *targetStreamBytes = NULL;
    SAFE_CALL(openListingAndByteFiles(&listingFile, &targetStreamBytes));

    DPRINTF("lines count: %d\n", text->lines_count);

    compilerInfo_t compilerInfo = {};
    initCompilerInfo(text, listingFile, compilerInfo);

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

    fclose(listingFile);
    fclose(targetStreamBytes);

    return SUCCESS;
}

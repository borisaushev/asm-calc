#include "compiler.h"

#include "compilerСommands.h"


compilerCommandInfo_t COMPILER_COMMANDS_INFO[COMMANDS_COUNT] = {
    {ADD,     "ADD",     noArgsCommand,  strHash("ADD")    },
    {SUB,     "SUB",     noArgsCommand,  strHash("SUB")    },
    {DIV,     "DIV",     noArgsCommand,  strHash("DIV")    },
    {MUL,     "MUL",     noArgsCommand,  strHash("MUL")    },
    {SQRT,    "SQRT",    noArgsCommand,  strHash("SQRT")   },
    {OUT,     "OUT",     noArgsCommand,  strHash("OUT")    },
    {IN,      "IN",      noArgsCommand,  strHash("IN")     },
    {PUSH,    "PUSH",    pushCommand  ,  strHash("PUSH")   },
    {POPREG,  "POPREG",  regParam     ,  strHash("POPREG") },
    {PUSHREG, "PUSHREG", regParam     ,  strHash("PUSHREG")},
    {JMP,     "JMP",     jumpCommand  ,  strHash("JMP")    },
    {JB,      "JB",      jumpCommand  ,  strHash("JB")     },
    {JBE,     "JBE",     jumpCommand  ,  strHash("JBE")    },
    {JA,      "JA",      jumpCommand  ,  strHash("JA")     },
    {JAE,     "JAE",     jumpCommand  ,  strHash("JAE")    },
    {JE,      "JE",      jumpCommand  ,  strHash("JE")     },
    {JNE,     "JNE",     jumpCommand  ,  strHash("JNE")    },
    {CALL,    "CALL",    jumpCommand  ,  strHash("CALL")   },
    {RET,     "RET",     noArgsCommand,  strHash("RET")    },
    {PUSHMEM, "PUSHMEM", regParam     ,  strHash("PUSHMEM")},
    {POPMEM,  "POPMEM",  regParam     ,  strHash("POPMEM") },
    {DRAW,    "DRAW",    noArgsCommand,  strHash("DRAW")   },
    {HLT,     "HLT",     noArgsCommand,  strHash("HLT")    },
};

error_t openListingAndByteFiles(FILE** targetListing, FILE** targetStreamBytes) {
    assert(targetListing);
    assert(targetStreamBytes);

    *targetListing = fopen(LISTING_PATH, "wb");
    *targetStreamBytes = fopen(BYTECODE_PATH, "wb");

    if (!*targetStreamBytes || !*targetListing) {
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

static int compareCommandInfo(const void* voidInfo1, const void* voidInfo2) {
    const compilerCommandInfo_t* info1 = (const compilerCommandInfo_t*) voidInfo1;
    const compilerCommandInfo_t* info2 = (const compilerCommandInfo_t*) voidInfo2;

    long long dif = info1->hash - info2->hash;
    if (dif == 0) {
        return 0;
    }
    if (dif < 0) {
        return -1;
    }
    return 1;
}

static error_t compileCommand(compilerInfo_t *compilerInfo) {
    assert(compilerInfo);

    compilerCommandInfo_t desiredCommand = {.hash=strHash(compilerInfo->curCommand)};

    compilerCommandInfo_t* curCommand = (compilerCommandInfo_t*)
        bsearch(&desiredCommand, COMPILER_COMMANDS_INFO, COMMANDS_COUNT,
                 sizeof(compilerCommandInfo_t), compareCommandInfo);
    if (curCommand == NULL) {
        PRINT_ASM_LINE_ERR();
        RETURN_ERR(INVALID_INPUT, "unknown command");
    }

    compilerInfo->command = curCommand->command;
    SAFE_CALL(curCommand->function(compilerInfo));

    compilerInfo->arrIndex++;

    return SUCCESS;
}

error_t verifyCommandsArray() {
    for (int i = 0; i < COMMANDS_COUNT; i++) {
        if (COMPILER_COMMANDS_INFO[i].command != i) {
            printf("verifyCommandsArray: invalid command at index %d\n", i);
            RETURN_ERR(INVALID_INPUT, "commands array is invalid");
        }
    }

    return SUCCESS;
}

static int compareCommandsInfo(const void* commandInfoVoid1, const void* commandInfoVoid2) {
    const compilerCommandInfo_t* commandInfo1 = (const compilerCommandInfo_t*) commandInfoVoid1;
    const compilerCommandInfo_t* commandInfo2 = (const compilerCommandInfo_t*) commandInfoVoid2;

    return commandInfo1->hash - commandInfo2->hash > 0 ? 1 : -1;
}

static error_t sortCommandsByHashes() {
    for (int i = 0; i < COMMANDS_COUNT; i++) {
        if (COMPILER_COMMANDS_INFO[i].hash != strHash(COMPILER_COMMANDS_INFO[i].commandStr)) {
            RETURN_ERR(INVALID_INPUT, "command hash doesnt match");
        }
    }

    qsort(COMPILER_COMMANDS_INFO, COMMANDS_COUNT,
          sizeof(compilerCommandInfo_t), compareCommandsInfo);

    return SUCCESS;
}

error_t compile(compilerInfo_t* compilerInfo) {
    assert(compilerInfo);

    SAFE_CALL(verifyCommandsArray());
    SAFE_CALL(sortCommandsByHashes());

    for (; compilerInfo->i < compilerInfo->text->lines_count; compilerInfo->i++) {
        int isBlank = 0;
        SAFE_CALL(parseLineAndCommand(compilerInfo, &isBlank));
        if (isBlank) {
            continue;
        }
        if (compilerInfo->curCommand[0] == ':') {
            SAFE_CALL(parseLabel(compilerInfo));
            continue;
        }

        SAFE_CALL(compileCommand(compilerInfo));
    }

    compilerInfo->size = compilerInfo->arrIndex;
    return SUCCESS;
}

static error_t makeListing(compilerInfo_t* compilerInfo) {
    assert(compilerInfo);

    fprintf(compilerInfo->listingFile, "%s V: %d\n", SIGNATURA, VERSION);
    for (size_t i = 0; i < compilerInfo->size; i++) {
        for (int ind = 0; ind < COMMANDS_COUNT; ind++) {
            compilerCommandInfo_t curCommand = COMPILER_COMMANDS_INFO[ind];

            if (compilerInfo->commandsArr[i] == curCommand.command) {
                fprintf(compilerInfo->listingFile, "%03llu    ", i);
                if (curCommand.function == noArgsCommand) {
                    fprintf(compilerInfo->listingFile, "%.3d         ", compilerInfo->commandsArr[i]);
                }
                else {
                    fprintf(compilerInfo->listingFile, "%.3d  %.3d    ", compilerInfo->commandsArr[i], compilerInfo->commandsArr[i + 1]);
                    i++;
                }

                fprintf(compilerInfo->listingFile, "%1.10s\n", curCommand.commandStr);
                break;
            }
        }
    }

    return SUCCESS;
}

static void initCompilerInfo(pointer_array_buf_t *text, FILE *listingFile, compilerInfo_t* compilerInfo) {
    compilerInfo->listingFile = listingFile;
    compilerInfo->text = text;
    compilerInfo->command = HLT;
    compilerInfo->size = 0;
    compilerInfo->arrIndex = 0;
    compilerInfo->i = 0;
    compilerInfo->line = NULL;
    compilerInfo->regVal = 'Z';
    initStack(&(compilerInfo->fixupStackIndex), MAX_LABELS);
    initStack(&(compilerInfo->fixupStackLabel), MAX_LABELS);

    for (int i = 0; i < MAX_LABELS; i++) {
        compilerInfo->labels[i] = -1;
    }
}

error_t fixupLabels(compilerInfo_t* compilerInfo) {
    while (compilerInfo->fixupStackIndex.elementCount != 0) {
        int curIndex = -1;
        SAFE_CALL(stackPop(&compilerInfo->fixupStackIndex, &curIndex));

        int curLabelIndex = -1;
        SAFE_CALL(stackPop(&compilerInfo->fixupStackLabel, &curLabelIndex));

        if (curIndex == POISON || curLabelIndex == POISON) {
            RETURN_ERR(INVALID_INPUT, "fixupLabels: invalid index");
        }

        compilerInfo->commandsArr[curIndex] = compilerInfo->labels[curLabelIndex];
    }

    return SUCCESS;
}

error_t compileAsm(pointer_array_buf_t* text) {
    assert(text);

    FILE* listingFile = NULL;
    FILE* targetStreamBytes = NULL;

    _TX_TRY {
        _TX_CHECKED(openListingAndByteFiles(&listingFile, &targetStreamBytes));
        DPRINTF("lines count: %d\n", text->lines_count);

        compilerInfo_t compilerInfo = {};
        initCompilerInfo(text, listingFile, &compilerInfo);

        _TX_CHECKED(compile(&compilerInfo));
        _TX_CHECKED(fixupLabels(&compilerInfo));

        makeListing(&compilerInfo);
        DPRINTF("the compiler meets its destiny\n");

        fwrite(&SIGNATURA_BYTE, sizeof(int), 1, targetStreamBytes);
        fwrite(&VERSION, sizeof(int), 1, targetStreamBytes);
        fwrite(compilerInfo.commandsArr, sizeof(int), compilerInfo.size, targetStreamBytes);
    } _TX_ENDTRY
    _TX_CATCH {
    } _TX_ENDCATCH
    _TX_FINALLY {
        fclose(listingFile);
        fclose(targetStreamBytes);
    }

    return SUCCESS;
}

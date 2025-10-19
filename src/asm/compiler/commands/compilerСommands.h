#ifndef SRC_UTILS_STACK_COMMANDS_H
#define SRC_UTILS_STACK_COMMANDS_H

#include "common.h"
#include "compiler.h"

#define PRINT_ASM_LINE_ERR() \
    BEGIN \
        PRINTERR("AT LINE %d\nin file: ", compilerInfo->i + 1); \
        PRINTERR(ASM_SRC_PATH); \
        PRINTERR(":%d\n\n", compilerInfo->i + 1); \
    END

typedef struct compilerCmdInfo {
    const command_t command;
    const char *const commandStr;
    error_t (*function)(compilerInfo_t *compilerInfo);
    long long hash;
} compilerCommandInfo_t;

error_t noArgsCommand(compilerInfo_t *compilerInfo);

error_t getRegVal(compilerInfo_t *compilerInfo);

error_t regParam(compilerInfo_t *compilerInfo);

error_t pushCommand(compilerInfo_t *compilerInfo);

error_t jumpCommand(compilerInfo_t *compilerInfo);

error_t parseLabel(compilerInfo_t *compilerInfo);


#endif //SRC_UTILS_STACK_COMMANDS_H

#ifndef SRC_UTILS_STACK_COMMANDS_H
#define SRC_UTILS_STACK_COMMANDS_H

#include "common.h"
#include "compiler.h"

#define PRINT_ASM_LINE_ERR() \
    BEGIN \
        PRINTERR("AT LINE %d\nin file: ", compilerInfo->i + 1); \
        PRINTERR(ASM_SRC_PATH); \
        PRINTERR(":%d\n\n", compilerInfo->i+1); \
    END

typedef struct compilerCmdInfo {
    const command_t command;
    const char* const commandStr;
    error_t (*func)(compilerInfo_t* compilerInfo);
    const int argc;
} compilerCmdInfo_t;

error_t addNoArgsCommand(compilerInfo_t* compilerInfo);

error_t getRegVal(compilerInfo_t* compilerInfo);

error_t modifyReg(compilerInfo_t* compilerInfo);

error_t pushCmndAndValue(compilerInfo_t* compilerInfo);

error_t jumpCommand(compilerInfo_t* compilerInfo);

error_t parseLabel(compilerInfo_t* compilerInfo, char cmnd[MAX_COMMAND_LENGTH]);

const compilerCmdInfo_t compilerCommandsInfo[COMMANDS_COUNT] = {
    {ADD, "ADD", addNoArgsCommand, 0},
    {SUB, "SUB", addNoArgsCommand, 0},
    {MUL, "MUL", addNoArgsCommand, 0},
    {DIV, "DIV", addNoArgsCommand, 0},
    {SQRT, "SQRT", addNoArgsCommand, 0},
    {OUT, "OUT", addNoArgsCommand, 0},
    {PUSH, "PUSH", pushCmndAndValue, 1},
    {IN, "IN", addNoArgsCommand, 0},
    {PUSHREG, "PUSHREG", modifyReg, 1},
    {POPREG, "POPREG", modifyReg, 1},
    {JMP, "JMP", jumpCommand, 1},
    {JB, "JB", jumpCommand, 1},
    {JBE, "JBE", jumpCommand, 1},
    {JA, "JA", jumpCommand, 1},
    {JAE, "JAE", jumpCommand, 1},
    {JE, "JE", jumpCommand, 1},
    {JNE, "JNE", jumpCommand, 1},
    {CALL, "CALL", jumpCommand, 1},
    {RET, "RET", addNoArgsCommand, 0},
    {HLT, "HLT", addNoArgsCommand, 0},
};

#endif //SRC_UTILS_STACK_COMMANDS_H
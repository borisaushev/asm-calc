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

error_t noArgsCommand(compilerInfo_t* compilerInfo);

error_t getRegVal(compilerInfo_t* compilerInfo);

error_t regParam(compilerInfo_t* compilerInfo);

error_t pushCmndAndValue(compilerInfo_t* compilerInfo);

error_t jumpCommand(compilerInfo_t* compilerInfo);

error_t parseLabel(compilerInfo_t* compilerInfo, char cmnd[MAX_COMMAND_LENGTH]);

const compilerCmdInfo_t COMPILER_COMMANDS_INFO[COMMANDS_COUNT] = {
    {ADD, "ADD", noArgsCommand, 0},
    {SUB, "SUB", noArgsCommand, 0},
    {MUL, "MUL", noArgsCommand, 0},
    {DIV, "DIV", noArgsCommand, 0},
    {SQRT, "SQRT", noArgsCommand, 0},
    {OUT, "OUT", noArgsCommand, 0},
    {PUSH, "PUSH", pushCmndAndValue, 1},
    {IN, "IN", noArgsCommand, 0},
    {PUSHREG, "PUSHREG", regParam, 1},
    {POPREG, "POPREG", regParam, 1},
    {JMP, "JMP", jumpCommand, 1},
    {JB, "JB", jumpCommand, 1},
    {JBE, "JBE", jumpCommand, 1},
    {JA, "JA", jumpCommand, 1},
    {JAE, "JAE", jumpCommand, 1},
    {JE, "JE", jumpCommand, 1},
    {JNE, "JNE", jumpCommand, 1},
    {CALL, "CALL", jumpCommand, 1},
    {RET, "RET", noArgsCommand, 0},
    {PUSHMEM, "PUSHMEM", regParam, 1},
    {POPMEM, "POPMEM", regParam, 1},
    {DRAW, "DRAW", noArgsCommand, 0},
    {HLT, "HLT", noArgsCommand, 0},
};

#endif //SRC_UTILS_STACK_COMMANDS_H
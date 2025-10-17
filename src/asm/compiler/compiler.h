#ifndef SRC_UTILS_STACK_COMPILER_H
#define SRC_UTILS_STACK_COMPILER_H

#include <stdlib.h>

#include "common.h"
#include "line_reader.h"
#include "stack.h"

typedef struct compilerInfo {
    FILE* targetPr;
    pointer_array_buf_t* text;
    command_t command;
    int commandsArr[MAX_COMMANDS ];
    size_t size;
    size_t arrIndex;
    int i;
    char* line;
    int labels[MAX_LABELS];
    int unknownLabels;
    char regVal;
} compilerInfo_t;

typedef struct commandsInfo {
    command_t command;
    const char* const commandStr;
    error_t (*func)(compilerInfo_t* compilerInfo);
} commandsInfo_t;

error_t addCmnd(compilerInfo_t* compilerInfo);

error_t getRegVal(compilerInfo_t* compilerInfo);

error_t modifyReg(compilerInfo_t* compilerInfo);

error_t pushCmndAndValue(compilerInfo_t* compilerInfo);

error_t jump(compilerInfo_t* compilerInfo);

error_t openFiles(FILE *&targetPr, FILE *&targetStreamBytes);

error_t compile(pointer_array_buf_t* text);

error_t writeCommands(compilerInfo_t* compilerInfo);

const commandsInfo_t commandsFuncs[COMMANDS_COUNT] = {
    {ADD, "ADD", addCmnd},
    {SUB, "SUB", addCmnd},
    {MUL, "MUL", addCmnd},
    {DIV, "DIV", addCmnd},
    {SQRT, "SQRT", addCmnd},
    {OUT, "OUT", addCmnd},
    {PUSH, "PUSH", pushCmndAndValue},
    {IN, "IN", addCmnd},
    {PUSHREG, "PUSHREG", modifyReg},
    {POPREG, "POPREG", modifyReg},
    {JMP, "JMP", jump},
    {JB, "JB", jump},
    {JBE, "JBE", jump},
    {JA, "JA", jump},
    {JAE, "JAE", jump},
    {JE, "JE", jump},
    {JNE, "JNE", jump},
    {CALL, "CALL", jump},
    {RET, "RET", addCmnd},
    {HLT, "HLT", addCmnd},
};

#endif //SRC_UTILS_STACK_COMPILER_H
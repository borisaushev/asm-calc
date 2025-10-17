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
    const command_t command;
    const char* const commandStr;
    error_t (*func)(compilerInfo_t* compilerInfo);
    const int argc;
} commandsInfo_t;

error_t addCmnd(compilerInfo_t* compilerInfo);

error_t getRegVal(compilerInfo_t* compilerInfo);

error_t modifyReg(compilerInfo_t* compilerInfo);

error_t pushCmndAndValue(compilerInfo_t* compilerInfo);

error_t jump(compilerInfo_t* compilerInfo);

error_t openFiles(FILE** targetPr, FILE** targetStreamBytes);

error_t compile(pointer_array_buf_t* text);

error_t compile(compilerInfo_t* compilerInfo);

const commandsInfo_t commandsFuncs[COMMANDS_COUNT] = {
    {ADD, "ADD", addCmnd, 0},
    {SUB, "SUB", addCmnd, 0},
    {MUL, "MUL", addCmnd, 0},
    {DIV, "DIV", addCmnd, 0},
    {SQRT, "SQRT", addCmnd, 0},
    {OUT, "OUT", addCmnd, 0},
    {PUSH, "PUSH", pushCmndAndValue, 1},
    {IN, "IN", addCmnd, 0},
    {PUSHREG, "PUSHREG", modifyReg, 1},
    {POPREG, "POPREG", modifyReg, 1},
    {JMP, "JMP", jump, 1},
    {JB, "JB", jump, 1},
    {JBE, "JBE", jump, 1},
    {JA, "JA", jump, 1},
    {JAE, "JAE", jump, 1},
    {JE, "JE", jump, 1},
    {JNE, "JNE", jump, 1},
    {CALL, "CALL", jump, 1},
    {RET, "RET", addCmnd, 0},
    {HLT, "HLT", addCmnd, 0},
};

#endif //SRC_UTILS_STACK_COMPILER_H
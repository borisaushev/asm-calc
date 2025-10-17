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

error_t openFiles(FILE** targetPr, FILE** targetStreamBytes);

error_t compileAsm(pointer_array_buf_t* text);

error_t compile(compilerInfo_t* compilerInfo);

#endif //SRC_UTILS_STACK_COMPILER_H
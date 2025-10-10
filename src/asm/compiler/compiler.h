#ifndef SRC_UTILS_STACK_COMPILER_H
#define SRC_UTILS_STACK_COMPILER_H

#include <stdlib.h>

#include "common.h"
#include "line_reader.h"
#include "stack.h"

error_t openFiles(FILE *&targetPr, FILE *&targetStreamRough);

error_t compile(pointer_array_buf_t* text);

#endif //SRC_UTILS_STACK_COMPILER_H
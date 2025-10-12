#ifndef SRC_UTILS_STACK_COMPILER_H
#define SRC_UTILS_STACK_COMPILER_H

#include <stdlib.h>

#include "common.h"
#include "line_reader.h"
#include "stack.h"

error_t openFiles(FILE *&targetPr, FILE *&targetStreamBytes);

error_t compile(pointer_array_buf_t* text);

error_t writeCommands(pointer_array_buf_t* text, FILE* targetPr, int labels[MAX_LABELS],
                      int commandsArr[MAX_COMMANDS], size_t* size, size_t* unknownLabels);

#endif //SRC_UTILS_STACK_COMPILER_H
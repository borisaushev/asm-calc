#ifndef SRC_UTILS_STACK_PROCESSOR_H
#define SRC_UTILS_STACK_PROCESSOR_H

#include "common.h"
#include "stack.h"

error_info_t parseInts(const char* filename, int** ptr, int* count);

error_info_t funcOfTwo(stack_t* stack, int (*func) (int a, int b), const char* const funcName);

error_info_t runCmnds(stack_t* stack, int count, const int *commands);

#endif //SRC_UTILS_STACK_PROCESSOR_H
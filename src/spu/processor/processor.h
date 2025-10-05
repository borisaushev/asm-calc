#ifndef SRC_UTILS_STACK_PROCESSOR_H
#define SRC_UTILS_STACK_PROCESSOR_H

#include "common.h"
#include "stack.h"
#include "struct/processor_str.h"

error_info_t verifySignature(FILE *file);

error_info_t parseInts(const char* filename, int arr[MAX_COMMANDS], size_t* commandsCount);

error_info_t funcOfTwo(stack_t* stack, int (*func) (int a, int b), const char* const funcName);

error_info_t runCmnds(processor_t* processor);

void DPrintProcessor(processor_t *processor);

#endif //SRC_UTILS_STACK_PROCESSOR_H
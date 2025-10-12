#ifndef SRC_UTILS_STACK_PROCESSOR_H
#define SRC_UTILS_STACK_PROCESSOR_H

#include "common.h"
#include "stack.h"
#include "struct/processorStruct.h"

error_t parseCommands(const char* filename, int arr[MAX_COMMANDS], size_t* commandsCount);

error_t funcOfTwo(stack_t* stack, int (*func) (int a, int b), const char* const funcName);

error_t runCmnds(processor_t* processor);

void DPrintProcessor(processor_t *processor);

#endif //SRC_UTILS_STACK_PROCESSOR_H
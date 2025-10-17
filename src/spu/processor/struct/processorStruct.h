#ifndef SRC_UTILS_STACK_PROCESSOR_STR_H
#define SRC_UTILS_STACK_PROCESSOR_STR_H
#include "common.h"
#include "stack.h"


typedef struct processor {
    stack_t* valuesStack;
    int commands[MAX_COMMANDS];
    size_t commandsCount;
    size_t CP;
    int registerArr[REGISTER_SIZE];
    stack_t* callStack;
    int* RAM;
} processor_t;

error_t initProcessor(processor_t* processor, stack_t* valuesStack, const int commands[MAX_COMMANDS],
                      size_t commandsCount, stack_t* callStack);
error_t verifyProcessor(processor_t* processor);

void dumpProcessor(processor_t* processor);

void destroyProcessor(processor_t* processor);

void dumpProcessor(processor_t *processor, FILE* dumpFile);


#endif //SRC_UTILS_STACK_PROCESSOR_STR_H
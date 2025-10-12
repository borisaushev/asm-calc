#ifndef SRC_UTILS_STACK_PROCESSOR_STR_H
#define SRC_UTILS_STACK_PROCESSOR_STR_H
#include "common.h"
#include "stack.h"


typedef struct processor {
    stack_t* stack;
    size_t commandsCount;
    size_t CP;
    int commands[MAX_COMMANDS];
    int registerArr[REGISTER_SIZE];
} processor_t;

error_t initProcessor(processor_t* processor, stack_t* stack, const int commands[MAX_COMMANDS], size_t commandsCount);
error_t verifyProcessor(processor_t* processor);

void dumpProcessor(processor_t* processor);

void destroyProcessor(processor_t* processor);

void printProcessor(processor_t *processor, FILE* dumpFile);


#endif //SRC_UTILS_STACK_PROCESSOR_STR_H
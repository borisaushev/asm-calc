#ifndef SRC_UTILS_STACK_PROCESSOR_STR_H
#define SRC_UTILS_STACK_PROCESSOR_STR_H
#include "common.h"
#include "stack.h"


typedef struct processor {
    stack_t* stack;
    size_t commandsCount;
    size_t curI;
    int commands[MAX_COMMANDS];
    int registerArr[REGISTER_SIZE];
} processor_t;

error_info_t initProcessor(processor_t* processor, stack_t* stack, int commands[MAX_COMMANDS], size_t commandsCount);
error_info_t verifyProcessor(processor_t* processor);

void dumpProcessor(processor_t* processor);

void destroyProcessor(processor_t* processor);

void printProcessor(processor_t *processor, FILE* dumpFile);

#define PROCESSOR_VALID(processor) \
    BEGIN \
    error_info_t valid ## __LINE__ = verifyProcessor(processor); \
    if (valid ## __LINE__.err_code != SUCCESS) { \
        SAFE_CALL(dumpProcessor(stack, __FILE__, __LINE__, __func__, valid ## __LINE__)); \
        return valid ## __LINE__;\
    } \
    END


#endif //SRC_UTILS_STACK_PROCESSOR_STR_H
#ifndef SRC_UTILS_STACK_PROCESSOR_H
#define SRC_UTILS_STACK_PROCESSOR_H

#include "common.h"
#include "stack.h"

typedef struct spuCmnds {
    int** cmdArr;
    int commandsCount;
    int curI;
} spuCmnds_t;

typedef struct rgValues {
    int** valArr;
    int valCount;
} rgValues_t;

struct processor {
    stack_t* stack;
    spuCmnds_t* commands;
    rgValues_t* rgValues;
};

error_info_t verifySignature(FILE *file);

error_info_t parseInts(const char* filename, int** ptr, int* count);

error_info_t funcOfTwo(stack_t* stack, int (*func) (int a, int b), const char* const funcName);

error_info_t runCmnds(stack_t* stack, const int *commands, int count);

#endif //SRC_UTILS_STACK_PROCESSOR_H
#ifndef ASM_CALC_STACK_H
#define ASM_CALC_STACK_H

#include <algorithm>

#include "common.h"

typedef int element_t;
# define REG "%d"
const int POISON = -777777777;
const char CANARRAY = 'W';

typedef struct stack {
    element_t* array;
    size_t capacity;
    size_t elementCount;
#ifdef ENABLE_PRT
    element_t* cnr1;
    element_t* cnr2;
    size_t hash;
#endif
} stack_t;


//Юзать относительные пути
const char* const STACK_LOG_FILE = "..\\files\\logs\\stack.log";
const char* const PROC_LOG_FILE = "..\\files\\logs\\proc.log";
const size_t MAX_REASONABLE_CAPACITY = 1024 * 1024 * 1024;

error_t stackDump(const stack_t *stack, const char *file, int line, const char *function, error_t validation);

#define STACK_VALID(stack) \
BEGIN \
    error_t valid ## __LINE__ = validateStack(stack); \
    if (valid ## __LINE__ != SUCCESS) { \
        SAFE_CALL(stackDump(stack, __FILE__, __LINE__, __func__, valid ## __LINE__)); \
    return valid ## __LINE__;\
    } \
END

void initStack(stack_t* stack, size_t capacity);

error_t stackPop(stack_t* stack, element_t* element);

error_t stackPush(stack_t *stack, element_t element);

error_t fprintStack(FILE* file, stack_t* stack);

error_t printStack(stack_t* stack);

error_t DPrintStack(stack_t* stack);

error_t stackDestroy(stack_t* stack);

error_t validateStack(const stack_t* stack);

error_t stackDumpStream(const stack_t *stack, error_t validation, FILE* dumpFile,
                        const char *file, int line, const char *function);

long long strHash(const char *str);

#endif //ASM_CALC_STACK_H
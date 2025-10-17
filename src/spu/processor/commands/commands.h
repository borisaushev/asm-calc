#ifndef SRC_UTILS_STACK_COMMANDS_H
#define SRC_UTILS_STACK_COMMANDS_H
#include "common.h"
#include "processorStruct.h"

// Arithmetic operations
error_t spuDiv(processor_t* processor);
error_t spuAdd(processor_t* processor);
error_t spuMul(processor_t* processor);
error_t spuSub(processor_t* processor);
error_t spuSqrt(processor_t* processor);

// Stack operations
error_t spuOut(processor_t* processor);
error_t spuPush(processor_t* processor);
error_t spuIn(processor_t* processor);
error_t spuPushReg(processor_t* processor);
error_t spuPopReg(processor_t* processor);

// Jump operations
error_t spuJmp(processor_t* processor);
error_t spuJb(processor_t* processor);
error_t spuJbe(processor_t* processor);
error_t spuJa(processor_t* processor);
error_t spuJae(processor_t* processor);
error_t spuJe(processor_t* processor);
error_t spuJne(processor_t* processor);

// Function call operations
error_t spuCall(processor_t* processor);
error_t spuRet(processor_t* processor);

#endif //SRC_UTILS_STACK_COMMANDS_H
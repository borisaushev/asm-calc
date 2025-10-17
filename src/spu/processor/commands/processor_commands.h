#ifndef SRC_UTILS_STACK_COMMANDS_H
#define SRC_UTILS_STACK_COMMANDS_H
#include "common.h"
#include "processorStruct.h"

typedef struct processorCmdInfo {
    const command_t command;
    error_t (*func)(processor_t* processor);
} processorCmdInfo_t;

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

//RAM
error_t spuPushMem(processor_t* processor);
error_t spuPopMem(processor_t* processor);
error_t spuDraw(processor_t* processor);

const processorCmdInfo_t processorCommandsInfo[COMMANDS_COUNT] = {
    {ADD, spuAdd},
    {SUB, spuSub},
    {MUL, spuMul},
    {DIV, spuDiv},
    {SQRT, spuSqrt},
    {OUT, spuOut},
    {PUSH, spuPush},
    {IN, spuIn},
    {PUSHREG, spuPushReg},
    {POPREG, spuPopReg},
    {JMP, spuJmp},
    {JB,  spuJb},
    {JBE, spuJbe},
    {JA,  spuJa},
    {JAE, spuJae},
    {JE,  spuJe},
    {JNE, spuJne},
    {CALL, spuCall},
    {RET, spuRet},
    {PUSHMEM, spuPushMem},
    {POPMEM, spuPopMem},
    {DRAW, spuDraw},
    {HLT, NULL}
};

#endif //SRC_UTILS_STACK_COMMANDS_H
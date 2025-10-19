#ifndef SRC_UTILS_STACK_COMMANDS_H
#define SRC_UTILS_STACK_COMMANDS_H

#include "common.h"
#include "compiler.h"

#define PRINT_ASM_LINE_ERR() \
    BEGIN \
        PRINTERR("AT LINE %d\nin file: ", compilerInfo->i + 1); \
        PRINTERR(ASM_SRC_PATH); \
        PRINTERR(":%d\n\n", compilerInfo->i + 1); \
    END

typedef struct compilerCmdInfo {
    const command_t command;
    const char *const commandStr;

    error_t (*function)(compilerInfo_t *compilerInfo);
} compilerCmdInfo_t;

error_t noArgsCommand(compilerInfo_t *compilerInfo);

error_t getRegVal(compilerInfo_t *compilerInfo);

error_t regParam(compilerInfo_t *compilerInfo);

error_t pushCommand(compilerInfo_t *compilerInfo);

error_t jumpCommand(compilerInfo_t *compilerInfo);

error_t parseLabel(compilerInfo_t *compilerInfo);

const compilerCmdInfo_t COMPILER_COMMANDS_INFO[COMMANDS_COUNT] = {
    {ADD,     "ADD",     noArgsCommand},
    {SUB,     "SUB",     noArgsCommand},
    {DIV,     "DIV",     noArgsCommand},
    {MUL,     "MUL",     noArgsCommand},
    {SQRT,    "SQRT",    noArgsCommand},
    {OUT,     "OUT",     noArgsCommand},
    {IN,      "IN",      noArgsCommand},
    {PUSH,    "PUSH",    pushCommand  },
    {POPREG,  "POPREG",  regParam     },
    {PUSHREG, "PUSHREG", regParam     },
    {JMP,     "JMP",     jumpCommand  },
    {JB,      "JB",      jumpCommand  },
    {JBE,     "JBE",     jumpCommand  },
    {JA,      "JA",      jumpCommand  },
    {JAE,     "JAE",     jumpCommand  },
    {JE,      "JE",      jumpCommand  },
    {JNE,     "JNE",     jumpCommand  },
    {CALL,    "CALL",    jumpCommand  },
    {RET,     "RET",     noArgsCommand},
    {PUSHMEM, "PUSHMEM", regParam     },
    {POPMEM,  "POPMEM",  regParam     },
    {DRAW,    "DRAW",    noArgsCommand},
    {HLT,     "HLT",     noArgsCommand},
};

#endif //SRC_UTILS_STACK_COMMANDS_H

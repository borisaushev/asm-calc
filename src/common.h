#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <io.h>
#include <math.h>
#include <cassert>
#include <string.h>
#include <sys/stat.h>
#include <filesystem>

typedef enum errors {
    SUCCESS = 0,
    NULL_PTR,
    INVALID_CAPACITY,
    INVALID_INDEX,
    INVALID_PTR,
    CONTENTS_MODIFIED,
    CANT_OPEN_FILE,
    FILE_NOT_FOUND,
    FILE_NOT_READABLE,
    INVALID_INPUT
} error_t;

typedef enum commands {
    ADD = 1,
    SUB = 2,
    DIV = 3,
    MUL = 4,
    SQRT = 5,
    OUT = 6,
    IN = 7, // считать с консоли в стек
    PUSH = 8, // в стек
    POPREG = 9, // POPREG AX - записать в регистр значение stackPop()
    PUSHREG = 10, // PUSHREG AX - сделать push() в стек значения AX
    JMP = 11,
    JB = 12,
    JBE = 13,
    JA = 14,
    JAE = 15,
    JE = 16,
    JNE = 17,
    CALL = 18,
    RET = 19,
    PUSHMEM = 20,
    POPMEM = 21,
    DRAW = 22,
    HLT = 32,
} command_t;
const int COMMANDS_COUNT = 23;

// const char* const ASM_SRC_PATH = "..\\files\\kvadratka.asm";
const char* const ASM_SRC_PATH = "..\\files\\factorial.asm";
// const char* const ASM_SRC_PATH = "..\\files\\circle.asm";

const char* const LISTING_PATH = "..\\files\\listing.lst";
const char* const BYTECODE_PATH = "..\\files\\bytecode.bbc";

const int VERSION = 23;
const char* const SIGNATURA = "BB";
const int SIGNATURA_BYTE = 0xBB;
const int SIGNATURA_SIZE = 2;

const int MAX_COMMAND_LENGTH = 100;
const int MAX_COMMANDS = 1024;
const int MAX_LABELS = 10;

const int REGISTER_SIZE = 10;
const int RAM_SIZE = 15*15;

const int STACK_BASE_SIZE = 100;

#define BEGIN do {
#define END   } while (0);

//DEBUG printf with immediate flush
#ifdef DEBUG
#include "line_reader.h"
#define DPRINTF(...) \
    BEGIN \
        printYellow(__VA_ARGS__); \
    END
#else
    #define DPRINTF(...) ;
#endif //DEBUG

//define for printing in stderr
#define PRINTERR(...) \
    BEGIN \
        fprintf(stderr, "\033[31m"); \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\033[0m"); \
    END

#define RETURN_ERR(code, desc) \
    BEGIN \
        PRINTERR("ERROR [%s:%d]: %s (code %d)\n", __FILE__, __LINE__, desc, code); \
        return code; \
    END

#define SAFE_CALL(func) \
    BEGIN \
        error_t sf_call ## __LINE__ = (func); \
        if (sf_call ## __LINE__ != SUCCESS) { \
            return sf_call ## __LINE__; \
        } \
    END


#define FREE_ALL(...) \
    BEGIN \
    void* _ptrs[] = { __VA_ARGS__ }; \
    for (size_t _i = 0; _i < sizeof(_ptrs) / sizeof(_ptrs[0]); _i++) { \
        free(_ptrs[_i]); \
    _ptrs[_i] = NULL; \
    } \
    END

#endif //COMMON_H


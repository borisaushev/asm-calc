#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <io.h>
#include <math.h>
#include <cassert>
#include <string.h>
#include <sys/stat.h>
#include <filesystem>

// TODO сделать отдельные enum-ы для разных мест
// TODO сделать массив структур, в процессоре оставить switch
// Можно сделать отдельную функцию для арифметики и также для джампов и т.д.
// Есть команда DRAW

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
    SUB,
    DIV,
    MUL,
    SQRT,
    OUT,
    IN,
    PUSH,
    POPREG,
    PUSHREG,
    JMP,
    JB,
    JBE,
    JA,
    JAE,
    JE,
    JNE,
    CALL,
    RET,
    HLT,
} command_t;

// const char* const ASM_SRC_PATH = "..\\files\\examples\\JNE.asm";
const char* const ASM_SRC_PATH = "..\\files\\factorial.asm";
const char* const BYTECODE_PR_PATH = "..\\files\\listing.lst";
const char* const BYTECODE_PATH = "..\\files\\bytecode.bbc";

const int VERSION = 16;
const char* const SIGNATURA = "BB";
const int SIGNATURA_BYTE = 0xBB;
const int SIGNATURA_SIZE = 2;
const int REGISTER_SIZE = 10;

const int MAX_COMMAND_LENGTH = 100;
const int MAX_COMMANDS = 1024;
const int MAX_LABELS = 10;

#define BEGIN do {
#define END   } while (0);

//DEBUG printf with immediate flush
#ifdef DEBUG
#define DPRINTF(...) \
    BEGIN \
        printf("\033[33m"); \
        printf(__VA_ARGS__); \
        printf("\033[0m"); \
        fflush(stdout); \
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


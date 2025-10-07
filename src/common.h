#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <io.h>
#include <math.h>
#include <cassert>
#include <string.h>
#include <sys/stat.h>
#include <filesystem>

//TODO сделать отдельные enum-ы для разных мест
//TODO добавить вывод строки где был дамп процессора

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
} error_code_t;

typedef struct error_info {
    error_code_t err_code;
    const char* const msg;
    int line;
    const char* file;
} error_info_t;

//TODO написать квадратку с разбором всех случаев (для a!=0, для a=1 выводим -1) вначале печатает кол-во корней потом если есть, то корни
// Потом будем решать все виды
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
    CP, // set given reg to next command (CP AX)
    JMP, // jump to given reg (JMP AX)
    JB,
    JBE,
    JA,
    JAE,
    JE,
    JNE,
    HLT,
} command_t;

// const char* const ASM_SRC_PATH = "..\\files\\examples\\JNE.asm";
const char* const ASM_SRC_PATH = "..\\files\\source.asm";
const char* const BYTECODE_PR_PATH = "..\\files\\bytecode_pretty.txt";
const char* const BYTECODE_PATH = "..\\files\\bytecode.bbc";

const int VERSION = 11;
const char* const SIGNATURA = "BB";
const int REGISTER_SIZE = 10;

const int MAX_COMMANDS = 1024;

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
        PRINTERR("ERROR: %s\n", desc); \
        return {code, desc, __LINE__, __FILE__}; \
    END

#define SAFE_CALL(func) \
    BEGIN \
        error_info_t sf_call ## __LINE__ = (func); \
        if (sf_call ## __LINE__.err_code != SUCCESS) { \
            return sf_call ## __LINE__; \
        } \
    END
#endif //COMMON_H


#define FREE_ALL(...) \
    BEGIN \
    void* _ptrs[] = { __VA_ARGS__ }; \
    for (size_t _i = 0; _i < sizeof(_ptrs) / sizeof(_ptrs[0]); _i++) { \
        free(_ptrs[_i]); \
    _ptrs[_i] = NULL; \
    } \
    END

#define RETURN_ON_ERR(func, ...) \
    BEGIN \
    error_info_t callResult = (func); \
    if(callResult.err_code != SUCCESS) { \
        DPRINTF("ERROR [%s:%d]: %s (code %d)\n", callResult.file, callResult.line, callResult.msg, callResult.err_code); \
        FREE_ALL(__VA_ARGS__); \
        return callResult.err_code; \
    } \
    END

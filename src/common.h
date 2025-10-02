#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <fcntl.h>
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
} error_code_t;

typedef struct error_info {
    error_code_t err_code;
    const char *msg;
} error_info_t;

typedef enum commands {
    ADD = 1,
    SUB,
    DIV,
    MUL,
    OUT,
    PUSH,
    HLT,
} command_t;

const char* const ASM_SRC_PATH = "C:\\Users\\bossb\\CLionProjects\\asm_calc\\files\\source.asm";
const char* const BYTECODE_PR_PATH = "C:\\Users\\bossb\\CLionProjects\\asm_calc\\files\\bytecode_pretty.txt";
const char* const BYTECODE_PATH = "C:\\Users\\bossb\\CLionProjects\\asm_calc\\files\\bytecode.bbc";


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
#define PRINTERR(...) fprintf(stderr, __VA_ARGS__)

#define SAFE_CALL(func) \
    BEGIN \
        error_info_t sf_call ## __LINE__ = (func); \
        if (sf_call ## __LINE__.err_code != SUCCESS) { \
            PRINTERR("ERROR [%s:%d]: %s (code %d)\n", \
            __FILE__, __LINE__, sf_call ## __LINE__.msg, sf_call ## __LINE__.err_code); \
            exit(EXIT_FAILURE); \
        } \
    END

#endif //COMMON_H

#include "compiler.h"
#include "line_reader.h"

int main(int argc, char *argv[]) {
    pointer_array_buf_t text = {};
    error_t error = SUCCESS;
    if(argc == 2) {
        error = parseText(argv[1], &text);
        if (error != SUCCESS) {
            FREE_ALL(text.buf, text.pointer_arr);
            return INVALID_INPUT;
        }
    }
    else {
        if (parseText(ASM_SRC_PATH, &text) != SUCCESS) {
            FREE_ALL(text.buf, text.pointer_arr);
            return INVALID_INPUT;
        }
    }
    #ifdef DEBUG
        printf("input from %s:\n", ASM_SRC_PATH);
        printPtrArray(stdout, &text);
    #endif

    if (compile(&text) != SUCCESS) {
        FREE_ALL(text.buf, text.pointer_arr);
    }

    printf("\ncompilation finished\n");
    printf("pretty output is written to file %s\n", BYTECODE_PR_PATH);
    printf("harsh-truth output is written to file %s\n", BYTECODE_PATH);
}
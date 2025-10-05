#include "compiler.h"
#include "line_reader.h"

int main() {
    pointer_array_buf_t text = {};

    RETURN_ON_ERR(parseText(ASM_SRC_PATH, &text), text.buf, text.pointer_arr);

    printf("input from %s:\n", ASM_SRC_PATH);
    printPtrArray(stdout, &text);

    RETURN_ON_ERR(compile(&text), text.buf, text.pointer_arr);

    printf("\ncompilation finished\n");
    printf("pretty output is written to file %s\n", BYTECODE_PR_PATH);
    printf("harsh-truth output is written to file %s\n", BYTECODE_PATH);
}

#include "compiler.h"
#include "line_reader.h"

int main() {
    pointer_array_buf_t text = {};

    SAFE_CALL(parseText(ASM_SRC_PATH, &text));
    printf("input from %s:\n", ASM_SRC_PATH);
    printPtrArray(stdout, &text);

    SAFE_CALL(compile(&text));

    printf("\ncompilation finished\n");
    printf("pretty output is written to file %s\n", BYTECODE_PR_PATH);
    printf("harsh-truth output is written to file %s\n", BYTECODE_PATH);
}

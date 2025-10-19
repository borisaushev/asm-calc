#include "compiler.h"
#include "line_reader.h"

int main(int argc, char *argv[]) {
    pointer_array_buf_t text = {};
    error_t error = SUCCESS;
    _TX_TRY {
        if(argc == 2) {
            error = parseText(argv[1], &text);
            if (error != SUCCESS) {
                _TX_FAIL(error);
            }
        }
        else {
            if (parseText(ASM_SRC_PATH, &text) != SUCCESS) {
                _TX_FAIL(error);
            }
        }
        #ifdef DEBUG
            printf("input from %s:\n", ASM_SRC_PATH);
            printPtrArray(stdout, &text);
        #endif
        error = compileAsm(&text);
        if (error != SUCCESS) {
            _TX_FAIL(error);
        }

        printf("\ncompilation finished\n");
        printf("listing is written to file %s\n", LISTING_PATH);
        printf("byte output is written to file %s\n", BYTECODE_PATH);
    } _TX_ENDTRY
    _TX_CATCH {}
    _TX_FINALLY {
        free(text.buf);
        free(text.pointer_arr);
    }
}
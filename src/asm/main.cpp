#include "compiler.h"
#include "line_reader.h"

int main(int argc, char *argv[]) {
    pointer_array_buf_t text = {};
    _TX_TRY {
        if(argc == 2) {
            _TX_CHECKED(parseText(argv[1], &text));
        }
        else {
            _TX_CHECKED(parseText(ASM_SRC_PATH, &text));
        }
        #ifdef DEBUG
            printf("input from %s:\n", ASM_SRC_PATH);
            printPtrArray(stdout, &text);
        #endif
        _TX_CHECKED(compileAsm(&text));

        printf("\ncompilation finished\n");
        printf("listing is written to file %s\n", LISTING_PATH);
        printf("byte output is written to file %s\n", BYTECODE_PATH);
    } _TX_ENDTRY
    _TX_CATCH {
    } _TX_ENDCATCH
    _TX_FINALLY {
        free(text.buf);
        free(text.pointer_arr);
    }

    return 0;
}
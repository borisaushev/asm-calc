#include <stdlib.h>

#include "common.h"
#include "utils/reader/line_reader.h"
#include "utils/stack/stack.h"

typedef enum commands {
    ADD = 1,
    SUB,
    DIV,
    MUL,
    OUT,
    PUSH,
    HLT,
} command_t;

void appendCommandPretty(char* dest, command_t command) {
    char buf[10] = {};
    itoa(command, buf, 10);
    strcat(dest, buf);
    strcat(dest, "\n");
}

int main() {
    const char* const src_file = "C:\\Users\\bossb\\CLionProjects\\asm_calc\\files\\source.asm";
    const char* const target_file_pretty = "C:\\Users\\bossb\\CLionProjects\\asm_calc\\files\\bytecode_pretty.bbc";
    const char* const target_file_rough = "C:\\Users\\bossb\\CLionProjects\\asm_calc\\files\\bytecode.bbc";
    pointer_array_buf_t text = {};

    SAFE_CALL(parseText(src_file, &text));
    printPtrArray(stdout, &text);

    int i = 0;
    int arrIndex = 0;
    char* line = text.pointer_arr[i].ptr;
    char pretty[1024] = {};
    int rough[1024] = {};
    while (strcmp(line, "HLT") != 0) {
        DPRINTF("read line: '%s'\n", line);
        if (strcmp(line, "ADD") == 0) {
            appendCommandPretty(pretty, ADD);
            rough[arrIndex] = ADD;
        }
        else if (strcmp(line, "SUB") == 0) {
            appendCommandPretty(pretty, SUB);
            rough[arrIndex] = SUB;
        }
        else if (strcmp(line, "MUL") == 0) {
            appendCommandPretty(pretty, MUL);
            rough[arrIndex] = MUL;
        }
        else if (strcmp(line, "DIV") == 0) {
            appendCommandPretty(pretty, DIV);
            rough[arrIndex] = DIV;
        }
        else if (strcmp(line, "OUT") == 0) {
            appendCommandPretty(pretty, OUT);
            rough[arrIndex] = OUT;
        }
        else if (strncmp(line, "PUSH", 4) == 0) {
            char buf[10] = {};
            itoa(PUSH, buf, 10);
            strcat(pretty, buf);
            strcat(pretty, " ");

            int pushVal = POISON;
            int scanCount = sscanf(line, "PUSH %d", &pushVal);
            if (scanCount != 1) {
                PRINTERR("invalid push value at line %d\n", i);
                break;
            }
            DPRINTF("pushVal: %d\n", pushVal);

            itoa(pushVal, buf, 10);
            strcat(pretty, buf);

            rough[arrIndex++] = PUSH;
            rough[arrIndex] = pushVal;
        }
        else {
            PRINTERR("invalid command at line %d\n", i);
            break;
        }

        arrIndex++;
        line = text.pointer_arr[++i].ptr;
    }
    appendCommandPretty(pretty, HLT, "HLT");
    DPRINTF("the compiler meets its destiny\n");
    DPRINTF("result byte code: \n'%s'\n", pretty);

    FILE* targetStreamPretty = fopen(target_file_pretty, "wb");
    fputs(pretty, targetStreamPretty);

    FILE* targetStreamRough = fopen(target_file_rough, "wb");
    fwrite(rough, sizeof(int), arrIndex, targetStreamRough);
}

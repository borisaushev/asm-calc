#include "compiler.h"

error_info_t openFiles(FILE *&targetPr, FILE *&targetStreamRough) {
    targetPr = fopen(BYTECODE_PR_PATH, "wb");
    targetStreamRough = fopen(BYTECODE_PATH, "wb");

    if (!targetStreamRough || !targetPr) {
        return {FILE_NOT_FOUND, "could not open file"};
    }

    return {SUCCESS};
}

error_info_t compile(pointer_array_buf_t* text) {
    FILE *targetPr;
    FILE *targetStreamRough;
    SAFE_CALL(openFiles(targetPr, targetStreamRough));

    int i = 0;
    size_t arrIndex = 0;
    char* line = strupr(text->pointer_arr[i].ptr);
    int* rough = (int*) calloc(text->lines_count * 2, sizeof(int));
    while (stricmp(line, "HLT") != 0 && i < text->lines_count) {
        DPRINTF("read line: '%s'\n", line);
        if (strncmp(line, "ADD", 3) == 0) {
            fprintf(targetPr, "%d\n", ADD);
            rough[arrIndex] = ADD;
        }
        else if (strncmp(line, "SUB", 3) == 0) {
            fprintf(targetPr, "%d\n", SUB);
            rough[arrIndex] = SUB;
        }
        else if (strncmp(line, "MUL", 3) == 0) {
            fprintf(targetPr, "%d\n", MUL);
            rough[arrIndex] = MUL;
        }
        else if (strncmp(line, "DIV", 3) == 0) {
            fprintf(targetPr, "%d\n", DIV);
            rough[arrIndex] = DIV;
        }
        else if (strncmp(line, "OUT", 3) == 0) {
            fprintf(targetPr, "%d\n", OUT);
            rough[arrIndex] = OUT;
        }
        else if (strncmp(line, "PUSH", 4) == 0) {
            int pushVal = POISON;
            int scanCount = sscanf(line, "PUSH %d", &pushVal);
            if (scanCount != 1) {
                PRINTERR("invalid push value at line %d\n", i);
                free(rough);
                return {INVALID_INPUT, "invalid push value"};
            }
            DPRINTF("pushVal: %d\n", pushVal);

            fprintf(targetPr, "%d %d\n", PUSH, pushVal);

            rough[arrIndex++] = PUSH;
            rough[arrIndex] = pushVal;
        }
        else {
            PRINTERR("invalid command at line %d\n", i);
            free(rough);
            return {INVALID_INPUT, "invalid command"};
        }

        arrIndex++;
        line = text->pointer_arr[++i].ptr;
    }
    if (strcmp(line, "HLT") != 0) {
        PRINTERR("PROGRAMM IS NOT FINITE, POSSIBLE TIME CURVATURE OF SPACE AND TIME\n");
        free(rough);
        return {INVALID_INPUT, "PROGRAMM IS NOT FINITE"};
    }

    fprintf(targetPr, "%d\n", HLT);
    DPRINTF("the compiler meets its destiny\n");

    fwrite(rough, sizeof(int), arrIndex, targetStreamRough);

    free(rough);
    fclose(targetPr);
    fclose(targetStreamRough);

    return {SUCCESS};
}

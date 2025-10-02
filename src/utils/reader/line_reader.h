#ifndef ONEGIN_LINEREADER_H
#define ONEGIN_LINEREADER_H

#include "common.h"


/**
 * structure containing ptr to the first symbol of line and the length of the line
 */
typedef struct ptr_wrap {
    char* ptr;
    int len;
} ptr_wrap_t;

/**
 * structure containing array of ptr-wraps, a text as an array, and a number of lines in array
 */
typedef struct ptr_array_buf {
    ptr_wrap* pointer_arr;
    char* buf;
    int lines_count;
} pointer_array_buf_t;

/**
 * parses text and sets pointers to each line start
 * @param file_path path to source file
 * @param arr_ptr pointer to pointer array struct
 * @return zero if all fine
 */
error_info_t parseText(const char *file_path, pointer_array_buf_t *arr_ptr);

/**
 * prints given pointer array line-by-line
 * @param output output stream
 * @param ptr_array ptr array to print
 */
void printPtrArray(FILE* output, ptr_array_buf* ptr_array);

/**
 * prints the whole buffer at once
 * @param output output stream
 * @param ptr_array pointer array
 */
void printBuf(FILE* output, ptr_array_buf* ptr_array);


/**
 * prints all ptr->len symbols and a \n at the end
 * @param output output stream
 * @param ptr_wrap ptr wrap to print
 */
void myPrintf(FILE* output, ptr_wrap_t ptr_wrap);

long getFileSize(const char* filename);

#endif //ONEGIN_LINEREADER_H
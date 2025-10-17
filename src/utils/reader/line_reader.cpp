#include "line_reader.h"

#include <cstdarg>
#include <fcntl.h>

#include "common.h"

void printYellow(const char* const format, ...) {
    printf("\033[33m");

    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    printf("\033[0m");

    fflush(stdout);
}

long getFileSize(const char* filename) {
    assert(filename);
    struct stat st = {};
    if (stat(filename, &st) == 0) {
        return st.st_size;
    }
    else {
        return -1;
    }
}

static void parsePointers(char *text, int ptr_count, ptr_wrap_t** ptr_array) {
    assert(text);
    assert(ptr_array);
    assert(*ptr_array);

    char* curptr = &text[0];
    char* nextLine = NULL;
    for (int i = 0; i < ptr_count; i++) {
        nextLine = strchr(curptr, '\n');
        assert(nextLine);
        assert(nextLine >= curptr);

        *nextLine = '\0';
        int len = (int) (nextLine - curptr);
        (*ptr_array)[i] = {.ptr=curptr, .len=len};
        curptr = nextLine + 1;
    }
}

static error_t readFile(const char *file_path, char** text, int* bytes_read) {
    assert(text);
    assert(file_path);
    assert(bytes_read);

    int stream = open(file_path, O_RDONLY);

    int file_size = getFileSize(file_path);
    DPRINTF("file size: %d\n", file_size);

    if (file_size < 0) {
        PRINTERR("Could not open file %s\n", file_path);
        RETURN_ERR(FILE_NOT_FOUND, "Could not open file");
    }
    *text = (char *) calloc(file_size, sizeof(char));
    *bytes_read = read(stream, *text, file_size);

    if (*bytes_read == -1) {
        PRINTERR("Could not read file %s with err: %s\n", file_path, strerror(errno));
        RETURN_ERR(FILE_NOT_READABLE, "Could not read file");
    }
    DPRINTF("Read %d bytes\n", *bytes_read);
    close(stream);

    *text = (char *) realloc(*text,  *bytes_read + 2);
    (*text)[*bytes_read] = (*text)[*bytes_read-1] == '\n' ? '\0' : '\n';
    (*text)[*bytes_read + 1] = '\0';

    return SUCCESS;
}

static void countLines(const char *text, int bytes_read, int* ptr_count) {
    assert(text);
    assert(ptr_count);

    for (int i = 0; i < bytes_read+1; i++) {
        if (text[i] == '\n') {
            (*ptr_count)++;
        }
    }
}

error_t parseText(const char *file_path, pointer_array_buf_t *arr_ptr) {
    assert(file_path);
    assert(arr_ptr);

    char *text = NULL;
    int bytes_read = -1;
    SAFE_CALL(readFile(file_path, &text, &bytes_read));

    int ptr_count = 0;
    countLines(text, bytes_read, &ptr_count);

    ptr_wrap_t* ptr_array = (ptr_wrap_t*) calloc((size_t) ptr_count, sizeof(ptr_wrap_t));
    parsePointers(text, ptr_count, &ptr_array);

    assert(text[bytes_read + 1] == '\0');
    DPRINTF("read %d lines from file %s\n", ptr_count, file_path);

    arr_ptr->pointer_arr = ptr_array;
    arr_ptr->buf = text;
    arr_ptr->lines_count = ptr_count;

    return SUCCESS;
}

void printPtrArray(FILE* output, ptr_array_buf* ptr_array) {
    assert(ptr_array);
    assert(ptr_array->pointer_arr);
    assert(output);

    DPRINTF("PRINTING POINTER ARRAY\n");
    for (int i = 0; i < ptr_array->lines_count; i++) {
        DPRINTF("line %d: ", i);
        fprintf(output, "%s\n", ptr_array->pointer_arr[i].ptr);
    }

    fflush(output);
}

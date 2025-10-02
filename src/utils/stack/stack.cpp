#include "stack.h"

#include <cassert>
#include <cstdio>

#include "common.h"

error_info_t validateStack(const stack_t* stack) {
    DPRINTF("=== STACK VALIDATION STARTED ===\n");

    if (stack == NULL) {
        return {NULL_PTR, "null stack"};
    }
    if (stack->array == NULL) {
        return {NULL_PTR, "null array"};
    }

    if (stack->capacity > MAX_REASONABLE_CAPACITY) {
        return {INVALID_CAPACITY, "stack size is too big"};
    }

    // Check index bounds
    if ((size_t)stack->elementCount > stack->capacity || stack->elementCount > MAX_REASONABLE_CAPACITY) {
        return {INVALID_INDEX, "index too big"};
    }

    if (*(stack->cnr1) != CANARRAY) {
        return {CONTENTS_MODIFIED, "first canarray is modified"};
    }
    if (*(stack->cnr2) != CANARRAY) {
        return {CONTENTS_MODIFIED, "second canarray is modified"};
    }

    DPRINTF("=== STACK VALIDATION PASSED ===\n");
    return {SUCCESS};
}

static void dumpData(FILE* output, const stack_t *stack) {
    fprintf(output, "\t\t\tcnr1: ptr: '%p' val: '%c' or '%d'; cnr2: ptr: '%p' val: '%c' or '%d'\n",
        stack->cnr1, *stack->cnr1, *stack->cnr1, stack->cnr2, *stack->cnr2, *stack->cnr2);
    fprintf(output, "\t\t\tcapacity: %llu\n", stack->capacity);
    if (stack->array == NULL) {
        fprintf(output, "\t\t\t[NULL DATA POINTER!]\n");
    } else if (stack->elementCount <= 0) {
        fprintf(output, "\t\t\t[empty]\n");
    } else {
        if ((size_t) stack->elementCount > stack->capacity) {
            fprintf(output, "\t\t\t--- MEMORY CORRUPTION DETECTED ---\n");
            size_t max = stack->elementCount > MAX_REASONABLE_CAPACITY ? 10 : stack->elementCount;
            for (size_t i = 0; i < max; i++) {
                fprintf(output, "\t\t\t**el[%zu]: " REG "\n", i, stack->array[i]);
            }
        }
        else {
            for (size_t i = 0; i < stack->elementCount; i++) {
                fprintf(output, "\t\t\tel[%zu]: " REG "\n", i, stack->array[i]);
            }
            for (size_t i = stack->elementCount; i < stack->capacity; i++) {
                fprintf(output, "\t\t\t*el[%zu]: " REG "\n", i, stack->array[i]);
            }
        }
    }
}

error_info_t stackDump(const stack_t *stack,
                       const char *file,
                       int line,
                       const char *function,
                       error_info_t validation) {

    FILE* dump_file = fopen(LOG_FILE, "a");
    if (dump_file == NULL) {
        // Если не удалось открыть файл, пишем только в консоль
        DPRINTF("StackDump() {\n");
        DPRINTF("\tCannot open dump file!\n");

        return {CANT_OPEN_FILE, "dump file not found"};
    }

    fprintf(dump_file, "StackDump() {\n");

    if (stack == NULL) {
        fprintf(dump_file, "\tStack [NULL] { ERROR! NULL POINTER\n\t}\n}\n");
        fclose(dump_file);
        return {NULL_PTR, "null pointer in dump"};
    }

    const char *status = (validation.err_code == SUCCESS) ? "" : "  ERROR! ";
    const char *error_msg = (validation.err_code != SUCCESS) ? validation.msg : "";

    fprintf(dump_file, "\t%s  [%p] {%s%s  \n\t\t\tfrom %s() at %s:%d\n",
            "stack", (void*)stack, status, error_msg, function, file, line);

    fprintf(dump_file, "\t\telements count = %zu", stack->elementCount);

    if (stack->elementCount > stack->capacity) {
        fprintf(dump_file, " (SIZE EXCEEDS CAPACITY!)");
    }
    fprintf(dump_file, ";\n");

    fprintf(dump_file, "\t\tdata[%p] {\n", (void*)stack->array);

    dumpData(dump_file, stack);

    fprintf(dump_file, "\t\t}\n\t}\n}\n");

    fclose(dump_file);


    return {SUCCESS};
}

void initStack(stack_t* stack, size_t capacity) {
    assert(stack);
    DPRINTF("initializing stack\n");
    stack->capacity = capacity;
    stack->elementCount = 0;

    char* mem = (char*) calloc(2 + sizeof(element_t)*capacity, sizeof(char)); //защита канарейкой
    stack->cnr1 = mem;
    stack->cnr2 = mem + (stack->capacity + 1)*sizeof(element_t);

    *(stack->cnr1) = CANARRAY;
    *(stack->cnr2) = CANARRAY;
    stack->array = (element_t*) (void*) (mem+1);
    for (size_t i = 0; i < capacity; i++) {
        stack->array[i] = POISON;
    }

    printf("after init: \n");
    printStack(stack);

    DPRINTF("stack init with poison values and canarrays\n");
}

error_info_t stackPop(stack_t* stack, element_t* element) {
    assert(stack);
    DPRINTF("---\nbefore pop: elcount: %llu; capacity: %llu\n", stack->elementCount, stack->capacity);

    STACK_VALID(stack);

    if (stack->elementCount == 0) {
        DPRINTF("stack empty, returning poison\n");
        *element = POISON;
        return {SUCCESS};
    }

    element_t result = stack->array[stack->elementCount - 1];
    stack->array[stack->elementCount - 1] = POISON;
    stack->elementCount--;

    STACK_VALID(stack);

    DPRINTF("after pop: elcount: %llu; capacity: %llu\n", stack->elementCount, stack->capacity);
    *element = result;

    printf("after pop: \n");
    printStack(stack);

    return {SUCCESS};
}

error_info_t stackPush(stack_t *stack, element_t element) {
    assert(stack);
    DPRINTF("---\nbefore push: elcount: %llu; capacity: %llu\n", stack->elementCount, stack->capacity);

    STACK_VALID(stack);

    if (stack->elementCount >= stack->capacity) {
        size_t newCapacity = stack->capacity * 2; //защита канарейкой
        DPRINTF("stack full, reallocating to size: %llu\n", newCapacity);
        stack->capacity = newCapacity;
        char* mem = (char*) realloc(stack->cnr1, 2 + sizeof(element_t)*newCapacity); //защита канарейкой
        stack->cnr1 = mem;
        stack->cnr2 = mem + (newCapacity + 1)*sizeof(element_t);

        *(stack->cnr1) = CANARRAY;
        *(stack->cnr2) = CANARRAY;
        stack->array = (element_t*) (void*) (mem+1);
        for (size_t i = stack->elementCount; i < newCapacity; i++) {
            stack->array[i] = POISON;
        }
    }

    stack->array[stack->elementCount] = element;
    stack->elementCount++;

    STACK_VALID(stack);

    DPRINTF("after push: elcount: %llu; capacity: %llu\n", stack->elementCount, stack->capacity);

    printf("after push: \n");
    printStack(stack);

    return {SUCCESS};
}

error_info_t printStack(stack_t* stack) {
    assert(stack);

    STACK_VALID(stack);

    printf("---\n" "stack {\n");
    for (size_t i = 0; i < stack->elementCount; i++) {
        printf("\tel[%llu] = ", i);
        printf(REG, stack->array[i]);
        printf("\n");
        fflush(stdout);
    }
    printf("}\n");

    return {SUCCESS};
}

void stackDestroy(stack_t* stack) {
    assert(stack);
    assert(stack->cnr1);
    free(stack->cnr1);
}



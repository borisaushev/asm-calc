#include "processorСommands.h"


static error_t getJmpIndex(processor_t* processor, int *index) {
    SAFE_CALL(verifyProcessor(processor));
    assert(index);

    *index = processor->commands[++processor->CP] - 1;
    if (*index < 0 || (size_t) *index >= processor->commandsCount) {
        RETURN_ERR(INVALID_INPUT, "index out of range");
    }

    return SUCCESS;
}

static error_t getJmpIndexAndVals(processor_t* processor, int* index, int* value1, int* value2) {
    SAFE_CALL(verifyProcessor(processor));
    assert(index);
    assert(value1);
    assert(value2);

    SAFE_CALL(getJmpIndex(processor, index));

    *value1 = -1;
    *value2 = -1;
    SAFE_CALL(stackPop(&processor->valuesStack, value1));
    SAFE_CALL(stackPop(&processor->valuesStack, value2));

    if (*value1 == POISON || *value2 == POISON) {
        RETURN_ERR(INVALID_INPUT, "stack is empty");
    }

    DPRINTF("parsed jmp index: %d, v1: %d, v2: %d\n", *index, *value1, *value2)

    return SUCCESS;
}

error_t spuDiv(processor_t* processor) {
    SAFE_CALL(verifyProcessor(processor));

    int value1 = POISON, value2 = POISON;
    SAFE_CALL(stackPop(&processor->valuesStack, &value1));
    SAFE_CALL(stackPop(&processor->valuesStack, &value2));

    if (value1 == POISON || value2 == POISON) {
        RETURN_ERR(INVALID_INPUT, "stack is empty");
    }

    if (value1 == 0) {
        RETURN_ERR(INVALID_INPUT, "division by zero");
    }
    int result = value2 / value1;
    DPRINTF("DIV: %d\n", result);
    SAFE_CALL(stackPush(&processor->valuesStack, result));

    return SUCCESS;
}

error_t spuAdd(processor_t* processor) {
    SAFE_CALL(verifyProcessor(processor));

    int value1 = POISON, value2 = POISON;
    SAFE_CALL(stackPop(&processor->valuesStack, &value1));
    SAFE_CALL(stackPop(&processor->valuesStack, &value2));
    if (value1 == POISON || value2 == POISON) {
        RETURN_ERR(INVALID_INPUT, "stack is empty");
    }

    int result = value1 + value2;
    DPRINTF("ADD");
    DPRINTF(": %d\n", result);
    SAFE_CALL(stackPush(&processor->valuesStack, result));

    return SUCCESS;
}

error_t spuMul(processor_t* processor) {
    SAFE_CALL(verifyProcessor(processor));

    int value1 = POISON, value2 = POISON;
    SAFE_CALL(stackPop(&processor->valuesStack, &value1));
    SAFE_CALL(stackPop(&processor->valuesStack, &value2));
    if (value1 == POISON || value2 == POISON) {
        RETURN_ERR(INVALID_INPUT, "stack is empty");
    }

    int result = value1 * value2;
    DPRINTF("MUL");
    DPRINTF(": %d\n", result);
    SAFE_CALL(stackPush(&processor->valuesStack, result));

    return SUCCESS;
}

error_t spuSub(processor_t* processor) {
    SAFE_CALL(verifyProcessor(processor));

    int value1 = POISON, value2 = POISON;
    SAFE_CALL(stackPop(&processor->valuesStack, &value1));
    SAFE_CALL(stackPop(&processor->valuesStack, &value2));
    if (value1 == POISON || value2 == POISON) {
        RETURN_ERR(INVALID_INPUT, "stack is empty");
    }

    int result = value2 - value1;
    DPRINTF("ADD");
    DPRINTF(": %d\n", result);
    SAFE_CALL(stackPush(&processor->valuesStack, result));

    return SUCCESS;
}

error_t spuSqrt(processor_t* processor) {
    SAFE_CALL(verifyProcessor(processor));

    int value = POISON;
    SAFE_CALL(stackPop(&processor->valuesStack, &value));

    if (value == POISON) {
        RETURN_ERR(INVALID_INPUT, "stack is empty");
    }
    if (value < 0) {
        RETURN_ERR(INVALID_INPUT, "negative number passed to sqrt");
    }
    int result = (int) sqrt(value);

    DPRINTF("SQRT: %d\n", result);
    SAFE_CALL(stackPush(&processor->valuesStack, result));

    return SUCCESS;
}

error_t spuOut(processor_t* processor) {
    SAFE_CALL(verifyProcessor(processor));

    int outValue = POISON;
    SAFE_CALL(stackPop(&processor->valuesStack, &outValue));
    if (outValue == POISON) {
        RETURN_ERR(INVALID_INPUT, "stack is empty");
    }

    printf("OUT: %d\n", outValue);

    return SUCCESS;
}

error_t spuPush(processor_t* processor) {
    SAFE_CALL(verifyProcessor(processor));

    int pushValue = processor->commands[++processor->CP];
    SAFE_CALL(stackPush(&processor->valuesStack, pushValue));

    DPRINTF("PUSH: %d\n", pushValue);

    return SUCCESS;
}

error_t spuIn(processor_t* processor) {
    SAFE_CALL(verifyProcessor(processor));

    printf("input val: ");
    int input = POISON;
    if (scanf("%d", &input) != 1) {
        RETURN_ERR(INVALID_INPUT, "invalid input");
    }
    printf("\n");

    DPRINTF("INPUT: %d\n", input);
    SAFE_CALL(stackPush(&processor->valuesStack, input));

    return SUCCESS;
}

error_t spuPushReg(processor_t* processor) {
    SAFE_CALL(verifyProcessor(processor));

    int reg = processor->commands[++processor->CP];
    if (reg >= REGISTER_SIZE || reg < 0) {
        RETURN_ERR(INVALID_INPUT, "reg value out of range");
    }
    if (processor->registerArr[reg] == POISON) {
        RETURN_ERR(INVALID_INPUT, "register is not initialized");
    }
    SAFE_CALL(stackPush(&processor->valuesStack, processor->registerArr[reg]));
    DPRINTF("PUSHREG: reg: %d, val: %d\n", reg, processor->registerArr[reg]);

    return SUCCESS;
}

error_t spuPopReg(processor_t* processor) {
    SAFE_CALL(verifyProcessor(processor));

    int reg = processor->commands[++processor->CP];
    if (reg >= REGISTER_SIZE || reg < 0) {
        RETURN_ERR(INVALID_INPUT, "reg value out of range");
    }

    SAFE_CALL(stackPop(&processor->valuesStack, &(processor->registerArr[reg])));
    if (processor->registerArr[reg] == POISON) {
        RETURN_ERR(INVALID_INPUT, "stack is empty");
    }
    DPRINTF("POPREG: reg: %d, val: %d\n", reg, processor->registerArr[reg]);

    return SUCCESS;
}

error_t spuJmp(processor_t* processor) {
    SAFE_CALL(verifyProcessor(processor));

    int index = -1;
    SAFE_CALL(getJmpIndex(processor, &index));

    DPRINTF("JMP form: %llu, to %d\n", processor->CP, index);
    processor->CP = (size_t) index;

    return SUCCESS;
}

error_t spuJb(processor_t* processor) {
    SAFE_CALL(verifyProcessor(processor));

    int index = -1, value1 = -1, value2 = -1;
    SAFE_CALL(getJmpIndexAndVals(processor, &index, &value1, &value2));

    if (value2 < value1) {
        DPRINTF("Jump form: %llu, to %d\n", processor->CP, index);
        processor->CP = (size_t) index;
    }
    DPRINTF("JB: index: %d, v1: %d, v2: %d\n", index, value1, value2);

    return SUCCESS;
}

error_t spuJbe(processor_t* processor) {
    SAFE_CALL(verifyProcessor(processor));

    int index = -1, value1 = -1, value2 = -1;
    SAFE_CALL(getJmpIndexAndVals(processor, &index, &value1, &value2));

    if (value2 <= value1) {
        DPRINTF("Jump form: %llu, to %d\n", processor->CP, index);
        processor->CP = (size_t) index;
    }
    DPRINTF("JBE: index: %d, v1: %d, v2: %d\n", index, value1, value2);

    return SUCCESS;
}

error_t spuJa(processor_t* processor) {
    SAFE_CALL(verifyProcessor(processor));

    int index = -1, value1 = -1, value2 = -1;
    SAFE_CALL(getJmpIndexAndVals(processor, &index, &value1, &value2));

    if (value2 > value1) {
        DPRINTF("Jump form: %llu, to %d\n", processor->CP, index);
        processor->CP = (size_t) index;
    }
    DPRINTF("JA: index: %d, v1: %d, v2: %d\n", index, value1, value2);

    return SUCCESS;
}

error_t spuJae(processor_t* processor) {
    SAFE_CALL(verifyProcessor(processor));

    int index = -1, value1 = -1, value2 = -1;
    SAFE_CALL(getJmpIndexAndVals(processor, &index, &value1, &value2));

    if (value2 >= value1) {
        DPRINTF("Jump form: %llu, to %d\n", processor->CP, index);
        processor->CP = (size_t) index;
    }
    DPRINTF("JAE: index: %d, v1: %d, v2: %d\n", index, value1, value2);

    return SUCCESS;
}

error_t spuJe(processor_t* processor) {
    SAFE_CALL(verifyProcessor(processor));

    int index = -1, value1 = -1, value2 = -1;
    SAFE_CALL(getJmpIndexAndVals(processor, &index, &value1, &value2));

    if (value2 == value1) {
        DPRINTF("Jump form: %llu, to %d\n", processor->CP, index);
        processor->CP = (size_t) index;
    }
    DPRINTF("JE: index: %d, v1: %d, v2: %d\n", index, value1, value2);

    return SUCCESS;
}

error_t spuJne(processor_t* processor) {
    SAFE_CALL(verifyProcessor(processor));

    int index = -1, value1 = -1, value2 = -1;
    SAFE_CALL(getJmpIndexAndVals(processor, &index, &value1, &value2));

    if (value2 != value1) {
        DPRINTF("Jump form: %llu, to %d\n", processor->CP, index);
        processor->CP = (size_t) index;
    }
    DPRINTF("JNE: index: %d, v1: %d, v2: %d\n", index, value1, value2);
    
    return SUCCESS;
}

error_t spuCall(processor_t* processor) {
    SAFE_CALL(verifyProcessor(processor));

    int index = -1;
    SAFE_CALL(getJmpIndex(processor, &index));

    DPRINTF("CALL: from: [%llu], to index: [%d]\n", processor->CP, index);
    SAFE_CALL(stackPush(&processor->callStack, (int) processor->CP));
    processor->CP = (size_t) index;

    return SUCCESS;
}

error_t spuRet(processor_t* processor) {
    SAFE_CALL(verifyProcessor(processor));

    int retIndex = -1;
    stackPop(&processor->callStack, &retIndex);
    if (retIndex < 0 || retIndex >= (int) processor->commandsCount || retIndex == POISON) {
        RETURN_ERR(INVALID_INPUT, "invalid return index");
    }
    DPRINTF("RET: from: [%llu] to index: [%d]\n", processor->CP, retIndex);
    processor->CP = (size_t) retIndex;

    return SUCCESS;
}

error_t spuPushMem(processor_t* processor) {
    SAFE_CALL(verifyProcessor(processor));

    int reg = processor->commands[++processor->CP];
    if (reg >= REGISTER_SIZE || reg < 0) {
        RETURN_ERR(INVALID_INPUT, "reg value out of range");
    }
    if (processor->registerArr[reg] == POISON) {
        RETURN_ERR(INVALID_INPUT, "register is not initialized");
    }

    SAFE_CALL(stackPush(&processor->valuesStack, processor->RAM[processor->registerArr[reg]]));
    DPRINTF("PUSHMEM: reg: %d, regval: %d, val: %d\n", reg, processor->registerArr[reg], processor->RAM[processor->registerArr[reg]]);

    return SUCCESS;
}

error_t spuPopMem(processor_t* processor) {
    SAFE_CALL(verifyProcessor(processor));

    int reg = processor->commands[++processor->CP];
    if (reg >= REGISTER_SIZE || reg < 0) {
        RETURN_ERR(INVALID_INPUT, "reg value out of range");
    }
    if (processor->registerArr[reg] == POISON) {
        RETURN_ERR(INVALID_INPUT, "register is not initialized");
    }

    int value = POISON;
    SAFE_CALL(stackPop(&processor->valuesStack, &value));
    if (value == POISON) {
        RETURN_ERR(INVALID_INPUT, "stack is empty");
    }

    processor->RAM[processor->registerArr[reg]] = value;
    DPRINTF("POPMEM: reg: %d, regval: %d, val: %d\n", reg, processor->registerArr[reg], value);

    return SUCCESS;
}

error_t spuDraw(processor_t* processor) {
    SAFE_CALL(verifyProcessor(processor));

    for (int i = 1; i <= RAM_SIZE; i++) {
        char ch = (char) processor->RAM[i - 1];
        printf("|%c| ", ch == 0 ? ' ' : ch);
        if (i % RAM_SQUARE_LENGTH == 0) {
            printf("\n");
        }
    }
    printf("\n");

    return SUCCESS;
}
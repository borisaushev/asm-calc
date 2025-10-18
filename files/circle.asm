; решетка = 35
PUSH 25 ; длинна стороны
POPREG HX

PUSHREG HX
PUSH 2
DIV
POPREG FX ; половина длинны

IN
POPREG CX ; c - радиус, центр - (25, 25)

PUSHREG CX
PUSH 0
JB :5 ; C < 0 - hlt

PUSH 0
POPREG AX


:1
PUSH 0
POPREG BX
    :2
    ;ищем x - x0
    PUSHREG BX
    PUSHREG FX
    SUB ; x - x0

    POPREG IX
    CALL :3
    PUSHREG IX ; (x - x0)^2

    ;ищем y - y0
    PUSHREG AX
    PUSHREG FX
    SUB ; y - y0

    POPREG IX
    CALL :3
    PUSHREG IX ; (y - y0)^2

    ADD ; (x - y0)^2 + (y - y0)^2

    PUSHREG CX
    PUSHREG CX
    MUL ; R^2

    JA :4
    PUSH 35 ; пушим #

    PUSHREG AX
    PUSHREG HX
    MUL
    PUSHREG BX
    ADD ; индекс в массиве RAM
    POPREG DX

    POPMEM DX ; закрашиваем

    :4
    PUSHREG BX
    PUSH 1
    ADD
    POPREG BX

    PUSHREG BX
    PUSHREG HX
    JB :2

PUSHREG AX
PUSH 1
ADD

POPREG AX

PUSHREG AX
PUSHREG HX
JB :1 ; повтор если a < 10

DRAW
HLT

:3
PUSHREG IX
PUSHREG IX
MUL
POPREG IX
RET

:5
HLT
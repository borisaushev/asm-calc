;если a == 0
;    out -1
;    перейти в hlt

;считаем b*b - 4ac
;если < 0 перейти в hlt
;если == 0 перейти к секции 1
;    принтим 1
;    x = -b / 2*a

;если > 0 перейти к секции 2
;    принтим 2
;    x1 = (-b + d) / 2*a
;    x2 = (-b - d) / 2*a
;    принтим x1, x2

; было 10
    ;
IN ; a
POPREG AX
IN ; b
POPREG BX
IN ; c
POPREG CX

; a == 0 - hlt
PUSHREG AX ; если a == 0 конец
PUSH 0
JE :2;TODO

PUSHREG BX ; b^2
PUSHREG BX
MUL

PUSH 4 ; 4*a*c
PUSHREG AX
PUSHREG CX
MUL
MUL

SUB ; b^2 - 4*a*c

POPREG DX ; записали квадрат D

PUSHREG DX ; если D^2 < 0 конец
PUSH 0
JB :1 ; TODO

PUSHREG DX
SQRT
POPREG DX ; записали sqrt(D)

; переход в зависимости от D
PUSHREG DX
PUSH 0
JNE :3; TODO


; если DX == 0
PUSH 1 ; вывод 1
OUT

PUSHREG BX ; -b
PUSH -1
MUL

PUSHREG AX ; 2*a
PUSH 2
MUL

DIV ; -b/2*a
OUT
HLT


:3
; если DX > 0
; считаем x1
PUSH 2 ; вывод 2
OUT

PUSHREG BX ; -b
PUSH -1
MUL

PUSHREG DX ; D

ADD ; -b + D

PUSH 2 ; 2*a
PUSHREG AX
MUL

DIV ; (-b+D)/2*a
OUT ; выводим x1

; считаем x2
PUSHREG BX ; -b
PUSH -1
MUL

PUSHREG DX ; D

SUB; -b - D

PUSH 2 ; 2*a
PUSHREG AX
MUL

DIV ; (-b-D)/2*a
OUT ; выводим x2
HLT

PUSH -1
OUT
HLT

:2
PUSH 0
OUT
:1
HLT
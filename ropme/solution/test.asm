bits 32

inc eax
inc ebx
inc ecx
inc edx
push eax
push ebx
push ecx
push edx
pop eax
pop ebx
pop ecx
pop edx
shl eax, 1
xor eax, eax
mov ebx, esp
ret 32
dec ebx

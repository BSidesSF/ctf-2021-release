__attribute__((naked)) int read(int fd, char *buf, int count) {
  __asm__(
      "push ebp;"
      "mov ebp, esp;"
      "push ebx;"

      "mov eax, 3;" // sys_read
      "mov ebx, [ebp+8];" // f = arg1
      "mov ecx, [ebp+12];" // buf = arg2
      "mov edx, [ebp+16];" // count = arg3
      "int 0x80;"

      "pop ebx;"
      "pop ebp;"
      "ret;"
  );
}

__attribute__((naked)) void write(int f, char *buf, int count) {
  __asm__(
      "push ebp;"
      "mov ebp, esp;"
      "push ebx;"

      "mov eax, 4;" // sys_write
      "mov ebx, [ebp+8];" // f = arg1
      "mov ecx, [ebp+12];" // buf = arg2
      "mov edx, [ebp+16];" // count = arg3
      "int 0x80;"

      "pop ebx;"
      "pop ebp;"
      "ret;"
  );
}

__attribute__((naked)) int open(char *filename, int flags, int mode) {
  __asm__(
      "push ebp;"
      "mov ebp, esp;"
      "push ebx;"

      "mov eax, 5;" // sys_open
      "mov ebx, [ebp+8];" // filename = arg1
      "mov ecx, [ebp+12];" // flags = arg2
      "mov edx, [ebp+16];" // mode = arg3
      "int 0x80;"

      "pop ebx;"
      "pop ebp;"
      "ret;"
  );
}

__attribute__((naked)) void close(int fd) {
  __asm__(
      "push ebp;"
      "mov ebp, esp;"
      "push ebx;"

      "mov eax, 6;" // sys_close
      "mov ebx, [ebp+8];" // arg1
      "int 0x80;"

      "pop ebx;"
      "pop ebp;"
      "ret;"
  );
}

__attribute__((naked)) int creat(char *filename, int mode) {
  __asm__(
      "push ebp;"
      "mov ebp, esp;"
      "push ebx;"

      "mov eax, 8;" // sys_creat
      "mov ebx, [ebp+8];" // filename = arg1
      "mov ecx, [ebp+12];" // mode = arg2
      "int 0x80;"

      "pop ebx;"
      "pop ebp;"
      "ret;"
  );
}

__attribute__((naked)) void exit(int code) {
  __asm__(
      "mov eax, 1;" // sys_exit
      "mov ebx, [esp+4];" // arg1
      "int 0x80;"
  );
}

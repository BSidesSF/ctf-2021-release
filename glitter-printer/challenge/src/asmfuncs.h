// Strlen
__attribute__((naked)) int strlen(char *s) {
  __asm__(
      "push ebp;"
      "mov ebp, esp;"

      "push esi;"
      "push edi;"

      "mov edi, [ebp+8];"

      "mov ecx, -1;"
      "xor eax, eax;"
      "repnz scasb;"
      "not ecx;"
      "dec ecx;"

      "mov eax, ecx;"

      "pop edi;"
      "pop esi;"
      "pop ebp;"
      "ret;"
      : /* Out */
      : /* In */
      : /* Reg */
  );
}

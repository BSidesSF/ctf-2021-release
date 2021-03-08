// Start the data at this offset from the binary start
#define DATA_START 0x8000-5
#define DATA_START_S "0x8000-5"

__asm__(
    "call bottom;"
    "bottom:"
    "pop eax;"
    "add eax, "DATA_START_S";"
    "push eax;"
    "call _start;"
);

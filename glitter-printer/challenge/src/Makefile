all: core harness

core: core.c
	gcc -Wl,--section-start=.text=0x00400000,--gc-sections -o core.elf -Wall -O0 core.c -static -no-pie -fno-stack-protector -z execstack -nostdlib -m32 -masm=intel
	strip -s --remove-section=!.text* --remove-section=* --strip-unneeded core.elf
	dd if=core.elf of=core bs=4096 skip=1

harness: harness.c
	gcc -o harness -Wall -O0 harness.c -m32

clean:
	rm -f core.elf *.o

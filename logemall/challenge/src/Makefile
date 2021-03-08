all: logemall

logemall: logemall.c map.o encounters.o asciimon.o character.o object.o world.o helpers.o
	gcc -o logemall -Wall -m64 -no-pie -O0 logemall.c map.o encounters.o asciimon.o character.o object.o world.o helpers.o
	strip logemall

clean:
	rm -f logemall *.o

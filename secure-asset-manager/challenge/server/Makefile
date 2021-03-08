CFLAGS?=-Wall -o0 -m64

all: not-solution

not-solution: not-solution.o ../common/validator.o ../common/proc_maps_parser/pmparser.o
	gcc -o not-solution not-solution.o ../common/validator.o ../common/proc_maps_parser/pmparser.o

clean:
	rm -f secure-asset-manager not-solution *.o

CFLAGS?=-Wall -o0 -m64

all: secure-asset-manager

secure-asset-manager: secure-asset-manager.o ../common/validator.o ../common/proc_maps_parser/pmparser.o
	gcc -o secure-asset-manager secure-asset-manager.o ../common/validator.o ../common/proc_maps_parser/pmparser.o
	strip secure-asset-manager

clean:
	rm -f secure-asset-manager not-solution *.o

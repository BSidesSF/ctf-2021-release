CC?=gcc
OBJS=ropme.o

ropme: ${OBJS}
	${CC} -m32 -Wall ${CFLAGS} -o ropme ropme.c

clean:
	rm -fv *.o core core.* ropme

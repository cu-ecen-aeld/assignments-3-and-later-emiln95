CC=gcc
TARGET=aesdsocket
CFLAGS+=-g -Wall -Werror
INCLUDES+=
OBJ+=
LDGLAFS+=

all: aesdsocket.o

	${CC} ${CFLAGS} ${INCLUDES} ${OBJ} aesdsocket.o -o ${TARGET} ${LDGLAFS}

aesdsocket.o:	aesdsocket.c

	${CC} ${CFLAGS} -c ${INCLUDES} ${OBJ} aesdsocket.c -o aesdsocket.o ${LDGLAFS}

default: all

clean: 
	rm -f *.o
	rm -f *.elf 
	rm -f aesdsocket

CROSS_COMPILE="" 

CC="$(CROSS_COMPILE)gcc"
TARGET="$(CROSS_COMPILE)writer"
CFLAGS=
INCLUDES=
OBJ=writer.o
LDGLAFS=


all: $(CROSS_COMPILE)writer


$(CROSS_COMPILE)writer: writer.o

	eval "$(CC) $(CFLAGS) $(INCLUDES) $(OBJ) -o $(TARGET) $(LDGLAFS)"
#	eval "$(CC) writer.o -o $(output)"

writer.o: writer.c


	eval "$(CC) $(CFLAGS) $(INCLUDES) -c -Wall writer.c -o writer.o $(LDGLAFS)"
#	eval "$(compiler) -c -Wall writer.c -o writer.o"


clean: 
	rm -f *.o
	rm -f *.elf
	rm -f *writer	

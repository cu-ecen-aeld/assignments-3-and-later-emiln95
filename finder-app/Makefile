CROSS_COMPILE="" 

compiler="$(CROSS_COMPILE)gcc"
output="$(CROSS_COMPILE)writer"

all : writer.o

$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(TARGET) $(LDGLAFS)

$(CROSS_COMPILE)writer: writer.o

	eval "$(compiler) writer.o -o $(output)"

writer.o: writer.c

	eval "$(compiler) -c -Wall writer.c -o writer.o"


clean: 
	rm -f *.o
	rm -f *.elf
	rm -f *writer	

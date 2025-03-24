CROSS_COMPILE="" 

compiler="$(CROSS_COMPILE)gcc"
output="$(CROSS_COMPILE)writer"

all : writer.o

	eval "aarch64-none-linux-gnu-gcc writer.o -o aarch64-none-linux-gnu-writer"
	eval "gcc writer.o -o writer"

$(CROSS_COMPILE)writer: writer.o

	eval "$(compiler) writer.o -o $(output)"

writer.o: writer.c

	eval "$(compiler) -c -Wall writer.c -o writer.o"


clean: 
	rm -f *.o
	rm -f *.elf
	rm -f *writer	

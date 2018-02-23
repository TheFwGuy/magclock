CC=msp430-elf-gcc
CFLAGS=-O0 -Wall -g -mmcu=msp430f2013 -I$(MSP430_TI)/include -L$(MSP430_TI)/include

OBJS=main.o


all: $(OBJS)
	$(CC) $(CFLAGS) -o test.elf $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -fr main.elf $(OBJS)

NAME   = LPC2138_flash

CC      = arm-elf-gcc
LD      = arm-elf-ld -v
AR      = arm-elf-ar
AS      = arm-elf-as
CP      = arm-elf-objcopy
OD		= arm-elf-objdump

CFLAGS  = -I./ -c -fno-common -O0 -g -mcpu=arm7tdmi -nostartfiles
AFLAGS  = -ahls -mapcs-32 -o crt.o
LFLAGS  =  -Map main.map -T LPC2138_flash.cmd
CPFLAGS = -O ihex
ODFLAGS	= -x --syms

all: test

clean:
	-rm crt.lst *.o

test: main.out
	@ echo "...copying"
	$(CP) $(CPFLAGS) main.out main.hex
	$(OD) $(ODFLAGS) main.out > main.dmp

main.out: crt.o main.o audio_sinewave.o system.o LPC2138_flash.cmd 
	@ echo "..linking"
	$(LD) $(LFLAGS) -o main.out  crt.o main.o audio_sinewave.o system.o 

crt.o: crt.s
	@ echo ".assembling"
	$(AS) $(AFLAGS) crt.s > crt.lst

main.o: main.c
	@ echo ".compiling"
	$(CC) $(CFLAGS) main.c

audio_sinewave.o: audio_sinewave.c 
	@ echo ".compiling audio_sinewave.c"
	$(CC) $(CFLAGS) audio_sinewave.c 

system.o: system.c
	@ echo ".compiling system.c"
	$(CC) $(CFLAGS) system.c 

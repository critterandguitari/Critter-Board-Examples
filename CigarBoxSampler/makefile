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

main.out: crt.o main.o system.o adc.o m25pxx.o sample_record.o sample_play.o LPC2138_flash.cmd 
	@ echo "..linking"
	$(LD) $(LFLAGS) -o main.out  crt.o main.o adc.o m25pxx.o sample_play.o sample_record.o system.o 

crt.o: crt.s
	@ echo ".assembling"
	$(AS) $(AFLAGS) crt.s > crt.lst

main.o: main.c
	@ echo ".compiling"
	$(CC) $(CFLAGS) main.c

system.o: system.c
	@ echo ".compiling system.c"
	$(CC) $(CFLAGS) system.c 

adc.o: adc.c
	@ echo ".compiling adc.c"
	$(CC) $(CFLAGS) adc.c
	
m25pxx.o: m25pxx.c
	@ echo ".compiling m25pxx.c"
	$(CC) $(CFLAGS) m25pxx.c
	
sample_play.o: sample_play.c
	@ echo ".compiling sample_play.c"
	$(CC) $(CFLAGS) sample_play.c
	
sample_record.o: sample_record.c
	@ echo ".compiling sample_record.c"
	$(CC) $(CFLAGS) sample_record.c


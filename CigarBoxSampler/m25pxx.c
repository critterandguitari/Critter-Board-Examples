/******************************************************************************/
/*  Flash Sampler Tutorial                                                    */
/*  (c)2006 GPL, Owen Osborn, Critter and Guitari, Dearraindrop               */
/******************************************************************************/
/*                                                                            */
/*  :  Sampler for the Critter Board with on-board flash or flash cards       */
/*                                                                            */
/******************************************************************************/

#include "LPC21xx.h"
#include "m25pxx.h"


void spi_1_init(void){
  PINSEL1 |= 0x000000A8;
  IODIR0 |= FLASH_CS;
  IOSET0 |= FLASH_CS;
  SSPCR0 = 0x0007;//0x0807;          // cpol = 1, cpha = 1
  SSPCPSR = 4;            // prescale
  SSPCR1 = 2;           // enable
}

unsigned char spi_1(unsigned char dat){
  SSPDR  = dat;
  while ((SSPSR & 0x10));     // wait for transfer completed
  return SSPDR;
}

void bulk_erase(void){
   IOCLR0 |= FLASH_CS;
  spi_1(0x06); // write enable
  IOSET0 |= FLASH_CS;

    IOCLR0 |= FLASH_CS;
  spi_1(0xc7); // erase
  IOSET0 |= FLASH_CS;
}

unsigned char read_byte(unsigned int loc){
  unsigned char tmp;
  IOCLR0 |= FLASH_CS;
  spi_1(0x03);  // read
  spi_1(loc >> 16);
  spi_1(loc >> 8);
  spi_1(loc);
  tmp = spi_1(0xaa);
  IOSET0 |= FLASH_CS;
  return tmp;
}

void write_byte(unsigned int loc, unsigned char byte){
  IOCLR0 |= FLASH_CS;
  spi_1(0x06); // write enable
  IOSET0 |= FLASH_CS;
  IOCLR0 |= FLASH_CS;
  spi_1(0x02);  // write page
  spi_1(loc >> 16);
  spi_1(loc >> 8);
  spi_1(loc);
  spi_1(byte);
  IOSET0 |= FLASH_CS;
}

void write_sector(unsigned int loc, unsigned char *buf){
  int count;
  IOCLR0 |= FLASH_CS;
  spi_1(0x06); // write enable
  IOSET0 |= FLASH_CS;
  IOCLR0 |= FLASH_CS;
  spi_1(0x02);  // write page
  spi_1(loc >> 16);
  spi_1(loc >> 8);
  spi_1(loc);
  for (count = 0; count < 256; count++){
    spi_1(buf[count]);
  }
  IOSET0 |= FLASH_CS;
}


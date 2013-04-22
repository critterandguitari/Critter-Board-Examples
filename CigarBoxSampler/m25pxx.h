/******************************************************************************/
/*  Flash Sampler Tutorial                                                    */
/*  (c)2006 GPL, Owen Osborn, Critter and Guitari, Dearraindrop               */
/******************************************************************************/
/*                                                                            */
/*  :  Sampler for the Critter Board with on-board flash or flash cards       */
/*                                                                            */
/******************************************************************************/


#define FLASH_CS (1 << 21)


void spi_1_init(void);

unsigned char spi_1(unsigned char dat);

unsigned char read_byte(unsigned int loc);

void write_byte(unsigned int loc, unsigned char byte);

void write_sector(unsigned int loc, unsigned char *buf);

void bulk_erase(void);

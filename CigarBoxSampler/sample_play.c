/******************************************************************************/
/*  Flash Sampler Tutorial                                                    */
/*  (c)2006 GPL, Owen Osborn, Critter and Guitari, Dearraindrop               */
/******************************************************************************/
/*                                                                            */
/*  :  Sampler for the Critter Board with on-board flash or flash cards       */
/*                                                                            */
/******************************************************************************/



#include "m25pxx.h"
#include "sample_play.h"
#include "LPC21xx.h"
#include "system.h"

static unsigned int address = 0;
static unsigned int loopStart = 0;
static unsigned int loopEnd = 0;
static unsigned int  dacVal = 0;
static unsigned int sample = 0;
static unsigned char tmp8 = 0;
static unsigned int rawSpeed = 0;
static unsigned int gainRamp = 0;
static int gainRampReduce = 0;

static unsigned int direction = 1;

// this is the interrupt handler for the sampler.  sampler on timer 0
static void timer0ISR(void) __attribute__ ((interrupt ("IRQ")));


void sample_play_start(void){
  /* Reset timer 0 */
  T0TCR = 0;
  /* Set the timer 0 prescale counter */
  T0PR = 10;
  /* Set timer 0 match register */
  T0MR0 = rawSpeed + 50;
  /* Generate interrupt and reset counter on match */
  T0MCR = 3;
  /* Start timer 0 */
  T0TCR = 1;
}

void sample_play_stop(void){
    /* Reset timer 0 */
    T0TCR = 0;
    IOSET0 |= FLASH_CS;
    disableIRQ();               // stop interrupt
    VICIntSelect |= 0x10;       // no timer 0 interrupt
    VICIntEnable &= ~0x10;      // disable timer 0 interrupt
    VICVectCntl0 &= ~0x24;        // remove timer 0 from slot 0
}

void sampler_speed(unsigned int speed){
    T0TCR = 3;  // stop and reset
    T0MR0 = speed + 50;
    T0TCR = 1;    // restart
}

void sampler_change(unsigned int beg, unsigned int end, unsigned int speed, unsigned int dir){
    direction = dir;
    T0TCR = 3;  // stop and reset
    T0MR0 = speed + 50;
    loopStart = beg;
    loopEnd =  beg + end;
    T0TCR = 1;    // restart
}

void sample_play_init(void){
  PINSEL1 |= 0x00080000;  // enable dac
  address = 6;               // remember not to f with any of these
  loopStart = 0;              // once interrupts are enabled, use temps
  loopEnd =   30000;

  /* Timer 0 interrupt is an IRQ interrupt */
  VICIntSelect &= ~0x10;
  /* Enable timer 0 interrupt */
  VICIntEnable = 0x10;
  /* Use slot 0 for timer 0 interrupt */
  VICVectCntl0 = 0x24;
  /* Set the address of ISR for slot 0 */
  VICVectAddr0 = (unsigned int)timer0ISR;

  enableIRQ();   // enable interrupt
}

static void timer0ISR(void)
{ 
  //IOCLR0 |= 0x80000000;
	if (direction){
		address += 1;
	}
	else{
		address -= 1;
	}

    if ((address < loopEnd) && (address > loopStart)){
        // speed version
        // fisrt read in the bytes (from last sample isr)
        sample = (unsigned char)SSPDR;
        sample = (unsigned char)SSPDR;
        sample = (unsigned char)SSPDR;  // read in fifo
        sample = (unsigned char)SSPDR;
        sample = (unsigned char)SSPDR;

        IOSET0 |= FLASH_CS;        // end command

        IOCLR0 |= FLASH_CS;           // new read command

        SSPDR = 0x03;
        SSPDR = ((unsigned char)(address >> 16));
        SSPDR = ((unsigned char)(address >> 8));
        SSPDR = ((unsigned char)(address));
        SSPDR = 0xaa;
          
        // sample out to DAC
        DACR = sample << 8;

    }
  
    else if (direction) {
	    address = loopStart;
    }
  
    else{
        address = loopEnd;
    }
  
  /* Clear the timer 0 interrupt */
  T0IR = 0xFF;
  /* Update VIC priorities */
  VICVectAddr = 0;
}

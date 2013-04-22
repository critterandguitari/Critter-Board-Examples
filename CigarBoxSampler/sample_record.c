/******************************************************************************/
/*  Flash Sampler Tutorial                                                    */
/*  (c)2006 GPL, Owen Osborn, Critter and Guitari, Dearraindrop               */
/******************************************************************************/
/*                                                                            */
/*  :  Sampler for the Critter Board with on-board flash or flash cards       */
/*                                                                            */
/******************************************************************************/


#include "sample_record.h"
#include "m25pxx.h"
#include "LPC21xx.h"
#include "system.h"

static unsigned char buffer1[256];
static unsigned char buffer0[256];
static int bufferFull = 0;
static int bufferNumber = 0; 
static int index = 0;
static int address = 0;
static unsigned char tmp8 = 0;
static unsigned int  dacVal = 0;
static unsigned int sample = 0;

// ISR for recieving samples from uart, and ISR for timer (if recording from line)
static void uart0ISR(void) __attribute__ ((interrupt ("IRQ")));
static void timer0ISR_record(void) __attribute__ ((interrupt ("IRQ")));



int sample_record_uart_init(void){
	PINSEL1 |= 0x00080000;  // enable dac (to hear samples from UART)
	
	// disable interupts
	disableIRQ();

	/* UART 0 interrupt is an IRQ interrupt */
	VICIntSelect &= ~0x40;
	/* Enable UART 0 interrupt */
	VICIntEnable = 0x40;
	/* Use slot 0 for UART 0 interrupt */
	VICVectCntl0 = 0x26;
	/* Set the address of ISR for slot 0 */
	VICVectAddr0 = (unsigned int)uart0ISR;
	  
	//enable interrupts to catch sound bytes
	U0IER         = 0x01;             // rx interrupt
	enableIRQ();
}

int sample_record_uart(void){


  // wait for rec status to be on
 /* while (!get_rec_status()){
    if (!get_rec_enable()){  // check to see if recording is disabled, if so, retrun
      return 0;             //  no bytes recorded
    }
  }*/
  
  // once record is on, and record is still enabled
  while(get_rec_enable()){
  // wait for recieve buffer to fill
    if (bufferFull){
          // read bytes from buffer and write to flash
     // ledOn();
      if (bufferNumber)
        write_sector(address, buffer1);
      else
        write_sector(address, buffer0);
  
                  // increment address by 256
      address += 256;
      bufferFull = 0;
      //ledOff();
    }
  }// while recording is on

  // disable interrupts when done
  disableIRQ();
  U0IER         = 0x00;             // rx interrupt disable
  /// remove uart0 interrupt from vic
  VICIntSelect |= 0x40;
  /* disable UART 0 interrupt */
  VICIntEnable = 0x00;
  /* nullify slot */
  VICVectCntl0 = 0x00;
}


int sample_record_line_init(void){
    PINSEL1 |= 0x00080000;  // enable dac

    address = 0;

  /* Timer 0 interrupt is an IRQ interrupt */
  VICIntSelect &= ~0x10;
  /* Enable timer 0 interrupt */
  VICIntEnable = 0x10;
  /* Use slot 0 for timer 0 interrupt */
  VICVectCntl0 = 0x24;
  /* Set the address of ISR for slot 0 */
  VICVectAddr0 = (unsigned int)timer0ISR_record;
  
    /* Reset timer 0 */
  T0TCR = 0;
  /* Set the timer 0 prescale counter */
  T0PR = 10;
  /* Set timer 0 match register */
  T0MR0 = 300;
  /* Generate interrupt and reset counter on match */
  T0MCR = 3;
  /* Start timer 0 */
  T0TCR = 1;
}

int sample_record_line(void){
	
	enableIRQ();            // enable interrupts to get sound
	
	while(get_rec_enable()){  // while record is enabled
	
	// if record button down
		if (get_rec_status()){
			if (bufferFull){
				if (bufferNumber)
					write_sector(address, buffer1);
				else
					write_sector(address, buffer0);
				address += 256;
				bufferFull = 0;
			}
		}
	}
    disableIRQ();

    VICIntSelect |= 0x10;      // no timer 0 interrupt in vic
    VICIntEnable &= ~0x10;       // disable interrupt
    VICVectCntl0 &= ~0x24;      // remove timer 0 from slot 0
    T0TCR = 0;          // stop timer
}



// check to stop recording
inline int get_rec_status(){
	if (RED_BUTTON)
		return 1;
	else
		return 0;
}


// check rec enable
inline int get_rec_enable(){
	if (SWITCH_UP && (address < 500000))
		return 1;
	else
		return 0;
}

static void uart0ISR(void) { 

  tmp8 = U0RBR;


   // output sample to hear UART recording
      sample = tmp8;
    dacVal = sample & 0x00000ff;
    DACR = dacVal << 8;

  if (bufferNumber)
    buffer0[index] = tmp8;
  else 
    buffer1[index] = tmp8;

  index++;
  index %= 256;
  if (!index){
    bufferFull = 1;
    bufferNumber++;
    bufferNumber %= 2;
  }
  tmp8 =  U0IIR;  // clear this interrupt
  /* Update VIC priorities */
  VICVectAddr = 0;
}

static void timer0ISR_record(void)
{ 
   // output sample to hear UART recording
      sample = get_adc1(5) >> 2;
    DACR = dacVal << 8;

    if (sample > 242)       // about -1 db
        led_card(1);
    else
        led_card(0);
        
    if (sample > 170)       // about -10 db
        led_board(1);
    else
        led_board(0);
        

    if (get_rec_status()){
        tmp8 = sample;
        if (bufferNumber)
            buffer0[index] = tmp8;
        else 
            buffer1[index] = tmp8;

        index++;
        index %= 256;
        if (!index){
            bufferFull = 1;
            bufferNumber++;
            bufferNumber &= 1;
        }
    }

    /* Clear the timer 0 interrupt */
    T0IR = 0xFF;
    /* Update VIC priorities */
    VICVectAddr = 0;
}


/*
 ****************************************************************************
 *
 * Hello Sine Wave
 *  
 * This program provides the basic structure for interrupt driven audio
 * synthesis with the LPC2138.
 * 
 * Copyright (C) 2006 Owen Osborn
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ****************************************************************************
 */

#include "audio_sinewave.h"            
#include "LPC21xx.h"                // register definitions for processor
#include "system.h"                 // system level stuff, interrupt stuff
#include "waves.h"                  // contains the lookup table


/*
    This value gets put into the LPC2138's timer in order
    to cause the audio interrupt at about 16 kHz.  The timer counts up at the
    CPU clock frequency: 58982400 Hz.  When it reaches 3685 it triggers an 
    interrupt and starts back at 0.  So the sampling rate (sr) is 
    58982400 / 3686 = 16001.74 Hz.
*/

#define SAMPLE_RATE 3685   

/*
    This is a scaling value used to convert phase increment to Hertz.  
    The phase accumulator is 24 bits, so for a desired frequency f, 
    the phase increment = f(2 ^ 24) / sr, where sr = 16001.74 from above.  
    So this scalar is (2 ^ 24) / 16001.74 = 1048.46. we round down.
*/
             
#define FREQSCALE   1048               

        
static int sample;                  // value that eventually gets output to DAC
static int phaseAccum = 0;          // phase accumulator
static int phaseInc = 0;            // phase increment
static int amplitude = 0;           // oscillator amplitude


/* 
    This is the interrupt handler for the synth.   on timer 0 
*/
static void timer0ISR(void) __attribute__ ((interrupt ("IRQ")));

/* 
    This function enables analog out, initializes the timer 0, and sets up the interrupt 
*/ 
void synth_init(void){
    PINSEL1 |= 0x00080000;              // enable dac
    VICIntSelect &= ~0x10;              // timer 0 is IRQ
    VICIntEnable = 0x10;                // enable timer 0 interrupt
    VICVectCntl0 = 0x24;                //  use slot 0 for timer 0 interrupt 
    VICVectAddr0 = (unsigned int)timer0ISR;     // address of ISR for slot 0
    T0TCR = 0;                          // reset timer
    T0PR = 0;                           // timer prescale
    T0MR0 = SAMPLE_RATE;                // match at sampling rate
    T0MCR = 3;                          // interrupt and reset on match
    T0TCR = 1;                          // start timer
    enableIRQ();                        // enable interrupt
}

/* 
    This function stops timer, disables interrupts     
*/
void synth_stop(void){
    T0TCR = 0;                          // stop timer
    disableIRQ();                       // interrupts off
}


/* 
    call this function from the main program to adjust the oscillator frequency 
    Note that the frequency is specified as an integer, giving a resolution of 1 Hz.
    The oscillator is capable of much greater resolution (1048 points per Hz to be exact),
    so this could be easily improved, either by using a floating point frequency, or some
    fixed point form.  
*/ 
void synth_freq(int freq){
    phaseInc = freq * FREQSCALE; 
}

/* 
    call this function from the main program to adjust the oscillator amplitude  (0 - 255)
*/ 
void synth_amp(int amp){
    amplitude = amp; 
}

/* 
    This is the audio interrupt routine.  This function gets called at the sampling rate
    after synth_init() is called.
 */ 
static void timer0ISR(void)
{ 
    //IOCLR0 |= (1 << 23);    // start speed test
    
    /* add phase increment value to accumulator */
    phaseAccum += phaseInc; 
    
    /* use bits 16 - 23 of the phase accumulator to lookup sample value from wave table */
    sample = sineTable[(phaseAccum >> 16) & 0x000000ff];   
    
    /* adjust amplitude */
    sample *= amplitude;
    sample >>= 8; 
    
    /* 
        output sample to digital to analog converter register.
        This is a 16 bit register, although the DAC is only 10 bits.
        The value must be left justified, so we shift the sample left before writing 
        it to the register.
    */
    DACR = sample << 8;
         
    //IOSET0 |= (1 << 23);    // end speed test
    
    /* maintain the interrupt */
    T0IR = 0xFF;                        // clear interrupts
    VICVectAddr = 0;                    // update vic priorities
}

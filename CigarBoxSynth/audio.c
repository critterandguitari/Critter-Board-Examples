/******************************************************************************/
/*  audio.c                                                        */
/*  (c)2006 GPL, Owen Osborn, Critter and Guitari                             */
/******************************************************************************/
/*                                                                            */
/*  :  audio.c  8 bit FM synth voices for Critter Board                        */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
           
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

        
int sample;                  // value that eventually gets output to DAC

static int phaseAccumCarrier[] = {0, 0, 0, 0};
static int phaseDeltaCarrier[] = {0, 0, 0, 0};

static int phaseAccumModulator[] = {0, 0, 0, 0};
static int phaseDeltaModulator[] = {0, 0, 0, 0};

static int deviation[] = {0, 0, 0, 0};

static int modulator[] = {0, 0, 0, 0};
static int carrier[] = {0, 0, 0, 0};

static int level[] = {255, 255, 255, 255};
static int mainVolume = 255;

/* 
    This is the interrupt handler for the synth.   on timer 0 
*/
static void timer0ISR(void) __attribute__ ((interrupt ("IRQ")));

/* 
    This function enables analog out, initializes the timer 0, and sets up the interrupt 
*/ 
void synth_init(void){
    IODIR0 |= (1 << 26);
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
    This is the function we call to adjust the synthesizer
       the parameters are: which voice to adjust, carrier frequency,
        index of modulation, and harmonicity.  Harmonicity is represented
        as a fixed point fraction where the 8 least significant bits
        are fractional. individual voice amplitude and overall amplitude
        are not implemented, but could be added easily.
*/
void fm_voice(int voice, int freq, int index, int harmonicity){
    int fmod;
    int amod;
    
    voice &= 0x3;                               // make sure we don't try to access out of bounds voice
    
    fmod = (harmonicity * freq) >> 8;          // convert harmonicity and index
    amod = (fmod * index) >> 8;
    phaseDeltaModulator[voice] = fmod * FREQSCALE; // update params, convert to hz
    phaseDeltaCarrier[voice] = freq * FREQSCALE;
    deviation[voice] = amod * FREQSCALE; 
}


/* 
    This is the audio interrupt routine.  This function gets called at the sampling rate
    after synth_init() is called.
 */ 
static void timer0ISR(void)
{ 
   // IOCLR0 |= (1 << 26);    // start speed test
    
     /* 
          here we calculate the 4 FM voices.  First the modulator oscillator,
                and add the result to the carrier frequency.  Each voice is added into 
                'sample' which holds the actual sample.
     */ 
    
    phaseAccumModulator[0] +=  phaseDeltaModulator[0];   
    modulator[0]  = ((sineTable[(phaseAccumModulator[0] >> 16) & 0x000000ff] - 128) * deviation[0]); 
    modulator[0] /= 256;
    phaseAccumCarrier[0] += phaseDeltaCarrier[0] + modulator[0];
    carrier[0] = ((sineTable[(phaseAccumCarrier[0] >> 16) & 0x000000ff]) * level[0]);
    carrier[0] /= 256;
    
    sample = carrier[0];
    
    phaseAccumModulator[1] +=  phaseDeltaModulator[1];   
    modulator[1]  = ((sineTable[(phaseAccumModulator[1] >> 16) & 0x000000ff] - 128) * deviation[1]); 
    modulator[1] /= 256;
    phaseAccumCarrier[1] += phaseDeltaCarrier[1] + modulator[1];
    carrier[1] = ((sineTable[(phaseAccumCarrier[1] >> 16) & 0x000000ff]) * level[1]);
    carrier[1] /= 256;
        
    sample += carrier[1];
    
    phaseAccumModulator[2] +=  phaseDeltaModulator[2];   
    modulator[2]  = ((sineTable[(phaseAccumModulator[2] >> 16) & 0x000000ff] - 128) * deviation[2]); 
    modulator[2] /= 256;
    phaseAccumCarrier[2] += phaseDeltaCarrier[2] + modulator[2];
    carrier[2] = ((sineTable[(phaseAccumCarrier[2] >> 16) & 0x000000ff]) * level[2]);
    carrier[2] /= 256;
    
    sample += carrier[2];
    
    phaseAccumModulator[3] +=  phaseDeltaModulator[3];   
    modulator[3]  = ((sineTable[(phaseAccumModulator[3] >> 16) & 0x000000ff] - 128) * deviation[3]); 
    modulator[3] /= 256;
    phaseAccumCarrier[3] += phaseDeltaCarrier[3] + modulator[3];
    carrier[3] = ((sineTable[(phaseAccumCarrier[3] >> 16) & 0x000000ff]) * level[3]);
    carrier[3] /= 256;
    
    sample += carrier[3];
    
                
    /* adjust amplitude */
    sample *= mainVolume;
    sample /= 256;
    
    /* 
        left adjust sample.  we added 4 8bit waves together yielding a 10 bit wave
        DACR is 16 bits, so shift right 6
    */
    DACR = sample << 6;
         
   // IOSET0 |= (1 << 26);    // end speed test
    
    /* maintain the interrupt */
    T0IR = 0xFF;                        // clear interrupts
    VICVectAddr = 0;                    // update vic priorities
}

/******************************************************************************/
/*  main.c                                                        */
/*  (c)2006 GPL, Owen Osborn, Critter and Guitari                             */
/******************************************************************************/
/*                                                                            */
/*  :  main.c  fm midi synth.  midi comes in on RXD1 (P0.9)                        */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
#include "LPC21xx.h"
#include "system.h"
#include "audio.h"              
#include "adc.h"
#include "miditof.h"

// defines midi note offset from keypad
#define OFFSET      60

#define NOTE_OFF 1
#define NOTE_ON 2


int main (void) {

    int i = 0;
    int j = 0;
    
    // holds values of 3 knobs
    int knob0 = 0;
    int knob1 = 0;
    int knob2 = 0;
    
    
    // keeps track of which midi byte we are on
    int midiState = 0;
    
    // holds the midi byte
    char midiByte;
    
    // holds 4 frequencies
    char midiFreq[4] = {0, 0, 0, 0};


    // Initialize the MCU
    Initialize();
    
    // enable ADCs
    PINSEL0 |= ((1 << 30) | (1 << 31));      // P0.15 connected to ADC1.5
    PINSEL0 |= ((1 << 26) | (1 << 27));      // P0.13 connected to ADC1.4
    PINSEL0 |= ((1 << 24) | (1 << 25));      // P0.12 connected to ADC1.3
    
   
        // setup UART1 for midi reception
    /* initialize the serial interface   */
    PINSEL0 |= 0x00050000;          /* Enable RxD1 and TxD1                      */
    U1LCR = 0x83;                   /* 8 bits, no Parity, 1 Stop bit             */
    U1DLM = 0x00;
    U1DLL = 118;                   /* midi baud rate       */
    U1LCR = 0x03;                   /* DLAB = 0                                  */

    led_board(0);
    delay_ms(100);              // flash LEDs
    led_board(1);
    delay_ms(100);
    led_board(0);
    delay_ms(100);
    led_board(1);  
    
    synth_init();               // initialize synthesis

    for (;;){                   // go!!

        // no delay, because we might miss a midi byte!!  (but the midi input should be moved to a uart interrupt)
        //delay_ms(5);            
        
        knob0 = get_adc1(5);    // update the knobs
        knob1 = get_adc1(4);
        
        /* check midi */
        if ((U1LSR & 0x01)){   // check uart
            midiByte = U1RBR;
            led_board(1);      // get the midi byte, and flash the led
        }
        else{
            midiByte = 0;
            led_board(0);
        }    
        
        /* 
            Poor mans midi routine.  This just looks at note on and note off messages
              on channel 1.  It does simple voice allocation, no voice stealing.  (if 4 keys are
                held down, one of them must be released before another can be sounded).
        */
        if (midiByte){                          // if a new midi byte is just came in...
            if (midiState == NOTE_ON) {         // if the first byte in the packet was a note on
                for (i = 0; i < 4; i++){        // look for an empty voice
                    if (midiFreq[i] == 0){
                        midiFreq[i] = midiByte;
                        break;
                    }
                }
                midiState = 0;                  // midi state could be set to, for example, VELOCITY to process next byte
                                                // but we arn't using velocity information, so we reset this to 0
            }
            
            if (midiState == NOTE_OFF) {        // if the first byte in the packet was a note off
                for (i = 0; i < 4; i++){        // look for the voice that needs to be turned off
                    if (midiByte == midiFreq[i]){
                        midiFreq[i] = 0;
                        break;
                    }
                }
                midiState = 0;
            }
            
            if (midiByte == 0x90){             // if its a note on message
                midiState = NOTE_ON;
            }
            
            if (midiByte == 0x80){             // if its a note off message
                midiState = NOTE_OFF;
            }    
        }
        
        // update syntesizer.  the four parameters are:
        // voice number (0 - 3), carrier frequency, modulation index, and harmonicity
        // knob0 and knob1 control modulation and harmonicity
        for (i = 0; i < 4; i++){
            if (midiFreq[i])                    // check to see the note is on
                fm_voice(i, miditof[midiFreq[i] + 24] , knob0, knob1);   // play note (shifted up 2 octaves)
            else
                fm_voice(i, 0 , 1024, 1024);            // don't play anything if note is zero
        
        }
	} 
} // main()






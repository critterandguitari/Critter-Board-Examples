/******************************************************************************/
/*  main.c                                                        */
/*  (c)2006 GPL, Owen Osborn, Critter and Guitari                             */
/******************************************************************************/
/*                                                                            */
/*  :  main.c  8 voice cigar box synth                                              */
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


int main (void) {

    int i = 0;
    int j = 0;
    
    // holds values of 3 knobs
    int knob0 = 0;
    int knob1 = 0;
    int knob2 = 0;

    // red button
    int button = 1;

    // this is a representation of the states of the keys
    unsigned int keys[] = { 1, 1, 1,
                            1, 1, 1, 
                            1, 1, 1, 
                            1, 1, 1 }; 
              
    // frequency for each voice
    unsigned int voices[] = {0, 0, 0, 0};
    
    // holds octave value for each voice
    unsigned int octave[] = {1, 1, 1, 1};
    
    // stores next available voice (0 - 3)
    unsigned int nextVoice = 0;
    
    // used to allocate voice
    unsigned int noteIsOn = 0;

    // Initialize the MCU
    Initialize();
    
    // enable ADCs
    PINSEL0 |= ((1 << 30) | (1 << 31));      // P0.15 connected to ADC1.5
    PINSEL0 |= ((1 << 26) | (1 << 27));      // P0.13 connected to ADC1.4
    PINSEL0 |= ((1 << 24) | (1 << 25));      // P0.12 connected to ADC1.3

    led_board(0);
    delay_ms(100);              // flash LEDs
    led_board(1);
    delay_ms(100);
    led_board(0);
    delay_ms(100);
    led_board(1);  
    
    synth_init();               // initialize synthesis

    for (;;){                   // go!!

        delay_ms(5);            
        
        knob0 = get_adc1(5);    // update the knobs
        knob1 = get_adc1(4);
        knob2 = get_adc1(3);
        
        // read keys. each array element holds one key.  not very efficient use of 
        // space, but theres plenty ram on the LPC2138 ...
        keys[0] = (IOPIN0 & (1 << 3)) >> 3;         // move one pin to the first bit
        keys[1] = (IOPIN0 & (1 << 4)) >> 4;         
        keys[2] = (IOPIN0 & (1 << 5)) >> 5;
        
        keys[3] = (IOPIN0 & (1 << 6)) >> 6;
        keys[4] = (IOPIN0 & (1 << 7)) >> 7;
        keys[5] = (IOPIN0 & (1 << 8)) >> 8;
       
        keys[6] = (IOPIN0 & (1 << 9)) >> 9;
        keys[7] = (IOPIN0 & (1 << 10)) >> 10;
        keys[8] = (IOPIN0 & (1 << 11)) >> 11;
        
        keys[9] = (IOPIN0 & (1 << 16)) >> 16;
        keys[10] = (IOPIN0 & (1 << 20)) >> 20;
        keys[11] = (IOPIN0 & (1 << 23)) >> 23;
        
        button = (IOPIN1 & (1 << 26)) >> 26;        // get the button value
        
        
        for (i = 0; i < 12; i++){                   // for each key
            if (!keys[i]){                          // if the key is down (remember it's a 1 when up)
                for (j = 0; j < 4; j++){            // check if it's a currently playing note
                    if (miditof[i + OFFSET] == voices[j]){
                        noteIsOn = 1;
                        break;
                    }
                    else
                        noteIsOn = 0;
                }
                if (!noteIsOn){                     // if note is not playing, steal new voice
                    for (j = 0; j < 4; j++){        // look for an empty voice
                        if (voices[j] == 0){
                            voices[j] = miditof[i + OFFSET];
                            octave[j] = button;     // if the red button is pressed, it plays octave higher
                            break;
                        }
                    }
                }
            }
            else{                                   // otherwise key is up
                for (j = 0; j < 4; j++){            // check if it was a currently playing note
                    if (miditof[i + OFFSET] == voices[j]){
                        voices[j] = 0;              // mute mute the note if so
                        octave[j] = 1;              // set octave to normal
                    }
                }
            }// key up
        }
        
        // update syntesizer.  the four parameters are:
        // voice number (0 - 3), carrier frequency, modulation index, and harmonicity
        // the carrier frequencty is scaled by the tuning knob (knob2).  It gets shifted an extra
        //   spot depending on octave[x], so if octave[x] is 0, the note plays an octave higher.
        // knob0 and knob1 control modulation and harmonicity
        fm_voice(0, (voices[0] * knob2) >> (8 + octave[0]), knob0, knob1);
        fm_voice(1, (voices[1] * knob2) >> (8 + octave[1]), knob0, knob1);
        fm_voice(2, (voices[2] * knob2) >> (8 + octave[2]), knob0, knob1);
        fm_voice(3, (voices[3] * knob2) >> (8 + octave[3]), knob0, knob1);
	} 
	
} // main()






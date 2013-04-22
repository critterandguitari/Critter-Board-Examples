/*
 ****************************************************************************
 *
 *  Hello Sine Wave
 *
 ****************************************************************************
 */

#include "LPC21xx.h"
#include "system.h"
#include "audio_sinewave.h"

int main (void) {

    // Initialize the MCU
    Initialize();

    led_board(0);
    delay_ms(100);              // flash LEDs
    led_board(1);
    delay_ms(100);
    led_board(0);
    delay_ms(100);
    led_board(1);  
    
    synth_init();               // initialize synthesis
    
    synth_amp(255);             // full volume
    
    synth_freq(440);            // play 440 Hz for 2 seconds
    delay_ms(2000);             
    
    for (;;){                   
      
        synth_freq(200);        // play four notes over and over
        delay_ms(500);
        synth_freq(400);
        delay_ms(500);
        synth_freq(600); 
        delay_ms(500);
        synth_freq(800);
        delay_ms(500);
        
        
	} 
	
} // main()




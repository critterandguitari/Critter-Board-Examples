/******************************************************************************/
/*  Flash Sampler Tutorial                                                    */
/*  (c)2006 GPL, Owen Osborn, Critter and Guitari, Dearraindrop               */
/******************************************************************************/
/*                                                                            */
/*  :  Sampler for the Critter Board with on-board flash or flash cards       */
/*                                                                            */
/******************************************************************************/

#include "LPC21xx.h"
#include "system.h"
#include "sample_play.h"
#include "sample_record.h"

int main (void) {
    
    int record_rdy = 1;

    // Initialize the MCU
    Initialize();
    
    spi_1_init();             // set spi port for flash card
    
    PINSEL1 |= ((1 << 22) | (1 << 24) | (1 << 26));  // ADC 0.0, 0.1, and 0.2 on
    PINSEL0 |= ((1 << 30)| (1 << 31));              //  ADC1.5 on

    led_board(0);
    delay_ms(100);              // flash LEDs
    led_board(1);
    delay_ms(100);
    led_board(0);
    delay_ms(100);
    
    led_board(0);  
    led_card(0);
    
    delay_ms(20);  
    
    

    
    delay_ms(20);

    sample_play_init();
    sample_play_start();

    
    delay_ms(200); 
    
          // 4 is button, 5 is down, 3 is up
          
          
    
    for (;;){  
        delay_ms(5);    
        
        if (SWITCH_UP && record_rdy){
            if (RED_BUTTON){
                sample_play_stop();
                led_board(1);  
                led_card(1);
                bulk_erase();
            	delay_ms(7500);
            	
                led_board(0);  
                led_card(0); 
           
            	sample_record_line_init();
            	sample_record_line();
            	
                sample_play_init();
                sample_play_start();
            	
                record_rdy = 0;
            }
        }
        else if (!SWITCH_UP)
            record_rdy = 1;
                    
    
        if (SWITCH_DOWN){
            sampler_change(get_adc0(0) * 500, get_adc0(1) * 32, get_adc0(2), 1);
        }
        else
            sampler_change(get_adc0(0) * 500, get_adc0(1) * 32, get_adc0(2), 0);

	} 
	
} // main()




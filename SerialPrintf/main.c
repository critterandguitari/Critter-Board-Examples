/*
 ****************************************************************************
 *
 *  Hello Sine Wave
 *
 ****************************************************************************
 */

#include "LPC21xx.h"
#include "adc.h"
#include "system.h"

int main (void) {
    
    int i = 0;

    // Initialize the MCU
    Initialize();

    led_board(0);
    delay_ms(100);              // flash LEDs
    led_board(1);
    delay_ms(100);
    led_board(0);
    delay_ms(100);
    led_board(1);  
    
    delay_ms(1000);             // wait a second
    
    printf("Hello World!!\n\r");
    
    i = 2334780;
    
    printf("The value of i is: %d\n\r", i);
    
    
    PINSEL0 |= ((1 << 30)| (1 << 31));      // configure pin P0.15 connected to ADC1.5
   
    for(;;){
       
        i = get_adc1(5);    
        
        printf("The value of ADC 1.5:  %d\n\r", i);
        
        delay_ms(100);
       
    }           
    
} // main()




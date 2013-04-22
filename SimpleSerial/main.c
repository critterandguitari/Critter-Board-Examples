/*
 ****************************************************************************
 *
 *  Hello Sine Wave
 *
 ****************************************************************************
 */

#include "LPC21xx.h"
#include "system.h"

int main (void) {
    // Initialize the MCU, also sets up the serial connection to 115200
    Initialize();

    led_board(0);
    delay_ms(100);              // flash LEDs
    led_board(1);
    delay_ms(100);
    led_board(0);
    delay_ms(100);
    led_board(1);  
    
    put_char('R');
    put_char('e');
    put_char('a');
    put_char('d');
    put_char('y');
    put_char('\n');
   
    for(;;){
        put_char(get_char());  // echo character
    }           
} // main()




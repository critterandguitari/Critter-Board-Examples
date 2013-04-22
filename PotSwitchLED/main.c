/*
 ****************************************************************************
 *
 *  Pot, Switch, LED, test
 *      This program demonstrates the use of a switch, LED, and potentiometer.
 *      Two LEDs are connected to P0.23 and P0.20
 *      One switch is connected to P0.13
 *      One potentiometer is connected to P0.15 (ADC1.5)
 *      Diagrams for connecting LEDs, Switches, and Pots can be found at:
 *          http://www.cigarsynth.com/tutorials/leds.shtml
 *
 ****************************************************************************
 */

#include "LPC21xx.h"
#include "system.h"
#include "adc.h"

int main (void) {
    
    int flashRate = 0;         // holds the rate for flashing LED

    // Initialize the MCU
    Initialize();

    led_board(0);               // flash on-board status LED
    delay_ms(100);              // both of delay_ms and led_board are in system.c
    led_board(1);
    delay_ms(100);
    led_board(0);
    delay_ms(100);
    led_board(1);  
             
    /*
        The LPC has 2 32 bit I/O ports, PORT0 and PORT1.
        The IODIR0 register controls the direction of pins on PORT0.
        A 0 is input, 1 is output.  We will connect two LEDs to pins P0.23 and P0.20,
        so they are set to output.
    */ 
    IODIR0 |= ((1 << 23) | (1 << 20));      // P0.23 and P0.20 set to output
    
    /*
        The LPC2138 uses pin select registers to select various alternate functions of pins.
        There are 2 bits for each pin.  We want pin P0.15 to be connected to the analog to 
        digital converter, accomplised by writing 1s to bits 30 and 31 of pin select register 0.
        See the LPC2138 documemtation for more info on pins and the pin connect block.
    */
    PINSEL0 |= ((1 << 30)| (1 << 31));      // P0.15 connected to ADC1.5
    
    
    for (;;){          
    
        if (!(IOPIN0 & (1 << 13))){        // The IOPIN0 register contains the pin values of PORT0
                                           // this statement checks if pin P0.13 is low.
                                           // so if the button is pressed, light one LED, and flash the other
                                           // at a rate specified by the knob.
            
            IOCLR0 |= (1 << 20);            // the IOCLR0 register is used to drive an IO pin on PORT0 low 
                                            // causing the LED to light
                
            flashRate = get_adc1(5);        //  get_adc1 returns an integer from 0 - 1023 (the ADCs are 10 bits).
            flashRate /= 4;                 // scale flash rate from 0 - 255 ms
            
            IOSET0 |= (1 << 23);            // the IOSET0 register is used to drive an IO pin on PORT0 high
            delay_ms(flashRate);        
            IOCLR0 |= (1 << 23);            
            delay_ms(flashRate);
        }
        else{                               // if switch is up, turn LEDs off
            IOSET0 |= (1 << 23) | (1 << 20); 
        }
        
	} 
	
} // main()




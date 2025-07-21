#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

#define RED_LED    (1U << 1) // PF1
#define BLUE_LED   (1U << 2) // PF2
#define SWITCH1    (1U << 4) // PF4
//#define SWITCH2    (1U << 0) // PF4

void SysTick_Handler(void);
void Sys_Port_Initialisation(void);
void SysTick_Initialisation(void);
int x = 0;

int main(void) {

    Sys_Port_Initialisation();
    SysTick_Initialisation();
    GPIO_PORTF_DATA_R |= BLUE_LED;         // turn off led in starting

    while (1) {
        //if ((GPIO_PORTF_DATA_R & SWITCH1) == 0) GPIO_PORTF_DATA_R |= RED_LED;  // checking switch1 input (PF4 is low) if yes Set PF2 (BLUE_LED) High
        //else  GPIO_PORTF_DATA_R &= ~RED_LED;                                   // set PF2 (BLUE_LED) Low
    }
}

void SysTick_Handler(void) {

    x = x+1;
    if(x == 1){
        NVIC_ST_RELOAD_R = 2400;        // Set reload value for 0.2 milli second led off, 1 KHz ( 12 MHz system clock)
        GPIO_PORTF_DATA_R &= ~BLUE_LED;         // switching off (BLUE_LED)
    }
    if(x == 2){
        NVIC_ST_RELOAD_R = 9600;        // Set reload value for 0.8 milli second led off, 1 KHz ( 12 MHz system clock)
        GPIO_PORTF_DATA_R |= BLUE_LED;         // switching on (BLUE_LED)
        x = 0;
    }
}

void Sys_Port_Initialisation(void){

    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;   // Enable clock for Port F
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;      // Unlock Port F
    GPIO_PORTF_CR_R = 0x1F;                 //commit changes
    GPIO_PORTF_DEN_R = 0x1E;                // Set Port F pins 1, 2, 3 as digital outputs
    GPIO_PORTF_DIR_R = 0x0E;                // Set PF1, PF2, PF3 as output (LEDs), PF4, PF0 as input (Switch)
    GPIO_PORTF_PUR_R = SWITCH1;             // Enable pull-up resistor on PF4 (Switch1)
    //GPIO_PORTF_PUR_R = SWITCH2;             // Enable pull-up resistor on PF0 (Switch2)
}
void SysTick_Initialisation(void){
    NVIC_ST_CTRL_R = 0;                     // Disable SysTick during setup
    NVIC_ST_RELOAD_R = 2400 - 1;            // Set reload value for 0.5 second ( 16 MHz system clock)
    NVIC_ST_CURRENT_R = 0;                  // Clear the current value register
    //NVIC_ST_CTRL_R = NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_INTEN | NVIC_ST_CTRL_ENABLE; // Enable SysTick with system clock, interrupts, and the timer
    NVIC_ST_CTRL_R = 0x7;                   // same as above
}


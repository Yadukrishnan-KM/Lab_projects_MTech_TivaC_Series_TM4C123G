#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

#define RED_LED    (1U << 1) // PF1
#define BLUE_LED   (1U << 2) // PF2
#define SWITCH1    (1U << 4) // PF4
#define debounce 6
uint8_t count = 0;
bool lost = true;
bool pressed = false;

//#define SWITCH2    (1U << 0) // PF4

void SysTick_Handler(void);
void Sys_Port_Initialisation(void);
void SysTick_Initialisation(void);

int main(void) {

    Sys_Port_Initialisation();
    SysTick_Initialisation();
    GPIO_PORTF_DATA_R |= BLUE_LED;         // turn off led in starting

    /*
    GPIO_PORTF_RIS_R = 0;
    GPIO_PORTF_IEV =
    GPIO_PORTF_IM_R = 0x01;
    GPIO_PORTF_ICR_R = 0x10;
    */

    while (1) {
        bool current = ((GPIO_PORTF_DATA_R & SWITCH1) == 0);

        if(current == lost){
            count++;
            if(count >= debounce){
                if(current && !pressed){
                    pressed = true;
                    GPIO_PORTF_DATA_R |= RED_LED;
                }
            }
        }
//        if ((GPIO_PORTF_DATA_R & SWITCH1) == 0)
//        {
//            GPIO_PORTF_DATA_R |= RED_LED;  // checking switch1 input (PF4 is low) if yes Set PF2 (BLUE_LED) High
//
//        }
        else count = 0;
        if(!current){
            pressed = false;
            GPIO_PORTF_DATA_R &= ~RED_LED;

        }
        lost = current;
//        else {
//            GPIO_PORTF_DATA_R &= ~RED_LED;                                   // set PF2 (BLUE_LED) Low
//        }
//       // SysTick_Initialisation();
    }
}

void SysTick_Handler(void) {

        SysTick_Initialisation();
        GPIO_PORTF_DATA_R ^= BLUE_LED;         // toggle (BLUE_LED)
        SysTick_Initialisation();
}

void Sys_Port_Initialisation(void){

    // PORTF, PF7-PF0, PF4-SW1, PF3-green, PF2-blue, PF1-red, PF0-SW2
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;   // Enable clock for Port F
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;      // Unlock Port F
    GPIO_PORTF_CR_R = 0x1f;                 // Commit changes,1-enable (PF7-PF0 = 00011111)
    GPIO_PORTF_DEN_R = 0x1f;                // Digital function enable, 1-enable (PF7-PF0 = 00011111)
    GPIO_PORTF_DIR_R = 0x0e;                // Set output/input, 1-output (PF7-PF0 = 00001110)
    GPIO_PORTF_PUR_R = 0x11;                // Enable pull-up resistor, 1-enable (PF7-PF0 = 00010001)
    GPIO_PORTF_DATA_R = 0x00;               // Reset the data register (PF7-PF0 = 00000000)

}

void SysTick_Initialisation(void){

   // NVIC_ST_CTRL_R = 0x00;                  // SysTick control, disable during setup

    NVIC_ST_CURRENT_R = 0;                  // Current value
    NVIC_ST_CTRL_R = 0x07;                  // SysTick control, last 3 bits, clk source(1-sys clk, 0-internal osc/4), interrupt enable(1), timer enable(1)
    NVIC_ST_RELOAD_R = 7999999;         // Reload value,Set reload value for 0.5 second ( 16 MHz system clock)
}













/* #include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

#define RED_LED    (1U << 1) // PF1
#define BLUE_LED   (1U << 2) // PF2
#define SWITCH1    (1U << 4) // PF4
#define SWITCH2   (1U << 0) // PF4

int i = 0;

void SysTick_Handler(void);
void Sys_Port_Initialisation(void);
void SysTick_Initialisation(void);



int main(void) {
    Sys_Port_Initialisation();
    SysTick_Initialisation();

    while (1) {
        if ((GPIO_PORTF_DATA_R & SWITCH2) == 0) GPIO_PORTF_DATA_R |= RED_LED;  // checking switch1 input (PF4 is low) if yes Set PF2 (BLUE_LED) High
        else  GPIO_PORTF_DATA_R &= ~RED_LED;                                   // set PF2 (BLUE_LED) Low
    }
}

void SysTick_Handler(void) {

    SysTick_Initialisation();
    GPIO_PORTF_DATA_R ^= BLUE_LED;           // Toggling PF1 (RED_LED)
    SysTick_Initialisation();

}
void Sys_Port_Initialisation(void){
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;   // Enable clock for Port F
        GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;      // Unlock Port F
        GPIO_PORTF_CR_R = 0x1f;                 // Commit changes,1-enable (PF7-PF0 = 00011111)
        GPIO_PORTF_DEN_R = 0x1f;                // Digital function enable, 1-enable (PF7-PF0 = 00011111)
        GPIO_PORTF_DIR_R = 0x0e;                // Set output/input, 1-output (PF7-PF0 = 00001110)
        GPIO_PORTF_PUR_R = 0x11;                // Enable pull-up resistor, 1-enable (PF7-PF0 = 00010001)
        GPIO_PORTF_DATA_R = 0x00;               // Reset the data register (PF7-PF0 = 00000000)

    /*SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;   // Enable clock for Port F
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;      // Unlock Port F
    GPIO_PORTF_CR_R = 0x1F;                 //commit changes
    GPIO_PORTF_DEN_R = 0x1E;                // Set Port F pins 1, 2, 3 as digital outputs
    GPIO_PORTF_DIR_R = 0x0E;                // Set PF1, PF2, PF3 as output (LEDs), PF4, PF0 as input (Switch)
    GPIO_PORTF_PUR_R = SWITCH1;             // Enable pull-up resistor on PF4 (Switch1)
    //GPIO_PORTF_PUR_R = SWITCH2;             // Enable pull-up resistor on PF0 (Switch2)*/
}
/*
void SysTick_Initialisation(void){
    NVIC_ST_CURRENT_R = 0;                  // Clear the current value register
    NVIC_ST_CTRL_R = 0x07;                     // Disable SysTick during setup
    NVIC_ST_RELOAD_R = 4000000;        // Set reload value for 0.5 second ( 16 MHz system clock)
    //NVIC_ST_CTRL_R = NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_INTEN | NVIC_ST_CTRL_ENABLE; // Enable SysTick with system clock, interrupts, and the timer

}
*/

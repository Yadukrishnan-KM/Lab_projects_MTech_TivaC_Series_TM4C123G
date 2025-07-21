#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

#define SWITCH1    (1U << 4) // PF4
#define SWITCH2    (1U << 0) // PF4


void LED_Init(void);
void Timer0A_Init(uint32_t period);
void Timer0Handler(void);

volatile bool ledState = false; // State of the LED

int main(void) {

    LED_Init();    // Initialize the LED
    Timer0A_Init(16000000); // 1 second period with 16MHz clock

    while (1) {if((GPIO_PORTF_DATA_R & SWITCH2 ) == 0){
        GPIO_PORTF_DATA_R |= 0x08;         // switching on (green_LED)
    }
    else GPIO_PORTF_DATA_R &= ~0x08;         // switching off (green_LED)
        // Main loop can do other tasks, LED toggling is handled in the interrupt
    }
}

void LED_Init(void) {

    // PORTF, PF7-PF0, PF4-SW1, PF3-green, PF2-blue, PF1-red, PF0-SW2
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;   // Enable clock for Port F
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;      // Unlock Port F
    GPIO_PORTF_CR_R = 0x1f;                 // Commit changes,1-enable (PF7-PF0 = 00011111)
    GPIO_PORTF_DEN_R = 0x1f;                // Digital function enable, 1-enable (PF7-PF0 = 00011111)
    GPIO_PORTF_DIR_R = 0x0e;                // Set output/input, 1-output (PF7-PF0 = 00001110)
    GPIO_PORTF_PUR_R = 0x11;                // Enable pull-up resistor, 1-enable (PF7-PF0 = 00010001)
    GPIO_PORTF_DATA_R = 0x00;               // Reset the data register (PF7-PF0 = 00000000)
}

void Timer0A_Init(uint32_t period) {
    SYSCTL_RCGCTIMER_R |= 0x01;     // Enable Timer0
    TIMER0_CTL_R = 0;                // Disable Timer0 during setup
    TIMER0_CFG_R = 0x00000000;       // 32-bit timer
    TIMER0_TAMR_R = 0x00000002;      // Periodic timer mode
    TIMER0_TAILR_R = 16000000 - 1;     // Load the interval value
    TIMER0_IMR_R = 0x00000001;       // Enable Timer0A timeout interrupt
    NVIC_EN0_R = 1 << 19;            // Enable interrupt for Timer0A in NVIC
    TIMER0_CTL_R |= 0x00000001;      // Enable Timer0A
}

void Timer0Handler(void) {
    TIMER0_ICR_R = 0x00000001;       // Clear the interrupt flag
    ledState = !ledState;             // Toggle LED state
    if (ledState) {
        GPIO_PORTF_DATA_R |= 0x02;   // Turn on LED
    } else {
        GPIO_PORTF_DATA_R &= ~0x02;  // Turn off LED
    }
}

/*
 #include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

#define SWITCH1    (1U << 4) // PF4
#define SWITCH2    (1U << 0) // PF4

void Sys_Port_Initialisation(void);
void Timer_Initialisation(void);
void Timer0Handler(void);
volatile bool ledState = false; // State of the LED

int x=0;

int main(void) {

    Sys_Port_Initialisation();

    while (1) {
        if((GPIO_PORTF_DATA_R & SWITCH2 ) == 0){
            GPIO_PORTF_DATA_R |= 0x08;         // switching on (green_LED)
        }
        else GPIO_PORTF_DATA_R &= ~0x08;         // switching off (green_LED)

    }
}

void Timer0Handler(){

    TIMER0_ICR_R = 0x00000001;       // Clear the interrupt flag
    ledState = !ledState;             // Toggle LED state
    if (ledState) {
        GPIO_PORTF_DATA_R |= 0x02;   // Turn on LED
    } else {
        GPIO_PORTF_DATA_R &= ~0x02;  // Turn off LED
    }

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

void Timer_Initialisation(){

    SYSCTL_RCGCTIMER_R |= 0x01;     // Enable Timer0
    TIMER0_CTL_R = 0;                // Disable Timer0 during setup
    TIMER0_CFG_R = 0x00000000;       // 32-bit timer
    TIMER0_TAMR_R = 0x00000002;      // Periodic timer mode
    TIMER0_TAILR_R = 16000000 - 1;     // Load the interval value
    TIMER0_IMR_R = 0x00000001;       // Enable Timer0A timeout interrupt
    NVIC_EN0_R = 1 << 19;            // Enable interrupt for Timer0A in NVIC
    TIMER0_CTL_R |= 0x00000001;      // Enable Timer0A

}

 */



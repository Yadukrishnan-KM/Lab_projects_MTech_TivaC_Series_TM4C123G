#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "definitions.h"

// PA0 as input for UART RX and PA1 as output for UART TX

void GPIO_PortA_Initialisation(void){

    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;// Enable clock for Port A
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R0) == 0); // Allow time for clock to stabilize
    GPIO_PORTA_LOCK_R = GPIO_LOCK_KEY;      // Unlock Port A
    GPIO_PORTA_CR_R |= 0x03;                // Commit changes,1-enable (PA7-PA0 = 00000011)
    GPIO_PORTA_DEN_R |= 0x03;               // Digital function enable, 1-enable (PA7-PA0 = 00000011)
    GPIO_PORTA_DIR_R |= 0x02;               // Set output/input, 1-output (PA7-PA0 = 00000010)
    GPIO_PORTA_PUR_R |= 0x01;               // Enable pull-up resistor, 1-enable (PA7-PA0 = 00000001)
    GPIO_PORTA_DATA_R = 0x00;               // Reset the data register (PA7-PA0 = 00000000)

}

void GPIO_PortA_Interrupt_Initialisation(void){

     // Setting PORTA interrupt registers

     GPIO_PORTA_IS_R = 0x00;                // Interrupt sense, 1-level, 0-edge (PA7-PA0 = 00000000)
     GPIO_PORTA_IBE_R = 0x00;               // Interrupt both edge, 1-both edge, 0-single edge (PA7-PA0 = 00000000)
     GPIO_PORTA_IEV_R = 0x00;               // Interrupt event, 0-falling edge, 1-falling edge (PA7-PA0 = 00000000)
     GPIO_PORTA_ICR_R = 0xff;               // Interrupt clear, 1-clear all prior interrupts (PA7-PA0 = 11111111)
     GPIO_PORTA_IM_R = 0x01;                // Interrupt mask, 1-Enable interrupt (PA7-PA0 = 00000001)

     //NVIC_PRI7_R &= 0xFF3FFFFF;             // Prioritize and enable interrupts in NVIC

     // Enable the GPIO Port A interrupt in the NVIC
     NVIC_EN0_R |= 0x00000001;              // Port A is interrupt 23 in the NVIC port is in 1st position

     //__asm("    cpsie i");                // Global interrupt enable

}

void Timer_0_Initialisation(void) {

    // Timer 0 Initialisation

    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R0;            // Enable Timer 0 clock
    while ((SYSCTL_PRTIMER_R & SYSCTL_PRTIMER_R0) == 0);  // Wait for Timer 0 to be ready

    TIMER0_CTL_R = 0x00;                   // Disable Timer 0 while configuring
    TIMER0_CFG_R = 0x04;                   // 16-bit mode
    TIMER0_TAMR_R = 0x02;                  // Periodic mode (count down)
    TIMER0_TAPR_R = 0x00;                  // Set prescalar, no prescalar

    // Set the timer period (using 1667 cycles for 9600 baud)
    TIMER0_TAILR_R = 0x0683;               // Load the timer with the period value, 1667 = 16000000/9600

    TIMER0_IMR_R = 0x01;                   // Enable timeout interrupt
    NVIC_EN0_R |= (1 << 19);               // Enable Timer 0 interrupt in NVIC
    TIMER0_CTL_R |= 0x01;                  // Enable Timer 0
}

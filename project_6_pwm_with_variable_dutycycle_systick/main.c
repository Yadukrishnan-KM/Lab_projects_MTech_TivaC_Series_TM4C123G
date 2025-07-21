#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

void SysTick_Initialisation(void);
void Sys_Port_Initialisation(void);
void GPIOinterrupt_Initialisation(void);
void SysTick_Handler(void);
void GPIOF_Handler(void);
void PwmControl(void);

int state = 0;                          // Variable for changing on/off of pulse
double clkFreq = 16;                    // Clock frequency for the systick timer in MHz
double period = 100;                // Time period required for PWM in microseconds            designed for 10KHz
double onTime = 0;                      // On time of pulse
double offTime = 0;                     // Off time of pulse
double pwmDutyCycle = 50;               // Duty cycle of pulse in percentage

int main(void) {

    Sys_Port_Initialisation();
    SysTick_Initialisation();
    GPIOinterrupt_Initialisation();
    PwmControl();

    while (1) {

    }
}

void PwmControl(){

    double x = clkFreq*period;
    onTime = x *(pwmDutyCycle/100);
    offTime = x * (100 - pwmDutyCycle)/100;
}

void SysTick_Handler(void) {

    state = state+1;
    if(state == 1){
        if(onTime == 0) onTime = 50;      // When reload value is zero not working
        NVIC_ST_RELOAD_R = onTime;         // Set reload value for pulse high level
        GPIO_PORTF_DATA_R &= ~0x04;        // switching off (BLUE_LED)
    }
    if(state == 2){
        if(offTime == 0) offTime = 50;    // When reload value is zero not working
        NVIC_ST_RELOAD_R = offTime;        // Set reload value for pulse low level
        GPIO_PORTF_DATA_R |= 0x04;         // switching on (BLUE_LED)
        state = 0;
    }
}

void GPIOF_Handler(void) {

    if(GPIO_PORTF_MIS_R == 0x10){           // Interrupt from SW1
        //GPIO_PORTF_DATA_R ^= 0x08;          // Toggle data in PF1 (green led)
        if(pwmDutyCycle != 100)pwmDutyCycle = pwmDutyCycle+5;      // Increasing duty cycle by 5 %
        PwmControl();
    }

    if(GPIO_PORTF_MIS_R == 0x01){           // Interrupt from SW2
        //GPIO_PORTF_DATA_R ^= 0x02;          // Toggle data in PF1 (red led)
        if(pwmDutyCycle != 0) pwmDutyCycle = pwmDutyCycle-5;      // Decreasing duty cycle by 5 %
        PwmControl();
    }

    GPIO_PORTF_ICR_R = 0x11;                // Interrupt clear, 1-clear all prior interrupts (PF7-PF0 = 00010001)

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
void GPIOinterrupt_Initialisation(void){

     // Setting PORTF interrupt registers
     GPIO_PORTF_IS_R = 0x00;                // Interrupt sense, 1-level, 0-edge (PF7-PF0 = 00000000)
     GPIO_PORTF_IBE_R = 0x00;               // Interrupt both edge, 1-both edge, 0-single edge (PF7-PF0 = 00000000)
     GPIO_PORTF_IEV_R = 0x00;               // Interrupt event, 0-falling edge, 1-falling edge (PF7-PF0 = 00000000)
     GPIO_PORTF_ICR_R = 0xff;               // Interrupt clear, 1-clear all prior interrupts (PF7-PF0 = 11111111)
     GPIO_PORTF_IM_R = 0x11;                // Interrupt mask, 1-Enable interrupt (PF7-PF0 = 00010001)

     // Enable the GPIO Port F interrupt in the NVIC
     NVIC_EN0_R = 0x40000000;               // Port F is interrupt 23 in the NVIC port is in 31st position

     //__asm("    cpsie i");                // Global interrupt enable

}

void SysTick_Initialisation(void){

    NVIC_ST_CTRL_R = 0x00;                  // SysTick control, disable during setup
    NVIC_ST_RELOAD_R = 8000000 - 1;         // Reload value,Set reload value for 0.5 second ( 16 MHz system clock)
    NVIC_ST_CURRENT_R = 0;                  // Current value
    NVIC_ST_CTRL_R = 0x07;                  // SysTick control, last 3 bits, clk source(1-sys clk, 0-internal osc/4), interrupt enable(1), timer enable(1)

}

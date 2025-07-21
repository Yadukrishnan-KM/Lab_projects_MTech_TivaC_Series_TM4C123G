#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "definitions.h"

bool switch_1 = 0;
bool switch_2 = 0;

void GPIO_PortF_Interrupt_Initialisation(void){

     // Setting PORTF interrupt registers
     GPIO_PORTF_IS_R = 0x00;                // Interrupt sense, 1-level, 0-edge (PF7-PF0 = 00000000)
     GPIO_PORTF_IBE_R = 0x00;               // Interrupt both edge, 1-both edge, 0-single edge (PF7-PF0 = 00000000)
     GPIO_PORTF_IEV_R = 0x00;               // Interrupt event, 0-falling edge, 1-falling edge (PF7-PF0 = 00000000)
     GPIO_PORTF_ICR_R = 0xff;               // Interrupt clear, 1-clear all prior interrupts (PF7-PF0 = 11111111)
     GPIO_PORTF_IM_R = 0x11;                // Interrupt mask, 1-Enable interrupt (PF7-PF0 = 00010001)

     NVIC_PRI7_R &= 0xFF3FFFFF;             // Prioritize and enable interrupts in NVIC

     // Enable the GPIO Port F interrupt in the NVIC
     NVIC_EN0_R = 0x40000000;               // Port F is interrupt 23 in the NVIC port is in 31st position

     //__asm("    cpsie i");                // Global interrupt enable

}

// Interrupt Handlers

void GPIO_PortF_Interrupt_Handler(void)
{
    if(GPIO_PORTF_MIS_R == 0x10){           // Interrupt from SW1

        switch_1 = 1;                       // Switch 1 press flag
    }

    if(GPIO_PORTF_MIS_R == 0x01){           // Interrupt from SW2

        switch_2 = 1;                       // Switch 2 press flag
    }

    GPIO_PORTF_ICR_R = 0x11;                // Interrupt clear, 1-clear all prior interrupts (PF7-PF0 = 00010001)
}

// Other functions

void SysTick_Initialisation(void){
  NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
  NVIC_ST_RELOAD_R = 15999;         // number of counts to wait, 1 mS
  NVIC_ST_CURRENT_R = 0;            // clears current value register
  NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock
}


/*

 Embedded course project

 Written by Yadukrishnan K M, EE24MT013, IIT Dharwad, India

 17/11/2024

 It will receive the CHAR from the PC using UART interface and replay the same CHAR back to the PC

 Not using any hardware UART only using software UART using bit banging

 GPIO and timer interrupts are used for reception and making timing events

*/

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "definitions.h"

// All global variables

volatile char transmit_data = 0x00;
volatile int bit_count_tx = 0;
volatile char receive_data = 0x00;
volatile int bit_count_rx = 0;
volatile bool rx_ready = 1;
volatile bool tx_ready = 1;

void main(void){

    GPIO_PortA_Initialisation();
    GPIO_PortA_Interrupt_Initialisation();
    Timer_0_Initialisation();

    while(1){

    }
}

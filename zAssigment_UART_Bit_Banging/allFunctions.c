#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "definitions.h"

// Tx code while timer interrupt activates, 1 bit each time

void Tx_1_Byte(void) {

    if (bit_count_tx == 0) {
        GPIO_PORTA_DATA_R &= ~0x02;            // Start bit (low) in PA1
    } else if (bit_count_tx <= 8) {
        if (transmit_data & 0x01) {
            GPIO_PORTA_DATA_R |= 0x02;         // Data bit (high) in PA1
        } else {
            GPIO_PORTA_DATA_R &= ~0x02;        // Data bit (low) in PA1
        }
        transmit_data >>= 1;                   // Shift to next bit
    } else {
        GPIO_PORTA_DATA_R |= 0x02;             // Stop bit (high)  in PA1
        bit_count_tx = 0;                      // Reset bit count
        tx_ready = 1;
        return;
    }

    bit_count_tx = bit_count_tx + 1;            // Move to the next bit
}

// UART Reception, activates when GPIO interrupt and continue for 8 bits

void Rx_1_Byte(void) {
   int i;                                        // little delay to ensure that correct detection of data bits
   for(i=0; i<150; i++);
    /*if (bit_count_rx == 0){
        bit_count_rx = bit_count_rx + 1;         // Increment the bit count
    }*/
    if (bit_count_rx < 8) {
        if (GPIO_PORTA_DATA_R & 0x01) {          // Check if data bit is high
            receive_data |= (1 << bit_count_rx); // Set the corresponding bit in rxData
         }
        bit_count_rx = bit_count_rx + 1;         // Increment the bit count
        }
    else if(bit_count_rx == 8){
        bit_count_rx = 0;
        rx_ready = 1;
        tx_ready = 0;
        //transmit_data = 0b01101100;
        transmit_data = receive_data;
    }
}

void Timer0_Interrupt_Handler(void){

    TIMER0_ICR_R = 0x01;                         // Clear the interrupt flag of timer

    if(!rx_ready) Rx_1_Byte();                   // Execute receiving operation function by checking flag
    else if(!tx_ready) Tx_1_Byte();              // Execute transmitting operation function by checking flag

}

void GPIOA_Interrupt_Handler(void){

    GPIO_PORTA_ICR_R = 0x01;                     // Clear interrupt flag
    if(rx_ready) {
        rx_ready = 0;                            // Enable receiving operation flag
        receive_data = 0x00;                     // Clear the previous data
    }

}

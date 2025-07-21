#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

// Define LED pins
#define RED_LED    (1U << 1) // PF1
#define BLUE_LED   (1U << 2) // PF2
#define GREEN_LED  (1U << 3) // PF3

char letter;
bool status = false;

void PortF_Initialisation(void){

    // PORTF, PF7-PF0, PF4-SW1, PF3-green, PF2-blue, PF1-red, PF0-SW2
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;   // Enable clock for Port F
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;      // Unlock Port F
    GPIO_PORTF_CR_R = 0x1f;                 // Commit changes,1-enable (PF7-PF0 = 00011111)
    GPIO_PORTF_DEN_R = 0x1f;                // Digital function enable, 1-enable (PF7-PF0 = 00011111)
    GPIO_PORTF_DIR_R = 0x0e;                // Set output/input, 1-output (PF7-PF0 = 00001110)
    GPIO_PORTF_PUR_R = 0x11;                // Enable pull-up resistor, 1-enable (PF7-PF0 = 00010001)
    GPIO_PORTF_DATA_R = 0x00;               // Reset the data register (PF7-PF0 = 00000000)

}

void PORTA_Initialisation(void) {

    // Enable the clock for UART0 and GPIO Port A

    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGC2_GPIOA;     // Enable GPIO Port A clock
    SYSCTL_RCGCUART_R |= SYSCTL_RCGC1_UART0;     // Enable UART0 clock

    // Configure PA0 (RX) and PA1 (TX)

    GPIO_PORTA_LOCK_R = GPIO_LOCK_KEY;      // Unlock Port A
    GPIO_PORTA_CR_R = 0x03;                 // Commit changes,1-enable (PA7-PA0 = 00000011)
    GPIO_PORTA_DEN_R = 0x03;                // Digital enable PA0 and PA1
    GPIO_PORTA_AFSEL_R = 0x03;              // Enable alternate function for PA0 and PA1
    GPIO_PORTA_AMSEL_R = 0x00;              // Turnoff analog function
    GPIO_PORTA_PCTL_R = 0xFFFFFF11;         // Set PCTL for PA0 and PA1
}

void UART0_Initialisation(void) {

    // Configure UART0 for 9600 baud rate, 8 data bits, odd parity
    UART0_CTL_R = 0x00;                     // Disable UART before configuration
    UART0_IBRD_R = 104;                     // Integer part of BRD = 16MHz / (16 * 9600) = 104
    UART0_FBRD_R = 11;                      // Fractional part of BRD = 0.16 * 64 + 0.5 = 11
    UART0_CC_R = 0x00;                      // UART clock set to system clock
    UART0_LCRH_R = 0x60;                    // 8 bits, odd parity
    //UART0_LCRH_R = 0x72;                    // 8 bits, odd parity
    UART0_CTL_R = 0x301;                    // Enable UART

}

void LED_Control(void){

    if (letter == 'R'){

        GPIO_PORTF_DATA_R &= ~GREEN_LED;  // Turn off green LED
        GPIO_PORTF_DATA_R &= ~BLUE_LED;   // Turn off blue LED
        GPIO_PORTF_DATA_R |= RED_LED;     // Turn on red LED

    }
    else if (letter == 'G'){

        GPIO_PORTF_DATA_R |= GREEN_LED;   // Turn on green LED
        GPIO_PORTF_DATA_R &= ~BLUE_LED;   // Turn off blue LED
        GPIO_PORTF_DATA_R &= ~RED_LED;    // Turn off red LED

    }
    else if (letter == 'B'){

        GPIO_PORTF_DATA_R &= ~GREEN_LED;  // Turn off green LED
        GPIO_PORTF_DATA_R |= BLUE_LED;    // Turn on blue LED
        GPIO_PORTF_DATA_R &= ~RED_LED;    // Turn off red LED

    }
    else if (letter != 'R' && letter != 'G' && letter != 'B'){

        GPIO_PORTF_DATA_R &= ~GREEN_LED;  // Turn off green LED
        GPIO_PORTF_DATA_R &= ~BLUE_LED;   // Turn on blue LED
        GPIO_PORTF_DATA_R &= ~RED_LED;    // Turn off red LED

    }
}

int main(void) {

    PortF_Initialisation();
    PORTA_Initialisation();
    UART0_Initialisation();


    while (1) {

        while ((UART0_FR_R & 0x10) != 0);  // Wait until RXFE is 0
        letter = UART0_DR_R;               // Read data
        LED_Control();
        while (UART0_FR_R & UART_FR_TXFF); // Wait until the transmit FIFO is not full
        UART0_DR_R = letter;               // Transmit data
    }
}

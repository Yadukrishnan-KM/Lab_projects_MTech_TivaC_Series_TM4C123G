
#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

// Error codes
#define I2C_SUCCESS 0
#define I2C_ERROR -1

void I2C_Init(void) {
    // Enable the I2C0 module and GPIOB
    SYSCTL_RCGCI2C_R |= SYSCTL_RCGCI2C_R0;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1) == 0);

    // Configure PB0 and PB1 for I2C0
    GPIO_PORTB_AFSEL_R |= (1 << 0) | (1 << 1);
    GPIO_PORTB_DEN_R |= (1 << 0) | (1 << 1);
    GPIO_PORTB_ODR_R |= (1 << 0);

    // Configure I2C
    I2C0_MCR_R = I2C_MCR_MFE;
    I2C0_MTPR_R = 7;
}

int I2C_Transmit(uint8_t address, uint8_t data) {
    I2C0_MSA_R = (address << 1);
    I2C0_MDR_R = data;
    I2C0_MCS_R = I2C_MCS_START | I2C_MCS_RUN;
    while (I2C0_MCS_R & I2C_MCS_BUSY);
    return (I2C0_MCS_R & I2C_MCS_ERROR) ? I2C_ERROR : I2C_SUCCESS;
}

int I2C_Receive(uint8_t address, uint8_t* data) {
    I2C0_MSA_R = (address << 1) | 1;
    I2C0_MCS_R = I2C_MCS_START | I2C_MCS_RUN;
    while (I2C0_MCS_R & I2C_MCS_BUSY);
    if (I2C0_MCS_R & I2C_MCS_ERROR) return I2C_ERROR;
    *data = I2C0_MDR_R;
    return I2C_SUCCESS;
}

// Main function
int main(void) {
    uint8_t data;

    I2C_Init(); // Initialize I2C

    // Example usage
    if (I2C_Transmit(0x50, 0xAB) == I2C_SUCCESS) {
        // Transmission successful
    }

    if (I2C_Receive(0x50, &data) == I2C_SUCCESS) {
        // Reception successful
    }

    while (1);
}



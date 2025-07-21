#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

// DAC I2C Address
#define DAC_ADDRESS 0x60  // MCP4725 address (adjust if needed)

// Constants for triangular waveform
#define MAX_VALUE 4095     // Max value for a 12-bit DAC
#define MIN_VALUE 0        // Min value
#define STEP 50            // Step size for waveform

void I2C0_Init(void) {
    // Enable I2C0
    SYSCTL_RCGCI2C_R |= (1U << 0); // Enable I2C0
    SYSCTL_RCGCGPIO_R |= (1U << 1); // Enable Port B

    // Set PB0 and PB1 as I2C pins
    GPIO_PORTB_AFSEL_R |= (1U << 0) | (1U << 1); // PB0 and PB1 as I2C
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFFFF00) | 0x00000033; // Set PB0 and PB1 to I2C
    GPIO_PORTB_DEN_R |= (1U << 0) | (1U << 1); // Enable digital function
    I2C0_MCR_R = I2C_MCR_MFE; // Enable I2C0 master
}

void I2C0_Write(uint16_t value) {
    // Prepare data for DAC (upper and lower byte)
    uint8_t data[2];
    data[0] = (value >> 4) & 0x0F; // Upper 4 bits
    data[1] = (value & 0x0F) << 4;  // Lower 8 bits with write command

    I2C0_MSA_R = DAC_ADDRESS; // Set slave address
    I2C0_MDR_R = data[0];     // Send upper byte
    I2C0_MCS_R = I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP; // Start and stop condition

    // Wait for I2C operation to complete
    while (I2C0_MCS_R & I2C_MCS_BUSY);

    // Send lower byte
    I2C0_MDR_R = data[1];
    I2C0_MCS_R = I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP; // Start and stop condition
}

void SimpleDelay(uint32_t count) {
    volatile uint32_t i;
    while (count--) {
        // Just a simple loop to create a delay
        for (i = 0; i < 100; i++);
    }
}

void Generate_Triangular_Waveform(void) {
    uint16_t value = MIN_VALUE;
    bool increasing = true;

    while (1) {
        I2C0_Write(value); // Write the value to the DAC
        SimpleDelay(40000); // Delay to control frequency

        // Update value for triangular waveform
        if (increasing) {
            value += STEP;
            if (value >= MAX_VALUE) {
                value = MAX_VALUE;
                increasing = false; // Switch direction
            }
        } else {
            value -= STEP;
            if (value <= MIN_VALUE) {
                value = MIN_VALUE;
                increasing = true; // Switch direction
            }
        }
    }
}

int main(void) {
    I2C0_Init(); // Initialize I2C0
    Generate_Triangular_Waveform(); // Generate the triangular waveform
}

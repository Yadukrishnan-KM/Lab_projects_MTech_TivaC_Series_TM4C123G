#include <stdint.h>
#include "tm4c123gh6pm.h"

// Global variable for duty cycle (initially 30%)
volatile uint8_t dutyCycle = 30;

void Timer0A_Init(uint32_t period, uint32_t duty);
void PortF_Init(void);
void Timer0A_SetDutyCycle(uint8_t duty);

int main(void)
{
    PortF_Init();             // Initialize Port F for LED
    Timer0A_Init(160000, 48);    // Initialize Timer0A for PWM (period = 160, duty = 48)

    while (1)
    {
        // Update the duty cycle dynamically if needed
        Timer0A_SetDutyCycle(dutyCycle);
    }
}

void PortF_Init(void)
{
    SYSCTL_RCGCGPIO_R |= 0x20;           // Enable clock for Port F
    while ((SYSCTL_PRGPIO_R & 0x20) == 0); // Wait until Port F is ready
    GPIO_PORTF_DIR_R |= 0x02;            // Set PF1 (Red LED) as output
    GPIO_PORTF_DEN_R |= 0x02;            // Enable digital function for PF1
    GPIO_PORTF_AFSEL_R &= ~0x02;         // Disable alternate function for PF1
    GPIO_PORTF_PCTL_R &= ~0x000000F0;    // Configure PF1 as GPIO
    GPIO_PORTF_DATA_R &= ~0x02;          // Initially turn off the red LED
}

void Timer0A_Init(uint32_t period, uint32_t duty)
{
    SYSCTL_RCGCTIMER_R |= 0x01;          // Enable clock to Timer0
    TIMER0_CTL_R = 0x00000000;           // Disable Timer0A during configuration
    TIMER0_CFG_R = 0x00000000;           // Configure for 32-bit timer mode
    TIMER0_TAMR_R = 0x0000000A;          // Configure for periodic, PWM mode
    TIMER0_TAILR_R = period - 1;         // Set the load value (period)
    TIMER0_TAMATCHR_R = duty;            // Set the compare value (duty cycle)
    TIMER0_CTL_R |= 0x00000001;          // Enable Timer0A
}

void Timer0A_SetDutyCycle(uint8_t duty)
{
    uint32_t compareValue;

    // Calculate compare value based on the global duty cycle (range 0-100%)
    compareValue = ((160 * duty) / 100) - 1;

    TIMER0_TAMATCHR_R = compareValue;
}




/*#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
volatile int i = 0;
void PortF_Init(void) {
    // Enable clock for Port F
    SYSCTL_RCGCGPIO_R |= 0x20; // Enable Port F clock
    while((SYSCTL_PRGPIO_R & 0x20) == 0); // Wait for Port F to be ready

    GPIO_PORTF_DIR_R |= 0x04; // Set PF2 as output (blue LED)
    GPIO_PORTF_DIR_R |= 0x02; // Set PF1 as output (red LED)
    GPIO_PORTF_DIR_R &= ~0x01; // Set PF0 as input (switch)
    GPIO_PORTF_DEN_R |= 0x07; // Enable digital function on PF0, PF1, and PF2
}

void Timer1_Init(void) {
    // Enable clock for Timer 1
    SYSCTL_RCGCTIMER_R |= 0x02; // Activate Timer 1
    while((SYSCTL_PRTIMER_R & 0x02) == 0); // Wait for Timer 1 to be ready

    TIMER1_CTL_R = 0; // Disable Timer 1 during configuration
    TIMER1_CFG_R = 0; // Configure as 32-bit timer
    TIMER1_TAMR_R = 0x0A; // Configure Timer 1A for PWM mode
    TIMER1_TAILR_R = 16000000 - 1; // Load value for 1 Hz (16 MHz / 1 Hz) - 1
    TIMER1_TAMATCHR_R = 1280000 - 1; // Match value for 20% duty cycle (20% of 16000000) - 1
    TIMER1_CTL_R |= 0x01; // Enable Timer 1A
}

void PWM_Init(void) {
    // Configure PWM output for Timer 1
    TIMER1_CTL_R |= 0x00000001; // Enable Timer 1A
}

int main(void) {
    PortF_Init(); // Initialize Port F for LEDs and switch
    Timer1_Init(); // Initialize Timer 1A for PWM
    PWM_Init(); // Initialize PWM mode

    while (1) {
        // Check switch state
        if ((GPIO_PORTF_DATA_R & 0x01) == 0) { // If switch (PF0) is pressed
            GPIO_PORTF_DATA_R ^= 0x04; // Toggle blue LED (PF2)
            for (i = 0; i < 200000; i++); // Simple delay
        }
    }
}


*/



/*#include <stdint.h>
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
    TIMER0_CTL_R = 0;                // Disable Timer0 during
    GPIO_PORTF_AFSEL_R |= 0x02;
    GPIO_PORTF_PCTL_R &= ~0xF0;
    GPIO_PORTF_PCTL_R |= 0x70;
    TIMER0_CFG_R = 0x00000000;       // 32-bit timer
    TIMER0_TAMR_R = 0x00000002;      // Periodic timer mode
    TIMER0_TAILR_R = 16000000 - 1;     // Load the interval value
    TIMER0_TAMATCHR_R = 8000000;
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
}*/

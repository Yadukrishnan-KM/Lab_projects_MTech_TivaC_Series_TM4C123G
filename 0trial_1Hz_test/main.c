#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

#define redled 0x02
#define blueled 0x04
#define greenled 0x08
#define off 0x00

// global variables

int timeIntervel = 509900;

int main(void)
{
    SYSCTL_RCGC2_R |= 0x00000020; // enable clock to f
    GPIO_PORTF_LOCK_R = 0X4C4F434B;
    GPIO_PORTF_CR_R = 0x1F;
    GPIO_PORTF_DEN_R = 0x1E;   // digital enable
    GPIO_PORTF_DIR_R = 0x0E; // direction register
    GPIO_PORTF_PUR_R = 0x10; // pullup

    long int i = 0;

    while(1){
        GPIO_PORTF_DATA_R = redled;             // red led
        for(i = 0; i < timeIntervel; i++);      // delay
        GPIO_PORTF_DATA_R = off;                // turning off all
        for(i = 0; i < timeIntervel; i++);      // delay
    }
}

//red led: stuck in tx while loop
//green led: stuck in rx while loop
//bue led: sd card status and working branch indication

// SPI(0) pin connections: PA2:-SCK(0) PA3:-CS(0) PA4:-MISO(0) PA5:-MOSI(0)
// SPI(2) pin connections: PB4:-SCK(2) PB5:-CS(2) PB6:-MISO(2) PB7:-MOSI(2)

// ********************************************///////////****************************************************//

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "definitions.h"

bool switchEnable = 0;
int option = 0;

// *******************************************////////////***************************************************//

void main(void){

    SysTick_Initialisation();
    GPIO_PortF_Initialisation();
    GPIO_PortF_Interrupt_Initialisation();
    GPIO_PortA_Initialisation();
    GPIO_PortB_Initialisation();
    SPI_2_Initialisation();
    UART0_Initialisation();
    UART0_Println("Hello");
    UART0_Println("Hardware Initialisation done");
    intia_Status = SD_Card_Initialisation();
    if(intia_Status == 1 && SD_Card_OCR_data()){
        SD_Read_Sector(0);
        UART0_Println("SD initialisation done");
    }
    else UART0_Println("SD initialisation failed");

    while(1){

        if(switch_1){

            SysTick_Delay_1mS(100);
            option = option + 1;
            if(option > 3) option = 0;
            switch_1 = 0;
            switch_2 = 0;
        }

        // Showing LED indication for the menu
        switch(option){
        case 1:

            GPIO_PORTF_DATA_R |= BLUE_LED;
            GPIO_PORTF_DATA_R &= ~GREEN_LED;
            GPIO_PORTF_DATA_R &= ~RED_LED;
            break;
        case 2:

            GPIO_PORTF_DATA_R |= GREEN_LED;
            GPIO_PORTF_DATA_R &= ~BLUE_LED;
            GPIO_PORTF_DATA_R &= ~RED_LED;
            break;
        case 3:

            GPIO_PORTF_DATA_R |= RED_LED;
            GPIO_PORTF_DATA_R &= ~GREEN_LED;
            GPIO_PORTF_DATA_R &= ~BLUE_LED;
            break;

        default:

            GPIO_PORTF_DATA_R &= ~RED_LED;
            GPIO_PORTF_DATA_R &= ~GREEN_LED;
            GPIO_PORTF_DATA_R &= ~BLUE_LED;
            break;
        }

        if (switch_2){

            SysTick_Delay_1mS(100);
            switchEnable = 1;
            switch_2 = 0;
            switch_1 = 0;
        }

        if(switchEnable){

            switch(option){
            case 1:

                UART0_Println("Entering into SD Sector Read section.....................................");
                SD_Read_Sector(10);
                option = 0;
                switchEnable = 0;
                switch_2 = 0;
                switch_1 = 0;
                break;

            case 2:

                UART0_Println("Entering into SD Sector Write section....................................");
                SD_Write_Block(10, 0);                   // sector number and clear flag value (0- write data, 1- clear data)
                option = 0;
                switchEnable = 0;
                switch_2 = 0;
                switch_1 = 0;
                break;

            case 3:

                UART0_Println("Entering into SD clear section...........................................");
                SD_Write_Block(10, 1);                   // sector number and clear flag value (0- write data, 1- clear data)
                option = 0;
                switchEnable = 0;
                switch_2 = 0;
                switch_1 = 0;
                break;

            default:
                break;

            }
        }

    }

}

// ********************************************///////////****************************************************//








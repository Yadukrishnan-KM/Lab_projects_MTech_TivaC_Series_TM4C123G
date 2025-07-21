#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "definitions.h"

char data_toWrite[] = {"abcdefghijklmnop"};         // Maximum array length is 512, 1 sector size


uint8_t replay;
uint8_t response;              // 8 bit response from the SD card while sendng CMD
int intia_Status = 0;          // Initialisation status, 1- Success, 2- CMD0 fail, 3- CMD8 fail, 4- CMD55 fail, 5- CMD41 fail
uint8_t ocr[4];
uint8_t ocr_register_4B[4];    // The OCR register value
uint8_t data_Received[512];    // Storing one sector of data from SD card, 512 bytes
unsigned long lba_begin_address,number_of_sectors,lba_addr,cluster_start,file_size,fat_begin_lba,sectors_per_fat,root_dir_first_cluster;

////// ********************************************////////**************************************************************//////

void SysTick_Delay_1mS(unsigned long delay){ // no of 1 mS
  unsigned long i;
  for(i=0; i<delay; i++){
      while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag
      }
  }
}

////// ********************************************////////**************************************************************//////

// Transmitting 8 bit data and receiving 8 bit data

uint8_t SPI2_Transmit(uint8_t data) {

    //UART0_Println("Inside SPI2_Transmit");
    //UART0_Println("Waiting for FIFO empty");

    while ((SSI2_SR_R & SSI_SR_TNF) == 0) UART0_Println(".");   // Wait until there's space in FIFO
    SSI2_DR_R = data;                       // Send data

    //UART0_Println("Waiting for FIFO data");

    while ((SSI2_SR_R & SSI_SR_RNE) == 0) UART0_Println(".");   // Wait until there's data to read

    //UART0_Println("Exiting SPI2_Transmit");

    return SSI2_DR_R;                       // Read and return data

}

////// ********************************************////////**************************************************************//////

uint8_t SD_Send(uint8_t command, uint32_t argument, uint8_t crc){

    UART0_Println("Inside SD sent, cmd sending");

    //uint8_t replay;
    //UART0_Println("Sending cmd");
    SPI2_Transmit(command | 0x40);          // 0x40 OR operation to ensure that first 2 bit (MSB) is 01
    //UART0_Println("Sending arg1");
    SPI2_Transmit((argument >> 24)& 0xff);  // right shift and bit wise AND operation to ensure result is lower 8 bit
    //UART0_Println("Sending arg2");
    SPI2_Transmit((argument >> 16)& 0xff);
    //UART0_Println("Sending arg3");
    SPI2_Transmit((argument >> 8)& 0xff);
    //UART0_Println("Sending arg4");
    SPI2_Transmit(argument & 0xff);
    //UART0_Println("Sending crc");
    SPI2_Transmit(crc | 0x01);              // 0x01 OR  operation is to ensure that last bit (LSB) is 1
    //UART0_Println("waiting rply, sending ff");
    replay = SPI2_Transmit(0xff);

    //UART0_Println("listening");

    while(replay == 0xff) {
        //UART0_Println("inside sd send rply while loop, sending ff");
        replay = SPI2_Transmit(0xff);
        //UART0_Println(".");
    }

    UART0_Println("Exiting SD sent, cmd send finished");

    return replay;

}

////// ********************************************////////**************************************************************//////

// It is sending the starting 75 pulses to ensure that SD card is set to SPI mode

void SD_Initial_Clock_Pulses(void) {

    volatile int ii;
    int i;
    for (ii = 0; ii < 100000; ii++);                      // Power on delay (optional, can help with SD card stability)
    for (i = 0; i < 10; i++) {                            // Send 80 clock pulses to wake up the card (idle state)
      SPI2_Transmit(0xff);                                // Dummy byte to generate clock pulses
    }

}

////// ********************************************////////**************************************************************//////

// SD Card Initialisation process, CMD0-CMD8-CMD55-CMD41 commands are executing inside

int SD_Card_Initialisation(void){

        UART0_Println("SD_card_init... Starting...");
        UART0_Println("Initial clk pulse sequence starting...");
    SD_Initial_Clock_Pulses();                                  // Calling the dummy spi send commad 10 times to ensure 80 clock pulse to take SD card into spi mode
        UART0_Println("80 clk pulses send");

        UART0_Println("Sending CMD0..........");
    if(SD_Send(CMD0, Zero_Arg, CMD0_CRC) == 1){

            UART0_Println("CMD0 Success!");
            UART0_Println("Sending CMD8..........");
        SysTick_Delay_1mS(2);                                   // Delay for settling down

        if(SD_Send(CMD8, CMD8_Arg, CMD8_CRC) == 1){

                UART0_Println("CMD8 Success!");

            int i;
            for(i=0; i<4; i++){                                 // Reading the replay of 4 bytes (argument will get as replay, 0x1aa)
                ocr[i] = SPI2_Transmit(0xff);                   // For reception of data sending dummy command
            }

                UART0_Println("Sending CMD55..........");
            SysTick_Delay_1mS(2);                               // Delay for settling down

            if(SD_Send(CMD55, Zero_Arg, Zero_CRC) <= 1){

                    UART0_Println("CMD55 Success!");
                    UART0_Println("Sending CMD41..........");
                SysTick_Delay_1mS(10);                          // Delay for settling down

                if(SD_Send(CMD41, CMD41_Arg, Zero_CRC) == 0){

                        UART0_Println("CMD41 Success!");
                    return 1;
                }
                else{                                          // CMD41 failed, then continiously sending CMD41 followed by CMD55 upto 50 times maximum to make it success

                    UART0_Println("CMD41 failed @ first attempt, trying again... Wait...");

                    int i;
                    for(i=0; i<50; i++){
                        SysTick_Delay_1mS(10);                 // Delay for settling down
                        SD_Send(CMD55, Zero_Arg, Zero_CRC);
                        SysTick_Delay_1mS(10);                 // Delay for settling down
                        response = SD_Send(CMD41, CMD41_Arg, Zero_CRC);
                        if (response == 0) break;              // CMD41 succeed then break the loop
                    }

                }

                if(response == 0){

                        UART0_Println("CMD41 Success!");
                    return 1;

                }
                else{

                        UART0_Println("CMD41 failed");
                    return 5;
                }

            }
            else {
                    UART0_Println("CMD55 failed");
                return 4;
            }

        }
        else {
                UART0_Println("CMD8 failed");
            return 3;
        }

    }
    else {
            UART0_Println("CMD0 failed");
        return 2;
    }

}

////// ********************************************////////**************************************************************//////

// Reading OCR register value and setting code length to 512 Bytes

bool SD_Card_OCR_data(){

    UART0_Println("Requisting SD OCR data and setting block length to 512B");

    UART0_Println("Sending CMD58..........");
    if(SD_Send(CMD58, Zero_Arg, Zero_CRC) == 0){            // no error ready

        UART0_Println("CMD58 Success!");
        UART0_Println("Reading OCR register...");

        int i;
        for(i=0; i<4; i++){                                 // Reading the replay of 4 bytes (Operation Conditions Register)
            ocr_register_4B[i] = SPI2_Transmit(0xff);       // For reception of data sending dummy command
        }
        UART0_Println("Reading OCR Success!");

          UART0_Println("Sending CMD16..........");
      SysTick_Delay_1mS(2);                                 // Delay for settling down
      if(SD_Send(CMD16, CMD16_Arg, Zero_CRC) == 0){         // Setting block length to 512 Bytes

          UART0_Println("CMD16 Success!");
          return 1;
      }
      else{
          UART0_Println("CMD16 failed");
          return 0;
      }

    }
    else{
        UART0_Println("CMD58 failed");
        return 0;
    }

}

////// ********************************************////////**************************************************************//////

// Verify if file system is FAT32

void SD_Read_Sector(uint32_t blockNo){

       UART0_Println("Reading first sector initialised");
       UART0_Println("Sending CMD17..........");
   if (SD_Send(CMD17, (blockNo * 512), Zero_CRC) == 0) // change back the argument to Zero_Arg
   {
          UART0_Println("CMD 17 Success!, reading first sector");
       Receive_Datablock_1B();
   }

   if(blockNo == 0){
       if((data_Received[450] == 0x0B || data_Received[450] == 0x0C) && data_Received[510] == 0x55 && data_Received[511] == 0xAA)
       {
           UART0_Println("FS is FAT32\n");
       }
       else{
           UART0_Println("\nError FAT32");
       }
       lba_begin_address=(unsigned long)data_Received[454]+(((unsigned long)data_Received[455])<<8)+(((unsigned long)data_Received[456])<<16)+(((unsigned long)data_Received[457])<<24);
       number_of_sectors=(unsigned long)data_Received[458]+(((unsigned long)data_Received[459])<<8)+(((unsigned long)data_Received[460])<<16)+(((unsigned long)data_Received[461])<<24);
   }


}

////// ********************************************////////**************************************************************//////

// Receives a block from a read of an SD card

bool Receive_Datablock_1B(void){

    unsigned char token = 0x00;
    int count = 0;

    // Wait for start of data token (0xFE)

    while ((token != 0xfe) && count < 50){    // Wait for data packet and if fails do 50 times

        token = SPI2_Transmit(0xff);
        count = count + 1;
    }

    if(token != 0xFE) {
            UART0_Println("Data tocken failed");
        return 0;               // If not valid data token (0xfe), break and return error
    }
        UART0_Println("Data tocken received, starting data reading");
    int i;
    for (i=0; i < 512; i++){
        data_Received[i] = SPI2_Transmit(0xff);
    }

    // Receiving the 2 checksum bytes, just receive and throw
    SPI2_Transmit(0xff);
    SPI2_Transmit(0xff);
       UART0_Println("Data reading success!");
    return 1;                                 // Return with success
}

////// ********************************************////////**************************************************************//////

// Function to write a block to the SD card (give sector number as input)

bool SD_Write_Block(uint32_t blockNo, bool clear_Flag) {

    int array_size = 0;
        UART0_Println("Data writing operation to one sector is initialised...");
    if (SD_Send(CMD24, (blockNo * 512), Default_CRC) == 0) {

            UART0_Println("CMD 24 Success!, writing data to sector");
            while(SPI2_Transmit(0xff) != 0xff);                   // Wait for SD card to be ready
            SPI2_Transmit(0xfe);                                  // Data token to confirm data is starting to send
            array_size = sizeof(data_toWrite);                    // Send data (512 bytes block)
            int i;
            for (i = 0; i < 512; i++){                            // Send data (512 bytes block)

                if((i < (array_size-1)) && !clear_Flag) {
                    SPI2_Transmit(data_toWrite[i]);
                    UART0_Println("Writing data");
                }
                else  {
                    SPI2_Transmit(0x00);
                        //UART0_Println("Erasing to 0x00");
                }

            }

            SysTick_Delay_1mS(10);                                 // delay for settle down
            SPI2_Transmit(0xff);                                   // Send CRC 2 bytes (0xff for no CRC in SPI mode)
            SPI2_Transmit(0xff);
            SysTick_Delay_1mS(10);                                 // delay for settle down

        if( (SPI2_Transmit(0xff) & 0x1f) == 0x05){                 // Wait for the write response
                UART0_Println("Data write to sector is Success!");
                while(SPI2_Transmit(0xff) == 0x00);
                UART0_Println("Data write finished, exiting....");
            return 1;
        }
        else{
                UART0_Println("Data write to sector is failed");
                while(SPI2_Transmit(0xff) == 0x00);
                UART0_Println("Data write failed, exiting....");
            return 0;
        }

    }
    else{
            UART0_Println("CMD 24 failed");
        return 0;
    }

}



////// ********************************************////////**************************************************************//////

// Functions for serial print using UART0

// Function to send a single character over UART0
void UART0_WriteChar(char c) {
    // Wait until the TX FIFO is not full
    while ((UART0_FR_R & UART_FR_TXFF) != 0) {}
    UART0_DR_R = c;  // Write character to the UART0 data register
}

// Function to send a string over UART0
void UART0_WriteString(const char* str) {
    while (*str) {
        UART0_WriteChar(*str);
        str++;
    }
}

// Function to send a string with a newline (simulating println)
void UART0_Println(const char* str) {
    UART0_WriteString(str);
    UART0_WriteString("\r\n");
}


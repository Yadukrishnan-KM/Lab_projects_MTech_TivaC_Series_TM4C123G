
// Define LED pins

#define RED_LED    (1U << 1) // PF1
#define BLUE_LED   (1U << 2) // PF2
#define GREEN_LED  (1U << 3) // PF3
#define SD_CS_Pin  (1U << 3) // PA3

// SD Command definitions

#define CMD0   0x40  // Reset command (GO_IDLE_STATE), R1 response
#define CMD8   0x48  // Send Interface Condition command (CHECK_PATTERN), 40 bit replay, R3 response
#define CMD55  0x77  // App command (prefix for other commands), R1 response
#define CMD41  0x69  // SD Card Initialization command (SEND_OP_COND), R1 response
#define CMD58  0x7a  // Operating Conditions Register, condition about voltage and card type, 40 bit replay, R3 response
#define CMD16  0x50  // Set block length (512 bytes), R1 response
#define CMD17  0x51  // Read single block (sector)
#define CMD24  0x58  // Write single block (WRITE_BLOCK)

// SD arguments

#define Zero_Arg  0x00         // for cmd0, cmd55, cmd58
#define CMD8_Arg  0x1aa
#define CMD41_Arg 0x40000000   // SDHC/SDXC cards: 0x40000000, SDSC cards: 0x00000000
#define CMD16_Arg 0x200        // common block length 512 bytes

// SD CRC

#define CMD8_CRC 0x87
#define CMD0_CRC 0x95
#define Zero_CRC 0x00
#define Default_CRC 0x01

//////**********************************************************/////**********************************************////

// Global variables

extern uint8_t response;          // 8 bit response from the SD card while sendng CMD
extern int intia_Status;          // Initialisation status, 1- Success, 2- CMD0 fail, 3- CMD8 fail, 4- CMD55 fail, 5- CMD41 fail
extern bool switch_1;
extern bool switch_2;
extern uint8_t ocr[4];            // The OCR register value
extern uint8_t ocr_register_4B[4];
extern unsigned long lba_begin_address,number_of_sectors,lba_addr,cluster_start,file_size,fat_begin_lba,sectors_per_fat,root_dir_first_cluster;
extern uint8_t data_Received[512];       // Storing one sector of data from SD card, 512 bytes


//////**********************************************************/////**********************************************////

// all functions declaration

// Initialisation functions

void GPIO_PortA_Initialisation(void);
void GPIO_PortB_Initialisation(void);
void GPIO_PortF_Initialisation(void);
void SPI_0_Initialisation(void);
void SPI_2_Initialisation(void);
void UART0_Initialisation(void);

// Interrupt related functions

void SysTick_Initialisation(void);
void GPIO_PortF_Interrupt_Initialisation(void);
void GPIO_PortF_Interrupt_Handler(void);

// Other functions

void SD_Card_Present(void);
void modify(void);
void SysTick_Delay_1mS(unsigned long delay);
uint8_t SPI2_Transmit(uint8_t data);
uint8_t SD_Send(uint8_t command, uint32_t argument, uint8_t crc);
int SD_Card_Initialisation(void);
void SD_Initial_Clock_Pulses(void);
bool SD_Card_OCR_data();
void SD_Read_Sector(uint32_t blockNo);
bool SD_Write_Block(uint32_t blockNo, bool clear_Flag);
bool Receive_Datablock_1B(void);
void UART0_WriteChar(char c);
void UART0_WriteString(const char* str);
void UART0_Println(const char* str);

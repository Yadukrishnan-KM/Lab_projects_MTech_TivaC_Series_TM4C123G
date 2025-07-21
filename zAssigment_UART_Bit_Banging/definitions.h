
// All the global variables

extern volatile char transmit_data;     // Character to be transmitted
extern volatile int bit_count_tx;       // Number of bits transmitted
extern volatile char receive_data;      // Character to be transmitted
extern volatile int bit_count_rx;       // Number of bits transmitted
extern volatile bool rx_ready;          // Enable flag for rx code
extern volatile bool tx_ready;          // Enable flag for tx code

// All the function declaration

void GPIO_PortA_Initialisation(void);
void GPIO_PortA_Interrupt_Initialisation(void);
void Timer_0_Initialisation(void);
void Tx_1_Byte(void);
void Rx_1_Byte(void);
void Timer0_Interrupt_Handler(void);
void GPIOA_Interrupt_Handler(void);

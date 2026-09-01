#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "uart0.h"

#define SYSCTL_RCGCUART_UART0		SYSCTL_RCGCUART_R0//define sysclock for UART0
#define SYSCTL_RCGCGPIO_PORTA		SYSCTL_RCGCGPIO_R0//define sysclock for portA
#define PA1_PA0_MASK  0x03  //port A mask for PA0-PA1 for UART0
//------------UART_Init------------
// Initialize the UART for 115,200 baud rate (assuming 50 MHz UART clock),
// 8 bit word length, no parity bits, one stop bit, FIFOs enabled
// Input: none
// Output: none
void UART0_Init(void){
  SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_UART0; // activate UART0
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_PORTA; // activate port A
	
  UART0_CTL_R = 0;                      // disable UART
  UART0_IBRD_R = 17;                    // IBRD = int(16,000,000 / (16 * 57600)) = int(17.3611)
  UART0_FBRD_R = 23;                     // FBRD = int(0.3611 * 64 + 0.5) = 23
                                        // 8 bit word length (no parity bits, one stop bit,  FIFOs enabled)
  UART0_LCRH_R = UART_LCRH_WLEN_8|UART_LCRH_FEN; // For simulator FIFO has to be enabled.
	
  UART0_CTL_R |= UART_CTL_RXE|UART_CTL_TXE|UART_CTL_UARTEN;// enable Tx, RX and UART
  GPIO_PORTA_AFSEL_R |= PA1_PA0_MASK;           // enable alt funct on PA1-0
  GPIO_PORTA_DEN_R |= PA1_PA0_MASK;             // enable digital I/O on PA1-0
                                        // configure PA1-0 as UART
  GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFFFFFF00)+0x00000011;
  GPIO_PORTA_AMSEL_R &= ~PA1_PA0_MASK;          // disable analog functionality on PA
}

//---------------------OutCRLF---------------------
// Output a CR,LF to UART to go to a new line
// Input: none
// Output: none

void UART0_OutCRLF(void){
  UART0_OutChar(CR);
  UART0_OutChar(LF);
}

//------------UART_InChar------------
// Wait for new serial port input
// Input: none
// Output: ASCII code for key typed
uint8_t UART0_InChar(void){
  while((UART0_FR_R&UART_FR_RXFE) != 0); // wait until the receiving FIFO is not empty
  return((uint8_t)(UART0_DR_R&0xFF));
}
//------------UART_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART0_OutChar(uint8_t data){
  while((UART0_FR_R&UART_FR_TXFF) != 0);
  UART0_DR_R = data;
}


//------------UART_OutString------------
// Output String (NULL termination)
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void UART0_OutString(uint8_t *pt){
  while(*pt){
    UART0_OutChar(*pt);
    pt++;
  }
}

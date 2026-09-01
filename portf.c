#include <stdint.h>
#include "tm4c123gh6pm.h"



//This is the C file to Init port F for leds and SW1
//SW1 PF4
//LEDS PF3-PF1
#define All_LED_MASK 0x0E
#define SW1_MASK        0x10
#define LED_SW1_MASK    0x1E

#define Trigger_Echo    0x30
#define TRIGGER_VALUE 	0x20   // trigger at bit 5
#define ECHO_VALUE 			0x10   // trigger at bit 4


void PortFInit(void){
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;           // 1) activate clock for Port B
	while ((SYSCTL_RCGC2_R&SYSCTL_RCGC2_GPIOF)!=SYSCTL_RCGC2_GPIOF){};
		
	GPIO_PORTF_DIR_R |= All_LED_MASK; // make PF3 - PF1 output
	GPIO_PORTF_DIR_R &= ~SW1_MASK;//make PF4 input
	GPIO_PORTF_PUR_R |= SW1_MASK; //enable pull up resistor for sw1 negative logic 
  GPIO_PORTF_AFSEL_R &= ~LED_SW1_MASK;// disable alt funct on PF4 - PF1
  GPIO_PORTF_DEN_R |= LED_SW1_MASK; // enable digital I/O on PF4-PF1                           
  GPIO_PORTF_PCTL_R &= ~0x000FFFF0; // configure PF4 - PF1 as GPIO
  GPIO_PORTF_AMSEL_R |= LED_SW1_MASK;   // disable analog functionality on PF4 - PF1
  GPIO_PORTF_IS_R &= ~SW1_MASK;         // PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~SW1_MASK;         // PB4 is NOT BOTH EDGES
  GPIO_PORTF_IEV_R &= ~SW1_MASK;        // PB4 FALLING EDGE
  GPIO_PORTF_ICR_R |= SW1_MASK;          // clear flag PF4
  GPIO_PORTF_IM_R |= SW1_MASK;          // arm interrupt on PF4
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF1FFFFF)|0x00E00000; // (g) priority 7
  NVIC_EN0_R = 0x40000000;          // (h) enable Port F edge interrupt
}

void PortB_Init(void){ 
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;           // 1) activate clock for Port B
	while ((SYSCTL_RCGC2_R&SYSCTL_RCGC2_GPIOB)!=SYSCTL_RCGC2_GPIOB){}; // wait for clock to start
                                    
  GPIO_PORTB_PCTL_R &= ~0x00FF0000; // 3) regular GPIO pin PB4 and PB5
  GPIO_PORTB_AMSEL_R &= (uint32_t)~Trigger_Echo;      
  GPIO_PORTB_DIR_R &= ~ECHO_VALUE;        // 5) PB4:echo pin, input
  GPIO_PORTB_DIR_R |= TRIGGER_VALUE;         // 5) PB5:trigger pin, output
  GPIO_PORTB_AFSEL_R &= ~Trigger_Echo;      // 6) regular port function
  GPIO_PORTB_DEN_R |= Trigger_Echo;         // 7) enable digital port
  GPIO_PORTB_IS_R &= ~ECHO_VALUE;         // PB4 is edge-sensitive
  GPIO_PORTB_IBE_R |= ECHO_VALUE;         // PB4 is both edges
  GPIO_PORTB_IEV_R &= ~ECHO_VALUE;        // PB4 both edge event
  GPIO_PORTB_ICR_R = ECHO_VALUE;          // clear flag 4
  GPIO_PORTB_IM_R |= ECHO_VALUE;          // arm interrupt on PB4
  NVIC_PRI0_R = (NVIC_PRI0_R&0xFFFF1FFF)|0x00006000; // (g) priority 3
  NVIC_EN0_R = 0x00000002;          // (h) enable Port B edge interrupt
}

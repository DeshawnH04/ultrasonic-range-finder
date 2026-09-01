#include <stdint.h>
#include "Timer1A.h"
#include "tm4c123gh6pm.h"

#define ONE_MICRO_SECOND          16 

void Timer1A_Init(void){//init the time
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000004;    // 2) configure for 16-bit mode
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic down-count mode
          
}

void TimerStop(void){
	 TIMER1_CTL_R = 0x00000000;//disable the timer to stop it
}

void EchoStart(unsigned long period){
	TIMER1_TAILR_R = period - 1;
	TIMER1_TAPR_R = 15;//set presacle to 15 to get to 640000
	TIMER1_CTL_R = 0x00000001;//start the timer
}

void TriggerStart(unsigned long period){
	TIMER1_TAILR_R = (period * ONE_MICRO_SECOND) - 1;
	TIMER1_TAPR_R = 0;   //set presacle to zero
	TIMER1_CTL_R = 0x00000001;//start the timer
	while(TIMER1_TAR_R){}//wait until value counts down to zero
	TIMER1_CTL_R = 0x00000000;// stop timer
}

uint32_t Pulse_width(void){
	return (uint32_t) (TIMER1_TAILR_R - TIMER1_TAR_R) * (TIMER1_TAPR_R + 1);//TIMER1_TAR_R gives the current value of the counter counting down
}
// Ultrasonic_Sensor.c
// This program runs on TM4C123.
// This is an example program to show how to interface HC-SR04 Ultrasonic sensor.
// PB6 connects to echo pin to generate edge-triggered interrupt.
// PB7 connects to Ultrasonic sensor trigger pin.
// SysTick timer is used to generate the required timing for trigger pin and measure echo pulse width.
// Global variable "distance" is used to hold the distance in cemtimeter for the obstacles
// in front of the sensor. 
// By Dr. Min He
// December 10th, 2018

// Ultrasonic_Sensor.c
//description this program runs the ultrasonic sensor as well as uart to display the current
//distance being read from the sensor on the serial port communcation
//along with three leds that correspond to certain distances on the portF

/* Group 6 
//hardware connection PB4 echo pin
//hardware connection PB5 trigger pin
Brandon Jamjampour
Daniel Banuelos
Anthony Nuth
Anastacia Estrella
*/

#include <stdint.h>
#include <stdio.h>
#include "Timer1A.h"
#include "PortF.h"
#include "uart0.h"
#include "tm4c123gh6pm.h"
#include "SysTick.h"

#define LED              (*((volatile unsigned long *)0x40025038))//specfic bit address for PF3-PF1 
#define Max             40000   //define to create a 40ms echo count length with prescale 16 to make 640000/16 = 40000
#define Trigger_Echo    0x30
#define TRIGGER_PIN 		(*((volatile unsigned long *)0x40005080))  // PB5 is the trigger pin	
#define TRIGGER_VALUE 	0x20   // trigger at bit 5
#define ECHO_PIN 				(*((volatile unsigned long *)0x40005040))  // PB4 is the echo pin	
#define ECHO_VALUE 			0x10   // trigger at bit 4
#define MC_LEN 					0.0625 // length of one machine cycle in microsecond for 16MHz clock
#define SOUND_SPEED 		0.0343 // centimeter per micro-second
#define MAX_STR_LEN 50

#define SW1_MASK  0x10//SW1 mask
/*
	Defines the colors for the leds
*/
#define RED       0x02
#define GREEN     0x08
#define BLUE      0x04
#define OFF       0x00
#define TwoHundred50_MS 4000000  // reload value to generate 250ms for system clock 16MHz.

/*
	protoype functions for interrupt handlers
*/
extern void EnableInterrupts(void);
extern void GPIOPortB_Handler(void);
extern void GPIOPortF_Handler(void);

/*
	Define done for when distance is calculated
	Define distance to be zero until we calculate it
	Define flag which Is when the button is clicked to be zero 
*/
static volatile uint8_t done=0;
static volatile uint32_t distance = 0;
static volatile uint8_t flag = 0;

int main(void){
	PortB_Init();
	Timer1A_Init();
	PortFInit();
  UART0_Init();
  EnableInterrupts();
  while(1){
		uint8_t str[MAX_STR_LEN];  //defines max string length for the uart message

		flag = 0;
		while(!flag);//checks if the button is pressed
		LED = OFF;
		done = 0;
		distance = 0;
		TRIGGER_PIN &= ~TRIGGER_VALUE; // send low to trigger
		TriggerStart(2);//creates 2uS reload value with prescale set to zero
		TRIGGER_PIN |= TRIGGER_VALUE; // send high to trigger
		TriggerStart(10);//creates 10uS delay 
		TRIGGER_PIN &= ~TRIGGER_VALUE; // send low to trigger
		
    while(!done);//wait here until a distance is calculated
		StopSystick();//stop the systick timer if it was running
		if(distance > 100){
			LED = OFF;
		}
		if(distance <= 100 && distance > 70){
			LED = BLUE;
		}
		if(distance <= 70 && distance >= 10){
			LED = GREEN;
		}
		if(distance < 10){
			SysTick_Init(TwoHundred50_MS);//if distance is less than 10 start the timer and load the value
		}
		//take care of leds and uart display
		if(distance >= 0 && distance < 100){//if the distance is in range display it in cm
			sprintf((char *)str, "The current distance is %d cm\n\r", distance);
			UART0_OutString(str);  // Send the distance over UART
		}else{//if its not in range so not between 0 - 100 then display out of range
			sprintf((char *)str, "The current distance is OUT OF RANGE \n\r");
			UART0_OutString(str);  // Send the distance over UART
		}
  }
}

void GPIOPortB_Handler(void){
	
	if (ECHO_PIN==ECHO_VALUE){  // echo pin rising edge is detected, start timing
		EchoStart(Max);//start 40000
	}
	else { // echo pin falling edge is detected, end timing and calculate distance.
    // The following code is based on the fact that the HCSR04 ultrasonic sensor 
    // echo pin will always go low after a trigger with bouncing back
    // or after a timeout. The maximum distance can be detected is 400cm.
		// The speed of sound is approximately 340 meters per second, 
		// or  .0343 c/ S.
    // Distance = (echo pulse width * 0.0343)/2; = ((# of mc)*MC_LEN*SOUND_SPEED)/2
		TimerStop();//stops the timer when receive falling edge trigger
		distance = (uint32_t)(Pulse_width()*MC_LEN*SOUND_SPEED)/2;		
		done = 1;
	}
  GPIO_PORTB_ICR_R = ECHO_VALUE;      // acknowledge flag for PB4
}
void GPIOPortF_Handler(void){
		for (int i = 0; i < 16000000; i++) {}//button debounce
		flag = 1;//set flag to one button pressed
		GPIO_PORTF_ICR_R = SW1_MASK;      // acknowledge flag for PF4 SW1

}
void SysTick_Handler(void){//systick interrupt
  LED ^= RED;       // toggle red LED at a rate of 2hz
}
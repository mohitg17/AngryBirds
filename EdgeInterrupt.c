// EdgeInterrupt.c
// Runs on LM4F120 or TM4C123
// Request an interrupt on the falling edge of PF4 (when the user
// button is pressed) and increment a counter in the interrupt.  Note
// that button bouncing is not addressed.
// Daniel Valvano
// May 3, 2015

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers"
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015
   Volume 1, Program 9.4
   
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
   Volume 2, Program 5.6, Section 5.5

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// user button connected to PF4 (increment counter on falling edge)

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"



void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void (*PeriodicTask2)(void);
void (*PeriodicTask3)(void);

// global variable visible in Watch window of debugger
// increments at least once per button press
volatile uint32_t FallingEdges = 0;
void EdgeCounter_Init(void(*task)(void)){                          
  SYSCTL_RCGCGPIO_R |= 0x00000020; // (a) activate clock for port F
	PeriodicTask2 = task;
  FallingEdges = 0;             // (b) initialize counter
  GPIO_PORTF_DIR_R &= ~0x10;    // (c) make PF4 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~0x10;  //     disable alt funct on PF4
  GPIO_PORTF_DEN_R |= 0x10;     //     enable digital I/O on PF4   
  GPIO_PORTF_PCTL_R &= ~0x000F0000; // configure PF4 as GPIO
  GPIO_PORTF_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTF_PUR_R |= 0x10;     //     enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x10;     // (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x10;    //     PF4 is not both edges
  GPIO_PORTF_IEV_R &= ~0x10;    //     PF4 falling edge event
  GPIO_PORTF_ICR_R = 0x10;      // (e) clear flag4
  GPIO_PORTF_IM_R |= 0x10;      // (f) arm interrupt on PF4 *** No IME bit as mentioned in Book ***
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
  EnableInterrupts();           // (i) Clears the I bit
}
void GPIOPortF_Handler(void){
  GPIO_PORTF_ICR_R = 0x10;      // acknowledge flag4
  FallingEdges = FallingEdges + 1;
	(*PeriodicTask2)();
}

void EdgeCounter_Init2(void(*task)(void)){                          
  SYSCTL_RCGCGPIO_R |= 0x00000010; // (a) activate clock for port E
	PeriodicTask3 = task;
  FallingEdges = 0;             // (b) initialize counter
  GPIO_PORTE_DIR_R &= ~0x1;    // (c) make PF4 in (built-in button)
  GPIO_PORTE_AFSEL_R &= ~0x1;  //     disable alt funct on PE0
  GPIO_PORTE_DEN_R |= 0x1;     //     enable digital I/O on PE0 
  GPIO_PORTE_PCTL_R &= ~0x0000000F; // configure PE0 as GPIO
  GPIO_PORTE_AMSEL_R = 0;       //     disable analog functionality on PF
//  GPIO_PORTE_PUR_R |= 0x10;     //     enable weak pull-up on PF4
  GPIO_PORTE_IS_R &= ~0x1;     // (d) PF4 is edge-sensitive
  GPIO_PORTE_IBE_R &= ~0x1;    //     PF4 is not both edges
  GPIO_PORTE_IEV_R &= ~0x1;    //     PF4 falling edge event
  GPIO_PORTE_ICR_R = 0x1;      // (e) clear flag4
  GPIO_PORTE_IM_R |= 0x1;      // (f) arm interrupt on PF4 *** No IME bit as mentioned in Book ***
  NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF0F)|0x000000C0; // (g) priority 6
  NVIC_EN0_R = 0x40000010;      // (h) enable interrupt 4 in NVIC
  EnableInterrupts();           // (i) Clears the I bit
}

void GPIOPortE_Handler(void){
  GPIO_PORTE_ICR_R = 0x1;      // acknowledge flag1
  FallingEdges = FallingEdges + 1;
	(*PeriodicTask3)();
}


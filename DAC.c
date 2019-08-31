// put implementations for functions, explain how it works
// put your names here, date
#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>


// **************DAC_Init*********************
// Initialize 4-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void){
		volatile int32_t delay;
		SYSCTL_RCGCGPIO_R |= 0x12;		//turn on PortB and PortE clocks
		delay = 42;
		//PortB Init (PB0-6 are outputs)
		GPIO_PORTB_DEN_R |= 0x3F;			//set PortB to output and PortE to input
		GPIO_PORTB_DIR_R |= 0x3F;
		//PortE Init (PE0-3 are inputs)
		GPIO_PORTE_DEN_R |= 0xF;
		GPIO_PORTE_DIR_R &= ~0xF;
}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Input=n is converted to n*3.3V/15
// Output: none
void DAC_Out(uint8_t data){
	GPIO_PORTB_DATA_R = data;				// set PortB to switch value
}

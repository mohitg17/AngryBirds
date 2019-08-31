// put implementations for functions, explain how it works
// put your names here, date
#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>

// ADC initialization function 
// Input: none
// Output: none
// measures from PD2, analog channel 5
void ADC_Init(void){ 
	volatile int delay;
		
	SYSCTL_RCGCGPIO_R |= 0x8;
	while((SYSCTL_PRGPIO_R & 0x8) == 0) {}
	GPIO_PORTD_DIR_R &= ~0x4;
	GPIO_PORTD_AFSEL_R |= 0x4;
	GPIO_PORTD_DEN_R &= ~0x4;
	GPIO_PORTD_AMSEL_R |= 0x4;
	GPIO_PORTD_DIR_R &= ~0x2;
	GPIO_PORTD_AFSEL_R |= 0x2;
	GPIO_PORTD_DEN_R &= ~0x2;
	GPIO_PORTD_AMSEL_R |= 0x2;
	
	SYSCTL_RCGCADC_R |= 0x01;
	delay = SYSCTL_RCGCADC_R;
	delay = SYSCTL_RCGCADC_R;
	delay = SYSCTL_RCGCADC_R;
	delay = SYSCTL_RCGCADC_R;
	ADC0_PC_R = 0x01;
  ADC0_SSPRI_R = 0x3210;          // 9) Sequencer 3 is lowest priority
  ADC0_ACTSS_R &= ~0x0004;        // 10) disable sample sequencer 2
  ADC0_EMUX_R &= ~0x0F00;         // 11) seq2 is software trigger
  ADC0_SSMUX2_R = 0x0056;         // 12) set channels for SS2
  ADC0_SSCTL2_R = 0x0060;         // 13) no TS0 D0 IE0 END0 TS1 D1, yes IE1 END1
  ADC0_IM_R &= ~0x0004;           // 14) disable SS2 interrupts
  ADC0_ACTSS_R |= 0x0004;         // 15) enable sample sequencer 2
}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5
void ADC_In(uint32_t input[2]){  
  ADC0_PSSI_R = 0x0004;            // 1) initiate SS2
  while((ADC0_RIS_R&0x04)==0){};   // 2) wait for conversion done
  input[1] = ADC0_SSFIFO2_R&0xFFF;  // 3A) read first result
  input[0] = ADC0_SSFIFO2_R&0xFFF;  // 3B) read second result
  ADC0_ISC_R = 0x0004;             // 4) acknowledge completion
}

//------------ADC_ConvertMag------------
uint32_t ADC_ConvertXMag(int32_t data) {
	uint32_t magnitude;
	if(data < 1000) {
		magnitude = 3;
	}
		else {
			magnitude = data/250;
		}
		return magnitude;
	}

uint32_t ADC_ConvertYMag(int32_t data) {
	uint32_t magnitude;
	if(data < 1000) {
		magnitude = 15;
	}
		else {
			magnitude = data/50;
		}
		return magnitude;
	}

//------------ADC_ConvertAngle------------	
uint32_t ADC_ConvertAngle(int32_t data) {
	uint32_t angle;
	if(data < 450) {
		angle = 10;
	} else {
		angle = data/45;
	}
	uint8_t difference = angle%10;
	angle -= difference;
	angle /= 10;
	angle--;
	return angle;
}
		



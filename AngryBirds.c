// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 11/20/2018 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2018

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2018

 Copyright 2018 by Jonathan W. Valvano, valvano@mail.utexas.edu
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
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "Random.h"
#include "PLL.h"
#include "ADC.h"
#include "Images.h"
#include "Sound.h"
#include "Timer0.h"
#include "Timer1.h"
#include "DAC.h"
#include "EdgeInterrupt.h"
#include <math.h>

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay10ms(uint32_t count); // time delay in 0.1 seconds
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void (*PeriodicTask)(void);   // user function

int32_t t;
uint8_t flg;
uint8_t pig1;
uint8_t pig2;
uint8_t pig3;
uint8_t pig4;
uint8_t pig5;
uint8_t birds;
uint8_t block1flag;
uint8_t block2flag; 
uint8_t block3flag;
uint8_t block4flag;
uint8_t block5flag;
uint8_t block6flag;
uint8_t x;
uint32_t sine[8] = {177, 350, 512, 658, 784, 886, 962, 1008};
uint32_t data[2];
char score[] = {'S', 'c', 'o', 'r', 'e', 0};
char zero[] = {'0', '0', '0', '0', 0};
uint32_t currentScore;

struct bird {
	int32_t xpos;
	int32_t ypos;
	int32_t width;
	int32_t height;
	int32_t xVel;
	int32_t yVel;
};
typedef struct bird bird;

struct blockA {
	int32_t xpos;
	int32_t ypos;
	int32_t width;
	int32_t height;
};
typedef struct blockA blockOne;

struct blockB {
	int32_t xpos;
	int32_t ypos;
	int32_t width;
	int32_t height;
};
typedef struct blockB blockTwo;

struct pig {
	int32_t xpos;
	int32_t ypos;
	int32_t width;
	int32_t height;
};
typedef struct pig pig;

void setVelocity(bird *red) {
	ADC_In(data);
	int32_t angle = ADC_ConvertAngle(data[0]);
	int32_t mag = data[1];
	red->xVel = (ADC_ConvertXMag(mag));
	red->yVel = (ADC_ConvertYMag(mag)*sine[angle])>>10;
}
	
void setPosition(bird *red) {
	red->xpos = -(1)*x*x+(red->xVel * t) + 5;
	red->ypos = (1)*t*t-(red->yVel*t)/5 + 90;
}

void drawBird(bird *redBird) {
	ST7735_DrawBitmap(redBird->xpos, redBird->ypos, red, redBird->width, redBird->height);
}

void drawBird1(bird *redBird) {
	ST7735_DrawBitmap(redBird->xpos, redBird->ypos, black, redBird->width, redBird->height);
}

void eraseBird(bird *redBird) {
	ST7735_DrawBitmap(redBird->xpos, redBird->ypos, blackbird, redBird->width, redBird->height);
}


void checkCollisions (bird *red){
	if (red->xpos >= 100 && red->xpos <=145)	{
		if (red->ypos >= 82 && red->ypos <=130)		{
//-------------------------------------------------------
			if (red->xpos >= 100 && red->xpos <=110)	{
				if (red->ypos >=87 && red->ypos <=127)	{
					if(block1flag > 0) {
					ST7735_DrawBitmap(105,127,black1, 5,40);
					x+=1;
						block1flag--;
						currentScore += 200;
						}
					}					
			}
//-------------------------------------------------------			
			if (red->xpos >= 132 && red->xpos <=150)	{
				if (red->ypos >=87 && red->ypos <=127)		{
					if(block2flag > 0) {
					ST7735_DrawBitmap(140,127,black1, 5,40); //block2
						block2flag--;
						currentScore += 200;
					}
				}	
			}
//-------------------------------------------------------			
			if (red->xpos >= 101 && red->xpos <=125)	{
				if (red->ypos >=110 && red->ypos <=130)		{
					ST7735_DrawBitmap(118,127,blackpiggy, 15,15); //piggy
					if(pig1>0) {
						pig1--;
						currentScore += 5000;
						}
					}	
				}
//-------------------------------------------------------
			if (red->xpos >= 90 && red->xpos <=146)	{
				if (red->ypos >=75 && red->ypos <=97)		{
					if(block3flag > 0) {
					ST7735_DrawBitmap(105,87,black2,40,5); //block2 top
						block3flag--;
						currentScore += 200;
						}
					}	
				}
//-------------------------------------------------------			
			}	
		}
	}

void checkCollisions2(bird *red) {
	checkCollisions(red);
		if (red->xpos >= 101 && red->xpos <=145)	{
		if (red->ypos >= 32 && red->ypos <=127)		{
//-------------------------------------------------------
			if (red->xpos >= 101 && red->xpos <=110)	{
				if (red->ypos >=37 && red->ypos <=87)	{
					if(block4flag > 0) {
					ST7735_DrawBitmap(105,87,black1, 5,40);
					x+=1;
						block4flag--;
						currentScore += 200;
						}
					}					
			}
//-------------------------------------------------------			
			if (red->xpos >= 132 && red->xpos <=150)	{
				if (red->ypos >=57 && red->ypos <=97)		{
					if(block5flag > 0) {
					ST7735_DrawBitmap(140,82,black1, 5,40); //block2
						block5flag--;
						currentScore += 200;
					}
				}	
			}
//-------------------------------------------------------			
			if (red->xpos >= 101 && red->xpos <=125)	{
				if (red->ypos >=60 && red->ypos <=85)		{
					ST7735_DrawBitmap(118,82,blackpiggy, 15,15); //piggy
					if(pig2>0) {
						pig2--;
						currentScore += 5000;
						}
					}	
				}
//-------------------------------------------------------
			if (red->xpos >= 90 && red->xpos <=146)	{
				if (red->ypos >=25 && red->ypos <=47)		{
					if(block6flag > 0) {
					ST7735_DrawBitmap(105,37,black2,40,5); //block2 top
						block6flag--;
						currentScore += 200;
						}
					}	
				}
//-------------------------------------------------------
			}
		}
}

void checkCollisions3(bird *red) {
		if (red->xpos >= 60 && red->xpos <=159)	{
		if (red->ypos >= 50 && red->ypos <=127)		{
//-------------------------------------------------------
			if (red->xpos >= 83 && red->xpos <=100)	{
				if (red->ypos >=85 && red->ypos <=127)	{
					if(block1flag > 0) {
					ST7735_DrawBitmap(95,127,black1, 5,40); //bottom left block1
					x+=1;
						block1flag--;
						currentScore += 200;
						}
					}					
			}
//-------------------------------------------------------			
			if (red->xpos >= 104 && red->xpos <=135)	{
				if (red->ypos >=85 && red->ypos <=127)		{
					if(block2flag > 0) {
					ST7735_DrawBitmap(130,127,black1, 5,40); //bottom right block1
						x+=1;
						block2flag--;
						currentScore += 200;
					}
				}	
			}
//-------------------------------------------------------			
			if (red->xpos >= 98 && red->xpos <=125)	{
				if (red->ypos >=100 && red->ypos <=127)		{
					ST7735_DrawBitmap(108,127,blackpiggy, 15,15); //piggy middle ground
					if(pig1>0) {
						pig1--;
						currentScore += 5000;
						}
					}	
				}
//-------------------------------------------------------
			if (red->xpos >= 118  && red->xpos <=155)	{
				if (red->ypos >=100 && red->ypos <=127)		{
					ST7735_DrawBitmap(140,127,blackpiggy,15,15); //piggy right ground
						if (pig2>0)
						{
							pig2--;
							currentScore += 5000;
						}
					}	
				}
//-------------------------------------------------------
			if (red->xpos >= 80 && red->xpos <=135)	{
				if (red->ypos >=80 && red->ypos <=97)		{
					if(block3flag > 0) {
					ST7735_DrawBitmap(95,87,black2, 40,5); //block2 bottom middle
						block3flag--;
						currentScore += 200;
					}
				}	
			}
//-------------------------------------------------------	
		if (red->xpos >= 65 && red->xpos <=120)	{
				if (red->ypos >=65 && red->ypos <=85)		{
					if(block4flag > 0) {
					ST7735_DrawBitmap(75,82,black2, 40,5); //block2 middle top
						block4flag--;
						currentScore += 200;
					}
				}	
			}
//-------------------------------------------------------	
		if (red->xpos >= 101 && red->xpos <=125)	{
				if (red->ypos >=42 && red->ypos <=82)		{
					if(block5flag > 0) {
					ST7735_DrawBitmap(115,82,black1, 5,40); //block2 vertical top
						block5flag--;
						currentScore += 200;
					}
				}	
			}
//-------------------------------------------------------	
			if (red->xpos >= 85  && red->xpos <=115)	{
				if (red->ypos >=62 && red->ypos <=92)		{
					ST7735_DrawBitmap(95,77,blackpiggy,15,15); //piggy left middle
						if (pig3>0)
						{
							pig3--;
							currentScore += 5000;
						}
					}	
				}
//-------------------------------------------------------				
				if (red->xpos >= 112  && red->xpos <=137)	{
				if (red->ypos >=67 && red->ypos <=97)		{
					ST7735_DrawBitmap(122,82,blackpiggy,15,15); //piggy right middle
						if (pig4>0)
						{
							pig4--;
							currentScore += 5000;
						}
					}	
				}
//-------------------------------------------------------	
if (red->xpos >= 90  && red->xpos <=125)	{
				if (red->ypos >=22 && red->ypos <=52)		{
					ST7735_DrawBitmap(100,37,blackpiggy,15,15); //piggy top
						if (pig5>0)
						{
							pig5--;
							currentScore += 5000;
						}
					}	
				}
//-------------------------------------------------------		
				if (red->xpos >= 69 && red->xpos <=125)	{
				if (red->ypos >= 25 && red->ypos <=52)		{
					if(block6flag > 0) {
					ST7735_DrawBitmap(80,42,black2, 40,5); //block2 horizontal top
						block6flag--;
						currentScore += 200;
					}
				}	
			}
//-------------------------------------------------------		
		}
		}
}

void Level1(void) {
while(1) {
	currentScore = 0;
	pig1 = 1;
	birds = 3;
	block1flag = 1;
	block2flag = 1;
	block3flag = 1;
	ST7735_SetCursor(0,0);
	ST7735_OutString(&score[0]);
	ST7735_SetCursor(0, 1);
	ST7735_OutUDec(currentScore);
	ST7735_DrawBitmap(0,127,catapult, 17,50);
	ST7735_DrawBitmap(105,127,block1, 5,40);
	ST7735_DrawBitmap(118,127,green, 15,15);
	ST7735_DrawBitmap(140,127,block1, 5,40);
  ST7735_DrawBitmap(105,87,block2, 40,5);
	ST7735_DrawBitmap(5,90,red, 9,9);
	bird red1;
	red1.xpos = 5;
	red1.ypos = 90;
	red1.width = 9;
	red1.height = 9;
	ST7735_DrawBitmap(25,127,red, 9,9);
	bird red2;
	red2.xpos = 25;
	red2.ypos = 127;
	red2.width = 9;
	red2.height = 9;
	ST7735_DrawBitmap(40,127,red, 9,9);
	bird red3;
	red3.xpos = 40;
	red3.ypos = 127;
	red3.width = 9;
	red3.height = 9;
			while(flg != 1) {}
				flg = 0;
				setVelocity(&red1);
				t = 0;
					while(red1.xpos < 159 && red1.ypos > 0) {
						eraseBird(&red1);
						setPosition(&red1);
						drawBird(&red1);
						checkCollisions(&red1);
						}
						ST7735_SetCursor(0, 1);
						ST7735_OutUDec(currentScore);
						if(pig1 == 0) {
						break;
					}
							DisableInterrupts();
							x = 0;
							ST7735_DrawBitmap(5,90,red, 9,9);
							red1.xpos = 5;
							red1.ypos = 90;
							ST7735_DrawBitmap(25,127,red, 9,9);
							red2.xpos = 25;
							red2.ypos = 127;
							eraseBird(&red3);
							EnableInterrupts();
							flg = 0;
				while(flg != 1) {}
					flg = 0;
					setVelocity(&red1);
					t = 0;
						while(red1.xpos < 159 && red1.ypos > 0) {
							eraseBird(&red1);
							setPosition(&red1);
							drawBird(&red1);
							checkCollisions(&red1);
							} 
							ST7735_SetCursor(0, 1);
							ST7735_OutUDec(currentScore);
							if(pig1 == 0) {
							break;
						}
							DisableInterrupts();
							x = 0;
							ST7735_DrawBitmap(5,90,red,9,9);
							red1.xpos = 5;
							red1.ypos = 90;
							eraseBird(&red2);
							EnableInterrupts();
							flg = 0;
				while(flg != 1) {}
					flg = 0;
					setVelocity(&red1);
					t = 0;
					while(red1.xpos < 159 && red1.ypos > 0) {
						eraseBird(&red1);
						setPosition(&red1);
						drawBird(&red1);
						checkCollisions(&red1);					
						}
						ST7735_SetCursor(0, 1);
						ST7735_OutUDec(currentScore);	
						if(pig1 == 0) {
						break;
						}
					flg = 0;
					}
			}

void Level2() {
while(1) {
	currentScore = 0;
	ST7735_SetCursor(0,0);
	ST7735_OutString(&score[0]);
	ST7735_SetCursor(0, 1);
	ST7735_OutString(&zero[0]);
	pig1 = 1;
	pig2 = 1;
	birds = 3;
	block1flag = 1;
	block2flag = 1;
	block3flag = 1;
	block4flag = 1;
	block5flag = 1;
	block6flag = 1;
	ST7735_DrawBitmap(0,127,catapult, 17,50);
	ST7735_DrawBitmap(105,127,block1, 5,40);
	ST7735_DrawBitmap(105, 87, block1, 5,40);
	ST7735_DrawBitmap(118,127,green, 15,15);
	ST7735_DrawBitmap(118, 82, green, 15,15);
	ST7735_DrawBitmap(140,127,block1, 5,40);
	ST7735_DrawBitmap(140, 87, block1, 5,40);
  ST7735_DrawBitmap(105,87,block2, 40,5);
	ST7735_DrawBitmap(105,47, block2, 40, 5);
	ST7735_DrawBitmap(5,90,red, 9,9);
	bird red1;
	red1.xpos = 5;
	red1.ypos = 90;
	red1.width = 9;
	red1.height = 9;
	ST7735_DrawBitmap(25,127,red, 9,9);
	bird red2;
	red2.xpos = 25;
	red2.ypos = 127;
	red2.width = 9;
	red2.height = 9;
	ST7735_DrawBitmap(40,127,red, 9,9);
	bird red3;
	red3.xpos = 40;
	red3.ypos = 127;
	red3.width = 9;
	red3.height = 9;
			flg = 0;
			while(flg != 1) {}
				flg = 0;
				setVelocity(&red1);
				t = 0;
					while(red1.xpos < 159 && red1.ypos > 0) {
						eraseBird(&red1);
						setPosition(&red1);
						drawBird(&red1);
						checkCollisions2(&red1);					
						}
						ST7735_SetCursor(0, 1);
						ST7735_OutUDec(currentScore);	
						if(pig1 == 0 && pig2 == 0) {
						break;
					}
							DisableInterrupts();
							x = 0;
							ST7735_DrawBitmap(5,90,red, 9,9);
							red1.xpos = 5;
							red1.ypos = 90;
							ST7735_DrawBitmap(25,127,red, 9,9);
							red2.xpos = 25;
							red2.ypos = 127;
							eraseBird(&red3);
							EnableInterrupts();
							flg = 0;
				while(flg != 1) {}
					flg = 0;
					setVelocity(&red1);
					t = 0;
						while(red1.xpos < 159 && red1.ypos > 0) {
							eraseBird(&red1);
							setPosition(&red1);
							drawBird(&red1);
							checkCollisions2(&red1);						
							} 
							ST7735_SetCursor(0, 1);
							ST7735_OutUDec(currentScore);
							if(pig1 == 0 && pig2 == 0) {
							break;
						}
							DisableInterrupts();
							x = 0;
							ST7735_DrawBitmap(5,90,red,9,9);
							red1.xpos = 5;
							red1.ypos = 90;
							eraseBird(&red2);
							EnableInterrupts();
							flg = 0;
				while(flg != 1) {}
					flg = 0;
					setVelocity(&red1);
					t = 0;
					while(red1.xpos < 159 && red1.ypos > 0) {
						eraseBird(&red1);
						setPosition(&red1);
						drawBird(&red1);
						checkCollisions2(&red1);
						}
						ST7735_SetCursor(0, 1);
						ST7735_OutUDec(currentScore);	
						if(pig1 == 0 && pig2 == 0) {
						break;
						}
					flg = 0;
	}
}

void Level3() {
while(1) {
	currentScore = 0;
	ST7735_SetCursor(0,0);
	ST7735_OutString(&score[0]);
	ST7735_SetCursor(0, 1);
	ST7735_OutString(&zero[0]);
	pig1 = 1;
	pig2 = 1;
	pig3 = 1;
	pig4 = 1;
	pig5 = 1;
	birds = 3;
	block1flag = 1;
	block2flag = 1;
	block3flag = 1;
	block4flag = 1;
	block5flag = 1;
	block6flag = 1;
	ST7735_DrawBitmap(0,127,catapult, 17,50);
	ST7735_DrawBitmap(95,127,block1, 5,40);
	ST7735_DrawBitmap(95,87,block2, 40,5);
	ST7735_DrawBitmap(108,127,green, 15,15);
	ST7735_DrawBitmap(130,127,block1, 5,40);
	ST7735_DrawBitmap(140, 127, green, 15,15);
	ST7735_DrawBitmap(75,82, block2, 40, 5);
	ST7735_DrawBitmap(115, 82, block1, 5,40);
	ST7735_DrawBitmap(122, 82, green, 15,15);
	ST7735_DrawBitmap(95, 77, green, 15,15);
	ST7735_DrawBitmap(80,42, block2, 40, 5);
	ST7735_DrawBitmap(100, 37, green, 15,15);
	ST7735_DrawBitmap(5,90,red, 9,9);
	bird red1;
	red1.xpos = 5;
	red1.ypos = 90;
	red1.width = 9;
	red1.height = 9;
	ST7735_DrawBitmap(25,127,red, 9,9);
	bird red2;
	red2.xpos = 25;
	red2.ypos = 127;
	red2.width = 9;
	red2.height = 9;
	ST7735_DrawBitmap(40,127,red, 9,9);
	bird red3;
	red3.xpos = 40;
	red3.ypos = 127;
	red3.width = 9;
	red3.height = 9;
	ST7735_DrawBitmap(65,127,black,9,9);
	bird red4;
	red4.xpos = 65;
	red4.ypos = 127;
	red4.width = 9;
	red4.height = 9;
	flg = 0;
			while(flg != 1) {}
				flg = 0;
				setVelocity(&red1);
				t = 0;
					while(red1.xpos < 159 && red1.ypos > 0) {
						eraseBird(&red1);
						setPosition(&red1);
						drawBird(&red1);
						checkCollisions3(&red1);
						}
						ST7735_SetCursor(0, 1);
						ST7735_OutUDec(currentScore);
						Delay10ms(10);
						if(pig1 == 0 && pig2 == 0 && pig3 == 0 && pig4 == 0 && pig5 ==0) {
						break;
					}
							DisableInterrupts();
							x = 0;
							ST7735_DrawBitmap(5,90,red, 9,9);
							red1.xpos = 5;
							red1.ypos = 90;
							ST7735_DrawBitmap(25,127,red, 9,9);
							red2.xpos = 25;
							red2.ypos = 127;
							eraseBird(&red3);
							ST7735_DrawBitmap(50,127,black, 9,9);
							red3.xpos = 50;
							red3.ypos = 127;
							eraseBird(&red4);
							EnableInterrupts();
							flg = 0;
//-------------------------------------------------------------------
				while(flg != 1) {}
					flg = 0;
					setVelocity(&red1);
					t = 0;
						while(red1.xpos < 159 && red1.ypos > 0) {
							eraseBird(&red1);
							setPosition(&red1);
							drawBird(&red1);
							checkCollisions3(&red1);
							} 
							ST7735_SetCursor(0, 1);
							ST7735_OutUDec(currentScore);
							Delay10ms(10);
							if(pig1 == 0 && pig2 == 0 && pig3 == 0 && pig4 == 0 && pig5 ==0) {
							break;
						}
							DisableInterrupts();
							x = 0;
							ST7735_DrawBitmap(5,90,red, 9,9);
							red1.xpos = 5;
							red1.ypos = 90;
							eraseBird(&red2);
							ST7735_DrawBitmap(30,127,black, 9,9);
							red2.xpos = 30;
							red2.ypos = 127;
							eraseBird(&red3);
							EnableInterrupts();
							flg = 0;
				while(flg != 1) {}
					flg = 0;
					setVelocity(&red1);
					t = 0;
					while(red1.xpos < 159 && red1.ypos > 0) {
						eraseBird(&red1);
						setPosition(&red1);
						drawBird(&red1);
						checkCollisions3(&red1);
						}
						ST7735_SetCursor(0, 1);
						ST7735_OutUDec(currentScore);
						Delay10ms(10);
						if(pig1 == 0 && pig2 == 0 && pig3 == 0 && pig4 == 0 && pig5 ==0) {
						break;
						}
						DisableInterrupts();
							x = 0;
							ST7735_DrawBitmap(5,90,black, 9,9);
							red1.xpos = 5;
							red1.ypos = 90;
							eraseBird(&red2);
							EnableInterrupts();
							flg = 0;
				while(flg != 1) {}
					flg = 0;
					setVelocity(&red1);
					t = 0;
					while(red1.xpos < 159 && red1.ypos > 0) {
						eraseBird(&red1);
						setPosition(&red1);
						drawBird1(&red1);
						checkCollisions3(&red1);
						pig1=pig2=pig3=pig4=pig5=0;
						}
						ST7735_SetCursor(0, 1);
						ST7735_OutUDec(currentScore);
						Delay10ms(10);
						if(pig1 == 0 && pig2 == 0 && pig3 == 0 && pig4 == 0 && pig5 ==0) {
						ST7735_DrawBitmap(0,127,fire, 159,127);
							Delay10ms(10000);
						break;
						}
					flg = 0;
	}
}

void engine(void) {
	t++;
}

void edge(void) {
	flg = 1;
	Timer0_Init(&engine, 10000000);
	Sound_Fly();
}

void edge2(void) {
	DisableInterrupts();
	while((GPIO_PORTE_DATA_R & 0x2)>>1 != 1) {};
//	while((GPIO_PORTE_DATA_R & 0x1) != 1) {
//		if(((GPIO_PORTE_DATA_R & 0x2) >> 1) == 1) {
//			break;
//			}
//		}
	EnableInterrupts();
	}

int main(void){
  PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
	Sound_Init();
	DAC_Init();
	ADC_Init();
	EdgeCounter_Init(&edge);
	EdgeCounter_Init2(&edge2);
	EnableInterrupts();
	
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(0x0000);
	ST7735_SetRotation(1);
	ST7735_SetCursor(0, 0);
	ST7735_DrawBitmap(0,127,angrybirds, 159,127);
	Delay10ms(500);
	ST7735_FillScreen(0x0000);

  while(1){	
		Sound_Background();
		Level1();
		ST7735_FillScreen(0x0000);
		Delay10ms(100);
		Level2();
		ST7735_FillScreen(0x0000);
		Delay10ms(100);
		Level3();
		ST7735_FillScreen(0x0000);
		Delay10ms(100);
		break;
		}	
}

void Delay10ms(uint32_t count){
	uint32_t volatile time;
  while(count>0){
    time = 72724;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}

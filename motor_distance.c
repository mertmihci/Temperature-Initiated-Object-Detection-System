#include "TM4C123.h"
#include "stdint.h"
#include "stdio.h"


extern void OutStr(char*);
double measurments[16];


void delay_ms(int milliseconds);
double measure_pulse(void);
void forwardstep(void);
void turnback(void);
void GPIOB_Init(void);
void GPIOE_Init(void);
void GPIOF_Init(void);
void TIMER0_Init(void);
void TIMER0A_Handler(void);



void objectdetection(void){
		for(int i = 0; i < 16; i++){
					measurments[i] = measure_pulse();
					forwardstep();
		}
		delay_ms(1000);
		turnback();
}
void arrange_leds(void){
		uint8_t state = 0;
		for(int i = 0; i < 16; i++){
				if(measurments[i] < 50.0){
						state |= 0x4;
						break;
				}
				else if(measurments[i] < 75.0){
						state |= 0x2;
				}
				else if(measurments[i] < 100.0){
						state |= 0x1;
				}
		}
		if((state & 0x4) == 0x4){
				GPIOF->DATA = 0x2;
		}
		else if((state & 0x2) == 0x2){
				GPIOF->DATA = 0x4;
		}
		else if((state & 0x1) == 0x1){
				GPIOF->DATA = 0x8;
		}
}

int main(void){
		GPIOB_Init();
		GPIOE_Init();
		GPIOF_Init();
		delay_ms(100);
		TIMER0_Init();
		char buffer[50];
		/*for(int i = 0; i < 128 ; i++){
				measurments[i] = measure_pulse();
				sprintf(buffer, "%d st Distance: %.2f cm \r\4",(i+1) , measurments[i]);
				OutStr(buffer); // Assuming outstr handles terminal output
		}*/
		objectdetection();
		arrange_leds();
		
		for(int i = 0; i < 16 ; i++){
				sprintf(buffer, "%d st Distance: %.2f cm \r\4",(i+1) , measurments[i]);
				OutStr(buffer); // Assuming outstr handles terminal output
				delay_ms(10);
		}
		while(1){
		}
}
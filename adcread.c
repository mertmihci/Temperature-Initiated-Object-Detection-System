#include "TM4C123.h"
#include "stdint.h"
#include "stdio.h"


extern void OutStr(char*);
double measurments[128];


void delay_ms(int milliseconds);
double analog_to_voltage(uint16_t measurment);
void Drive_Motor(void);
double measure_pulse(void);
void GPIOB_Init(void);
void GPIOC_Init(void);
void GPIOD_Init(void);
void GPIOE_Init(void);
void GPIOF_Init(void);
void ADC0_Init(void);
uint16_t ADC0_Read(void);
void TIMER0_Init(void);
void TIMER0A_Handler(void);
void COMP0_Init(void);
void COMP0_Handler(void);

void Drive_Motor(void){
		//Uses PD0-3
		uint8_t steps [4] = {0b0001, 0b0010, 0b0100, 0b1000};
		uint8_t step_no = 0;
		for(int i = 0; i < 1024 ; i++){
				GPIOD->DATA &= ~0xF;
				GPIOD->DATA |= steps[step_no];
				step_no = (step_no == 3) ? 0 : step_no + 1;
				delay_ms(5);
				/*if(i % 8 == 0){
						measurments[i/8] = measure_pulse();
				}*/
		}
		step_no = 0;
		for(int i = 0; i < 1024 ; i++){
				GPIOD->DATA &= ~0xF;
				GPIOD->DATA |= steps[step_no];
				step_no = (step_no == 0) ? 3 : step_no - 1;
				delay_ms(5);						
		}	
}

int main(void){
		GPIOB_Init();
		GPIOD_Init();
		delay_ms(100);
		TIMER0_Init();
		//Drive_Motor();
		char buffer[50];
		for(int i = 0; i < 128 ; i++){
				measurments[i] = measure_pulse();
				sprintf(buffer, "%d st Distance: %.2f cm \r\4",(i+1) , measurments[i]);
				OutStr(buffer); // Assuming outstr handles terminal output
		}
		while(1){
		}
}
#include "utils.h"
#include "lm35.h"
#include "Nokia5110.h"
#include "bmp280.h"
#include "keypad.h"
#include "speaker.h"

volatile float LM35_Temperature = 0;
volatile float Digital_Temperature_Threshold = 0;
volatile uint8_t Deep_Sleep_Mode = 1;
volatile float BMP280_Temperature = 0;	
volatile uint8_t Keypad_Read_Value = 0;
volatile uint8_t Show_Graph = 0;

void Init_Power_LED() 
{
    SYSCTL->RCGCGPIO |= (1 << 0);         
    while ((SYSCTL->PRGPIO & (1 << 0)) == 0);

    GPIOA->DIR |= (1 << 4);             
    GPIOA->DEN |= (1 << 4);             
    GPIOA->DATA &= ~(1 << 4);           
}

void COMP1_Handler(void) 
{
    if (COMP->ACMIS & 0x02) 
		{ 
			if (COMP->ACSTAT1 & 0x2)
			{
				Deep_Sleep_Mode = 0;
				GPIOA->DATA |= 0x10;
				COMP->ACINTEN = 0x0;
			}
			COMP->ACMIS |= 0x02;
    }
}

void GPIOB_Handler(void) {
	char key = Keypad_Scan();
	
	if (key >= '0' && key <= '9') 
	{
			Keypad_Read_Value = (Keypad_Read_Value * 10) + (key - '0');
	} 
	else if (key == 'A') 
	{
		Deep_Sleep_Mode = 1;
		COMP->ACINTEN = 0x2;
		GPIOA->DATA &= ~0x10;
	}
	else if (key == 'B') 
	{
		Show_Graph ^= 1;
	}
	else if (key == 'D') 
	{
					Digital_Temperature_Threshold = (float) Keypad_Read_Value;
					Keypad_Read_Value = 0;
	}
    
	GPIOD->DATA = 0x0F;
	GPIOB->ICR |= 0x0F;
}

void Deep_Sleep_Screen()
{
		char buffer[100];
		Nokia5110_Clear();
	
		sprintf(buffer, "LM35: %.2f ", LM35_Temperature);
		Nokia5110_OutString(buffer);
	
		sprintf(buffer, "Dig.: %.2f ", Digital_Temperature_Threshold);
		Nokia5110_OutString(buffer);
	
		sprintf(buffer, "Ana.: %.2f", 29.00);
		Nokia5110_OutString(buffer);
}

void Awake_Screen() 
{
		char buffer[100];
		Nokia5110_Clear();
		
		sprintf(buffer, "LM35: %.2f ", LM35_Temperature);
		Nokia5110_OutString(buffer);
	
		sprintf(buffer, "DigT: %.2f ", Digital_Temperature_Threshold);
		Nokia5110_OutString(buffer);
	
		sprintf(buffer, "BMPT: %.2f", BMP280_Temperature);
		Nokia5110_OutString(buffer);
}

void Graph_Screen(){
	
		char buffer[100];
		Nokia5110_Clear();
		
		sprintf(buffer, "!!GRAPH!!");
		Nokia5110_OutString(buffer);

}

void TIMER1A_Handler(void) {
    TIMER1->ICR = 0x01;
    GPIOD->DATA ^= (1 << 6);
}

int main(void)
{
	Nokia5110_Init();
	Init_SysTick();	
	Init_Power_LED();
	LM35_Init();
	I2C_Init();
	BMP280_Init();
	Keypad_Init();
	Speaker_Init();

	while(1)
	{
		
		BMP280_Temperature = BMP280_ReadTemperature();
		delay_ms(10);
		LM35_Temperature = LM35_Read_Temperature();
		delay_ms(10);
		if(Deep_Sleep_Mode)
		{
			Deep_Sleep_Screen();
			delay_ms(500);
		}
		else
		{
			
			if(Show_Graph){
				Graph_Screen();
			}
			else{
				Awake_Screen();
			}
			
			if (BMP280_Temperature > Digital_Temperature_Threshold){
					Nokia5110_Clear();
					Nokia5110_OutString("  !!HOT!!  ");
					Speaker_Works(BMP280_Temperature);
				
			}
			
			
			delay_ms(500);
			
		}
	}
	
	
}

#include "utils.h"
#include "LM35.h"
#include "PowerLED.h"
#include "Nokia5110.h"
#include "BMP280.h"
#include "speaker.h"

volatile float LM35_Temperature = 0;
volatile float BMP280_Temperature = 0;

volatile float Digital_Temperature_Threshold = 40;
volatile uint8_t Deep_Sleep_Mode = 1;

void COMP1_Handler(void) 
{
    if (COMP->ACMIS & 0x02) 
		{ 
			if (COMP->ACSTAT1 & 0x2)
			{
				Deep_Sleep_Mode = 0;
				GPIOA->DATA |= (1 << 4);
				COMP->ACINTEN = 0x0;
			}
			COMP->ACMIS |= 0x02;
    }
}

void Info_Screen()
{
		char buffer[100];
		Nokia5110_Clear();
	
		sprintf(buffer, "Ana.: %.2f ", 29.50);
		Nokia5110_OutString(buffer);
		
		sprintf(buffer, "LM35: %.2f ", LM35_Temperature);
		Nokia5110_OutString(buffer);
	
		sprintf(buffer, "Dig.: %.2f ", Digital_Temperature_Threshold);
		Nokia5110_OutString(buffer);

		sprintf(buffer, "BMPT: %.2f ", BMP280_Temperature);
		Nokia5110_OutString(buffer);
}

void Awake_Screen() 
{
		char buffer[100];
		Nokia5110_Clear();
		
	
		sprintf(buffer, "DigT: %.2f ", Digital_Temperature_Threshold);
		Nokia5110_OutString(buffer);
	
		sprintf(buffer, "BMPT: %.2f ", BMP280_Temperature);
		Nokia5110_OutString(buffer);

	}



	
int main(){
	
	Nokia5110_Init();
	Speaker_Init();
	Init_SysTick();
	Init_LM35();
	Init_ACOMP1();
	Init_PowerLED();
	I2C_Init();
	delay_ms(10);
	BMP280_Init();
	delay_ms(100);
	
	while(1)
	{
		LM35_Temperature = LM35_Read_Temperature();
		delay_ms(10);
		BMP280_Temperature = BMP280_ReadTemperature();
		delay_ms(10);
		Info_Screen();

		if(!Deep_Sleep_Mode)
		{
			Speaker_Works(BMP280_Temperature);
			print("Awake!");
		}
		delay_ms(500);	
	}
	
}

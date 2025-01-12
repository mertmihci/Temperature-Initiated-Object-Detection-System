#include "LM35.h"
#include "PowerLED.h"
#include "Nokia5110.h"
#include "BMP280.h"
#include "speaker.h"
#include "keypad.h"
#include "motor_distance.h"

volatile float LM35_Temperature = 0;
volatile float BMP280_Temperature = 0;
float Digital_Temperature_Threshold = 0;
uint8_t keypad_read_value = 0;

volatile uint8_t Deep_Sleep_Mode = 1;
volatile uint8_t Once_Flag = 1;
void Speaker_Works(float BMP280_temp){
		
		Speaker_SetFrequency(BMP280_Temperature);
		Speaker_Start();
		delay_ms(2000);
		Speaker_Stop();
}

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
    }
		COMP->ACMIS |= 0x02;
}

void Info_Screen()
{
		char buffer[100];
		Nokia5110_Clear();
	
		if(Deep_Sleep_Mode){
		Nokia5110_OutString(" Deep Sleep ");
		}
	
		sprintf(buffer, "Ana.: %.2f ", 29.50);
		Nokia5110_OutString(buffer);
		
		sprintf(buffer, "LM35: %.2f ", LM35_Temperature);
		Nokia5110_OutString(buffer);
	
		sprintf(buffer, "Dig.: %.2f ", Digital_Temperature_Threshold);
		Nokia5110_OutString(buffer);

		sprintf(buffer, "BMPT: %.2f ", BMP280_Temperature);
		Nokia5110_OutString(buffer);
}


void GPIOB_Handler(void) {
	
	char key = Keypad_Scan();
	//print("Key: %c", key);
	
	if (key >= '0' && key <= '9') 
	{
			keypad_read_value = (keypad_read_value * 10) + (key - '0');
			print("%d", keypad_read_value);
	} 
	else if (key == 'A') 
	{
					Digital_Temperature_Threshold += 1;
	} 
	else if (key == 'B') 
	{
					Digital_Temperature_Threshold -= 1;
	}
	else if (key == 'C') 
	{
				GPIOA->DATA &= ~(1 << 4);
				COMP->ACMIS |= 0x02;
				COMP->ACINTEN |= 0x2;	
				Deep_Sleep_Mode = 1;
				Once_Flag = 1;
	} 
	else if (key == 'D') 
	{
					Digital_Temperature_Threshold = keypad_read_value;
					keypad_read_value = 0;
	} 
    
	GPIOD->DATA |= 0x0F;
	GPIOB->ICR |= 0x0F;
}
	
int main(){
	
	Nokia5110_Init();
	
	Init_SysTick();
	Init_LM35();
	Init_ACOMP1();
	
	Init_PowerLED();
	
	I2C_Init();
	delay_ms(10);
	BMP280_Init();
	
	Speaker_Init();
	Keypad_Init();
	
	motor_init();
	distance_init();
	
	delay_ms(100);

	while(1)
	{
		LM35_Temperature = LM35_Read_Temperature();
		delay_ms(10);
		BMP280_Temperature = BMP280_ReadTemperature();
		delay_ms(10);
		Info_Screen();
				
		if((!Deep_Sleep_Mode) & (BMP280_Temperature > Digital_Temperature_Threshold) & Once_Flag)
		{		
				Once_Flag = 0;
				Speaker_Works(BMP280_Temperature);
				float measurments[16];
				memcpy(measurments, objectdetection(), sizeof(measurments));
				for(int i = 0; i < 16 ; i++){
				print("%d st Distance: %.2f cm \r\4",(i+1) , measurments[i]);
				delay_ms(2);
		}
		}
		delay_ms(500);	
	}
	
}

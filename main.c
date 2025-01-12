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
float angle[33] = {
		0.00, 5.63, 11.25, 16.88, 22.50, 28.13, 33.75, 39.38, 45.00, 50.63,
		56.25, 61.88, 67.50, 73.13, 78.75, 84.38, 90.00, 95.63, 101.25, 106.88,
		112.50, 118.13, 123.75, 129.38, 135.00, 140.63, 146.25, 151.88, 157.50, 163.13,
		168.75, 174.38, 180.00
};
volatile uint8_t keypad_read_value = 0;
volatile uint8_t switch_screen = 1;
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
	print("COMP1_Handler");
    if (COMP->ACMIS & 0x02) 
		{ 
			if (COMP->ACSTAT1 & 0x2)
			{
				Deep_Sleep_Mode = 0;
				GPIOA->DATA |= (1 << 4);
				COMP->ACINTEN = 0x0;
				Init_SysTick();
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
		print("GPIOB_Handler");
	char key = Keypad_Scan();

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
	else if (key == '#') 
	{
		switch_screen ^= 1;
	}
    
	GPIOD->DATA |= 0x0F;
	GPIOB->ICR |= 0x0F;
}






#define SCREEN_WIDTH 84
#define SCREEN_HEIGHT 48
#define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 8) // 504 bytes

void set_bit(uint8_t screen[SCREEN_SIZE], int x, int y) {
    int byte_index = x + (y / 8) * SCREEN_WIDTH;
    int bit_offset = y % 8;
    screen[byte_index] |= (1 << bit_offset);
}



float coss[33] = {
    1.00, 0.99, 0.98, 0.95, 0.92, 0.88, 0.82, 0.76, 0.70, 0.63,
    0.57, 0.50, 0.38, 0.29, 0.19, 0.09, 0.00, 0.09, 0.19, 0.29,
    0.38, 0.50, 0.57, 0.63, 0.70, 0.76, 0.82, 0.88, 0.92, 0.95,
    0.98, 0.99, 1.00
};
float sins[33] = {
    0.00, 0.09, 0.19, 0.28, 0.38, 0.46, 0.54, 0.62, 0.70, 0.76,
    0.82, 0.87, 0.92, 0.95, 0.98, 0.99, 1.00, 0.99, 0.98, 0.95,
    0.92, 0.87, 0.82, 0.76, 0.70, 0.62, 0.54, 0.46, 0.38, 0.28,
    0.19, 0.09, 0.00
};

int x_val[33];
int y_val[33];
unsigned char graphic[504] = {0};

void	draw_graph(float measurments[33])
{
			for (int i = 0; i < 33; i++) {
				y_val[i] = 48 - (int)((measurments[i]*0.48) * sins[i]);  
				if (i <= 15) {
						x_val[i] = 41 + (int)((measurments[i]*0.42) * coss[i]);  
				} 
				else {
						x_val[i] = 42 - (int)((measurments[i]*0.42) * coss[i]); 
				}
		}
			
		for(int i = 0; i < 504; i ++)
		{
			graphic[i] = 0x00;
		
		}		
		for(int i = 0; i < 33; i++)
		{
			set_bit(graphic, x_val[i], y_val[i]);
		}
}

int main(){
	
	Init_PowerLED();
	Nokia5110_Init();
	Init_LM35();
	Init_ACOMP1();	
	sprintf(buffer, "Dig.: %.2f ", Digital_Temperature_Threshold);
	Nokia5110_OutString(buffer);		
	sprintf(buffer, "Ana.: %.2f ", 29.50);
	Nokia5110_OutString(buffer);
	
	__asm("WFI");
	Init_SysTick();
	delay_ms(100);
	I2C_Init();
	delay_ms(10);
	BMP280_Init();
	Keypad_Init();
	Speaker_Init();

	
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
				float measurments[33];
				objectdetection(measurments);
				uint8_t object_detected = 0;
				uint8_t printed_angle_index = 0;
				float min_distance = 100;
				for(int i = 0; i < 32 ; i++)
				{
					if (measurments[i] != 100 && measurments[i] < min_distance)
					{
							min_distance = measurments[i];
							printed_angle_index = i;
							object_detected = 1;
					}
					else{}
				}
				
								
				while(!Deep_Sleep_Mode){
					if(object_detected)
					{
						Nokia5110_Clear();
					
						if(switch_screen)
						{
							sprintf(buffer, "Ang.: %.2f ", angle[printed_angle_index]);
							Nokia5110_OutString(buffer);
							sprintf(buffer, "Dist: %.2f ", min_distance);
							Nokia5110_OutString(buffer);	
						}
						else
						{

							draw_graph(measurments);
							Nokia5110_DrawFullImage(graphic);
							delay_ms(500);
						}
			
					}
					else
					{
						Nokia5110_Clear();
						Nokia5110_OutString("  !NO OBJECT! ");
					}
				}
				
		}
		else if(Deep_Sleep_Mode)
		{
				Nokia5110_Clear();
				sprintf(buffer, "Dig.: %.2f ", Digital_Temperature_Threshold);
				Nokia5110_OutString(buffer);		
				sprintf(buffer, "Ana.: %.2f ", 29.50);
				Nokia5110_OutString(buffer);

				SysTick->CTRL &= ~0x3;
				__asm("WFI");
				SysTick->CTRL |= 0x3;
		
		}
		delay_ms(500);	
	}
	
}

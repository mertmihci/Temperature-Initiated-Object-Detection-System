#include "TM4C123GH6PM.h" // Device-specific header file
#include "speaker.h"
#include "lm35.h"
#include "Nokia5110.h"

volatile uint8_t deep_sleep_mode = 1;
volatile float LM35_Temperature = 0;

char buffer[100];

uint32_t ms;
void _delay_ms(uint16_t delay){

	ms = 0;
	while(delay > ms);
}
void SysTick_Handler(){
	ms++;
}

void init_SysTick(){

	SysTick->CTRL = 3;
	SysTick->LOAD = 4000 - 1;

}


int main(void) {
		init_SysTick();
    ADC_Init();
		COMP0_Init();
    Nokia5110_Init();

    while (1) {
			 	
        if (deep_sleep_mode) {
            LM35_Temperature = LM35_read_temperature();
            sprintf(buffer, "Temperature: %.2f\r\n", LM35_Temperature);
						Nokia5110_Clear();
            Nokia5110_OutString(buffer);

        }
				_delay_ms(1000);
			
    }
}
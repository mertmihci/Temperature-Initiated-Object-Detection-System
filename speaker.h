#include "utils.h"
#define SYSTEM_CLOCK 16000000  // Assuming 16 MHz system clock

void Speaker_Stop(void);

void Speaker_SetFrequency(float temperature) {

    const float tempMin = -10.0;   // Minimum Celsius temperature
    const float tempMax = 100.0;    // Maximum Celsius temperature
    const int freqMin = 100;       // Minimum frequency in Hz
    const int freqMax = 2200;      // Maximum frequency in Hz

    // Clamp the temperature to stay within the valid range
    if (temperature < tempMin) temperature = tempMin;
    if (temperature > tempMax) temperature = tempMax;

    // Perform the linear mapping
    float mappedFreq = freqMin + (temperature - tempMin) * (freqMax - freqMin) / (tempMax - tempMin);

    // Manual rounding
    int frequency = (int)(mappedFreq + 0.5f);
			
    // Calculate the load value for the timer
    TIMER1->TAILR = (SYSTEM_CLOCK / frequency / 2) - 1;
	
}

void Speaker_Init(void) {
    // Enable the clock for Timer 1 and GPIO Port B
		SYSCTL->RCGCTIMER |= (1 << 1);
    SYSCTL->RCGCGPIO |= (1 << 2);   // Enable GPIO Port B clock
    while ((SYSCTL->PRTIMER & 0x02) == 0);
    while ((SYSCTL->PRGPIO & 0x04) == 0);


    GPIOC->DIR|= (1 << 6);   // Set PC6 as output
    GPIOC->DEN |= (1 << 6);   // Enable PC6 as digital


    TIMER1->CTL &= ~0x01;

    // Configure Timer 1 as a 16-bit timer in periodic mode
    TIMER1->CFG = 0x04;  // 16-bit timer configuration
    TIMER1->TAMR = 0x02; // Periodic mode

    // Enable Timer 1 interrupt
    TIMER1->IMR |= 0x01;  // Enable timeout interrupt
		NVIC->ISER[0] |= (1 << 21);
}




void TIMER1A_Handler(void) {
    
    GPIOC->DATA ^= (1 << 6);
		TIMER1->ICR = 0x01;
}

void Speaker_Start(void) {
    TIMER1->CTL |= 0x01; // Enable Timer 1
}

// Stops the PWM signal
void Speaker_Stop(void) {
    TIMER1->CTL &= ~0x01; // Disable Timer 1
    GPIOC->DATA &= ~(1 << 6); // Ensure PB5 is low
}


void Speaker_Works(float BMP280_Temperature){
		
		Speaker_SetFrequency(BMP280_Temperature);
		Speaker_Start();
		delay_ms(2000);
		Speaker_Stop();
}


/*
float BMP_Measurment;

int main(void){
		Speaker_Init();
		for (int i = 0; i < 100; i++) {
        for (volatile int j = 0; j < 1000; j++);
		}
		
		BMP_Measurment = 20.0;
		Speaker_Works(BMP_Measurment);
		for (int i = 0; i < 100; i++) {
        for (volatile int j = 0; j < 1000; j++);
		}
		BMP_Measurment = 30.0;
		Speaker_Works(BMP_Measurment);
		for (int i = 0; i < 100; i++) {
        for (volatile int j = 0; j < 1000; j++);
		}
		BMP_Measurment = 40.0;
		Speaker_Works(BMP_Measurment);
		for (int i = 0; i < 100; i++) {
        for (volatile int j = 0; j < 1000; j++);
		}
}

*/

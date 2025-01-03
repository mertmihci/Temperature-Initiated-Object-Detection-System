#include "TM4C123GH6PM.h"

#define SYSTEM_CLOCK 16000000 

void Speaker_Stop(void);

void Speaker_SetFrequency(float temperature) {
		 if (temperature == 0) {
        Speaker_Stop();
        return;
    }

    const float tempMin = -10.0;   // Minimum Celsius temperature
    const float tempMax = 100.0;    // Maximum Celsius temperature
    const int freqMin = 100;       // Minimum frequency in Hz
    const int freqMax = 2200;      // Maximum frequency in Hz

    if (temperature < tempMin) temperature = tempMin;
    if (temperature > tempMax) temperature = tempMax;

    float mappedFreq = freqMin + (temperature - tempMin) * (freqMax - freqMin) / (tempMax - tempMin);

    int frequency = (int) mappedFreq;
			
    uint32_t load_value = (SYSTEM_CLOCK / frequency / 2) - 1;

    // Update the Timer 1 load value
    TIMER1->TAILR = load_value;

    // Start the timer
    TIMER1->CTL|= 0x01;
}

void Speaker_Init() 
{
    // Enable the clock for Timer 1 and GPIO Port B
		SYSCTL->RCGCTIMER |= (1 << 1);
    while ((SYSCTL->PRTIMER & 0x02) == 0);

    SYSCTL->RCGCGPIO |= (1 << 3);         
    while ((SYSCTL->PRGPIO & (1 << 3)) == 0);

    GPIOD->DIR |= (1 << 6);             
    GPIOD->DEN |= (1 << 6);             
    GPIOD->DATA &= ~(1 << 6); 
	
    TIMER1->CTL &= ~0x01;

    // Configure Timer 1 as a 16-bit timer in periodic mode
    TIMER1->CFG = 0x04;  // 16-bit timer configuration
    TIMER1->TAMR = 0x02; // Periodic mode


    Speaker_SetFrequency(0);

    // Enable Timer 1 interrupt
    TIMER1->IMR |= 0x01;  // Enable timeout interrupt
		NVIC->ISER[0] |= (1 << 21);
}







void Speaker_Start(void) {
    TIMER1->CTL |= 0x01; // Enable Timer 1
}

// Stops the PWM signal
void Speaker_Stop(void) {
    TIMER1->CTL &= ~0x01; // Disable Timer 1
    GPIOB->DATA &= ~(1 << 5); // Ensure PB5 is low
}

void Speaker_Works(float BMP280_temp){
		TIMER1->TAV = 0;
		Speaker_SetFrequency(BMP280_temp);
		Speaker_Start();
		for (int i = 0; i < 1800; i++) {
        for (volatile int j = 0; j < 1000; j++);
		}
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

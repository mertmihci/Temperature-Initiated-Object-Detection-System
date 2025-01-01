#include "TM4C123GH6PM.h"

#define SYSTEM_CLOCK 16000000  // Assuming 16 MHz system clock

static uint32_t current_frequency = 0;

void Speaker_Start(void) {
    TIMER1->CTL |= 0x01; // Enable Timer 1
}

// Stops the PWM signal
void Speaker_Stop(void) {
    TIMER1->CTL &= ~0x01; // Disable Timer 1
    GPIOB->DATA &= ~(1 << 6); // Ensure PB6 is low
}

void Speaker_SetFrequency(uint32_t frequency) {
    if (frequency == 0) {
        Speaker_Stop();
        return;
    }

    current_frequency = frequency;

    // Calculate the load value for the timer
    uint32_t load_value = (SYSTEM_CLOCK / frequency / 2) - 1;

    // Update the Timer 1 load value
    TIMER1->TAILR = load_value;

    // Start the timer
    TIMER1->CTL|= 0x01;
}

void Speaker_Init(uint32_t frequency) {
    // Enable the clock for Timer 1 and GPIO Port B
		SYSCTL->RCGCTIMER |= (1 << 1);
    SYSCTL->RCGCGPIO |= (1 << 1);   // Enable GPIO Port B clock
    while ((SYSCTL->PRTIMER & 0x02) == 0);
    while ((SYSCTL->PRGPIO & 0x02) == 0);


    GPIOB->DIR|= (1 << 6);   // Set PB6 as output
    GPIOB->DEN |= (1 << 6);   // Enable PB6 as digital


    TIMER1->CTL &= ~0x01;

    // Configure Timer 1 as a 16-bit timer in periodic mode
    TIMER1->CFG = 0x04;  // 16-bit timer configuration
    TIMER1->TAMR = 0x02; // Periodic mode


    Speaker_SetFrequency(frequency);

    // Enable Timer 1 interrupt
    TIMER1->IMR |= 0x01;  // Enable timeout interrupt
		NVIC->ISER[0] |= (1 << 21);
}




// Timer 1A interrupt handler
void TIMER1A_Handler(void) {
    // Clear the interrupt flag
    TIMER1->ICR = 0x01;

    // Toggle PB6 to create the PWM signal
    GPIOB->DATA ^= (1 << 6);
}


/*

int main(void) {
    // Initialize the speaker with a frequency of 1 kHz
    Speaker_Init(100);

    // Start the PWM signal
    Speaker_Start();

    // Change frequency to 2 kHz after some delay
    for (int i = 0; i < 50000000; i++);
    Speaker_SetFrequency(200);

    // Stop the speaker after some time
    for (int i = 0; i < 50000000; i++);
    Speaker_Stop();

    while (1);
}

*/

#include "TM4C123GH6PM.h"

// Function to initialize a Power LED connected to PA4
void Init_PowerLED() 
{
    SYSCTL->RCGCGPIO |= (1 << 0);         						// Enable the clock for GPIO Port A
    while ((SYSCTL->PRGPIO & (1 << 0)) == 0);         // Wait for the clock to stabilize for Port A
                                                      
    GPIOA->DIR |= (1 << 4);                           // Set pin as an output
    GPIOA->DEN |= (1 << 4);                           // Enable the digital functionality
    GPIOA->DATA &= ~(1 << 4);                         // Initialize to a low state (LED OFF)
}

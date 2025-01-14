#include "utils.h"
#include <stdint.h>
#include <stdio.h>

#define L_MASK 0x0F // PB0-PB3
#define R_MASK 0x0F // PD0-PD3

// Keypad mapping
const uint8_t keypad_map[4][4] = 
{
    { '1', '2', '3', 'A' },
    { '4', '5', '6', 'B' },
    { '7', '8', '9', 'C' },
    { '*', '0', '#', 'D' }
};

// Keypad initialization
void Keypad_Init(void) 
{
		// Enable clocks for Port B and Port D
		SYSCTL->RCGCGPIO |= (1 << 3) | (1 << 1); 
    while ((SYSCTL->PRGPIO& ((1 << 3) | (1 << 1))) == 0); 

    // Configure Port D (columns) as output
    GPIOD->DIR |= L_MASK;
    GPIOD->DEN |= L_MASK;
		GPIOD->DATA |= 0x0F;

	  // Configure Port B (rows) as input with pull-down resistors
    GPIOB->DIR &= ~R_MASK;
    GPIOB->DEN |= R_MASK;
    GPIOB->PDR |= R_MASK;
				
    GPIOB->IM &= ~R_MASK; 																		// Mask interrupts for rows during configuration
    GPIOB->IS &= ~R_MASK;                                  		// Set interrupts to edge-triggered
    GPIOB->IBE &= ~R_MASK;                                 	 	// Configure for single edge (not both)
    GPIOB->IEV |= R_MASK;                                  	 	// Rising edge triggers the interrupt
    GPIOB->ICR |= R_MASK;                                   	// Clear any prior interrupts
    GPIOB->IM |= R_MASK;                                    	// Enable interrupts for rows

		// Enable interrupts in NVIC
		NVIC->ISER[0] |= (1 << 1); 
}

// Keypad scanning function
uint8_t Keypad_Scan(void)	
{
    
    for (uint8_t L = 0; L < 4; L++) 
		{
				GPIOD->DATA &= ~0x0F;
        GPIOD->DATA |= (1 << L);
        uint8_t R = GPIOB->DATA & R_MASK; 

				if(R & 0x1)
				{
					return keypad_map[L][0];
				}
				else if(R & 0x2)
				{
					return keypad_map[L][1];
				}
				else if(R & 0x4)
				{
					return keypad_map[L][2];
				}
				else if(R & 0x8)
				{
					return keypad_map[L][3];
				}
    }
   return 0;
}

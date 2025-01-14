#include "TM4C123GH6PM.h"

#define ADC_INPUT_PORT (1 << 4)									// ADC input pin PB4
#define ACOMP_INPUT_PORTS ((1 << 4)|(1 << 5))		// Analog Comparator input pins (C1+, PC4), (C1-, PC5)

void Init_ACOMP1() 
{
    SYSCTL->RCGCGPIO |= (1 << 2);  
    SYSCTL->RCGCACMP |= (1 << 0);  							// Enable Analog Comparator Module and GPIO pins

		
    while ((SYSCTL->PRGPIO & (1 << 2)) == 0){};	// Wait until they are ready
		while ((SYSCTL->PRACMP & (1 << 0)) == 0){};
		
			
    GPIOC->AFSEL &= ~ACOMP_INPUT_PORTS;      		// Disable alternate functions
    GPIOC->DEN &= ~ACOMP_INPUT_PORTS;        		// Disable digital functions
		GPIOC->DIR	&= ~ACOMP_INPUT_PORTS;       		// Configure pins as input
    GPIOC->AMSEL |= ACOMP_INPUT_PORTS;       		// Enable analog mode
			
		// Enable Interrupt for Analog Comparator
    COMP->ACCTL1 = 0x10; 												// Configure Analog Comparator 1
		COMP->ACMIS |= 0x03;			                  // Clear interrupts
    COMP->ACINTEN |= 0x2;                       // Enable interrupts for Comparator 1
		NVIC->ISER[0] |= (1 << 26);									// Enable interrupt for Analog Comparator in NVIC
		NVIC_SetPriority(COMP1_IRQn, 7);            // Set priority for Comparator 1 
}

void Init_LM35() 
{
    SYSCTL->RCGCGPIO |= (1 << 1);  
    SYSCTL->RCGCADC |= (1 << 0);   							// Enable ADC Module and GPIO pin

    while ((SYSCTL->PRGPIO & (1 << 1)) == 0){}
		while ((SYSCTL->PRADC & (1 << 0)) == 0){}		// Wait until they are ready

    GPIOB->AFSEL |= ~(1 << 4);  								// Disable alternate function
    GPIOB->DEN &= ~(1 << 4);   								  // Disable digital function
    GPIOB->AMSEL |= (1 << 4);								    // Enable analog mode
		GPIOB->IM &= ~(1 << 4); 										// Disable interrupts
								                                
    ADC0->ACTSS &= ~(1 << 3);										// Disable sample sequencer 3 during initialization
		ADC0->SAC |= 0x4;														// Enable hardware averaging (16 samples)
    ADC0->SSMUX3 = 0x0A; 												// Select input channel AIN10 connected to PB4
    ADC0->SSCTL3 = 0x06;   											// Configure to generate interrupt and mark end of sequence
    ADC0->ACTSS |= (1 << 3);   									// Enable sample sequencer 3
}								


float LM35_Read_Temperature() 
{
	
    ADC0->PSSI = (1 << 3);  										// Trigger sampling from ADC
    while ((ADC0->RIS & (1 << 3)) == 0);  			// Wait for the sampling done
    ADC0->ISC = (1 << 3);       								// Clear the interrupt

    return ((ADC0->SSFIFO3 & 0x0FFF) * 330) / 4095.0;
}

void Init_ADC1(){

    SYSCTL->RCGCGPIO |= (1 << 1);  
    SYSCTL->RCGCADC |= (1 << 1);   							// Enable ADC Module and GPIO pin

    while ((SYSCTL->PRGPIO & (1 << 1)) == 0){}
		while ((SYSCTL->PRADC & (1 << 1)) == 0){}		// Wait until they are ready

    GPIOB->AFSEL |= ~(1 << 5);  								// Disable alternate function
    GPIOB->DEN &= ~(1 << 5);   								  // Disable digital function
    GPIOB->AMSEL |= (1 << 5);								    // Enable analog mode
		GPIOB->IM &= ~(1 << 5); 										// Disable interrupts
								                                
    ADC1->ACTSS &= ~(1 << 3);										// Disable sample sequencer 3 during initialization
		ADC1->SAC |= 0x6;														// Enable hardware averaging (16 samples)
    ADC1->SSMUX3 = 0x0B; 												// Select input channel AIN11 connected to PB5
    ADC1->SSCTL3 = 0x06;   											// Configure to generate interrupt and mark end of sequence
    ADC1->ACTSS |= (1 << 3);   									// Enable sample sequencer 3


}

float Read_AnalogThreshold() 
{
	
    ADC1->PSSI = (1 << 3);  										// Trigger sampling from ADC
    while ((ADC1->RIS & (1 << 3)) == 0);  			// Wait for the sampling done
    ADC1->ISC = (1 << 3);       								// Clear the interrupt

    return ((ADC1->SSFIFO3 & 0x0FFF) * 330) / 4095.0;
}

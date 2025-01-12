#include "TM4C123GH6PM.h"

#define ADC_INPUT_PORT (1 << 4)
#define ACOMP_INPUT_PORTS ((1 << 4)|(1 << 5))

void Init_ACOMP1() 
{
    SYSCTL->RCGCGPIO |= (1 << 2);  
    SYSCTL->RCGCACMP |= (1 << 0);  					// Enable Analog Comparator Module

    while ((SYSCTL->PRGPIO & (1 << 2)) == 0){};
		while ((SYSCTL->PRACMP & (1 << 0)) == 0){};
		
    GPIOC->AFSEL &= ~ACOMP_INPUT_PORTS;  
    GPIOC->DEN &= ~ACOMP_INPUT_PORTS;   
		GPIOC->DIR	&= ~ACOMP_INPUT_PORTS;  
    GPIOC->AMSEL |= ACOMP_INPUT_PORTS;  

		// Enable Interrupt for Analog Comparator
    COMP->ACCTL1 = 0x10; 
		COMP->ACMIS |= 0x03;			
    COMP->ACINTEN |= 0x2;      
		NVIC->ISER[0] |= (1 << 26);
}

void Init_LM35() 
{
    SYSCTL->RCGCGPIO |= (1 << 1);  
    SYSCTL->RCGCADC |= (1 << 0);   

    while ((SYSCTL->PRGPIO & (1 << 1)) == 0){}
		while ((SYSCTL->PRADC & (1 << 0)) == 0){}

    GPIOB->AFSEL |= ~(1 << 4);  
    GPIOB->DEN &= ~(1 << 4);   
    GPIOB->AMSEL |= (1 << 4);
		GPIOB->IM &= ~(1 << 4); 		// Disable interrupt for LM35

    ADC0->ACTSS &= ~(1 << 3);		// Disable SS3 during initiliza  
		ADC0->SAC |= 0x4;						// ADC takes 16 samples and give the average
    ADC0->SSMUX3 = 0x0A; 				// Use Analog Input 10  (AIN10)
    ADC0->SSCTL3 = 0x06;   			// Sample Interrupt Enable, End of Sequence
    ADC0->ACTSS |= (1 << 3);   	//	Enable SS3
}


float LM35_Read_Temperature() 
{
	
    ADC0->PSSI = (1 << 3);  							// Trigger sampling from ADC
    while ((ADC0->RIS & (1 << 3)) == 0);  // Wait for the sampling done
    ADC0->ISC = (1 << 3);       					// Clear the interrupt

    return ((ADC0->SSFIFO3 & 0x0FFF) * 330) / 4095.0;
}



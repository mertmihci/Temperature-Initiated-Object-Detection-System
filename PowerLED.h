#include "TM4C123GH6PM.h"

void Init_PowerLED() 
{
    SYSCTL->RCGCGPIO |= (1 << 0);         
    while ((SYSCTL->PRGPIO & (1 << 0)) == 0);

    GPIOA->DIR |= (1 << 4);             
    GPIOA->DEN |= (1 << 4);             
    GPIOA->DATA &= ~(1 << 4);           
}

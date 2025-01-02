#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"  

#define I2C_TIMEOUT 10000  
extern void OutStr(char*);

// Initialize I2C module
void I2C_Init() 
{
    SYSCTL->RCGCI2C |= (1 << 2);  
    SYSCTL->RCGCGPIO |= (1 << 4); 

    GPIOE->AFSEL |= (1 << 4) | (1 << 5); 
    GPIOE->ODR |= (1 << 5);              
    GPIOE->PCTL |= 0x00330000; 
    GPIOE->DEN |= (1 << 4) | (1 << 5);   

    I2C2->MCR = 0x10;  
    I2C2->MTPR = 7;    
}

void I2C_WriteByte(uint8_t slaveAddr, uint8_t reg, uint8_t data) {
    I2C2->MSA = (slaveAddr << 1) & ~0x01; 
    I2C2->MDR = reg;                            
    I2C2->MCS = 0x03;                           
    while (I2C2->MCS & 0x01);                  
    if (I2C2->MCS & 0x02) {
        OutStr("I2C Write Error (1)\r\4");
        return;
    }

    I2C2->MDR = data;                             
    I2C2->MCS = 0x05;                             
    while (I2C2->MCS & 0x01);                     
    if (I2C2->MCS & 0x02) {
        OutStr("I2C Write Error (2) \r\4");
    }
}


uint8_t I2C_ReadByte(uint8_t slaveAddr, uint8_t reg) {
    I2C2->MSA = (slaveAddr << 1) & ~0x01; 
    I2C2->MDR = reg;                      
    I2C2->MCS = 0x03;                     
    while (I2C2->MCS & 0x01);             
    if (I2C2->MCS & 0x02) {
        OutStr("I2C Read Error (1)\r\4");
        return 0xFF;
    }

    I2C2->MSA = (slaveAddr << 1) | 0x01;  
    I2C2->MCS = 0x07;                     
    while (I2C2->MCS & 0x01);             
    if (I2C2->MCS & 0x02) {
        OutStr("I2C Read Error (2)\r\4");
        return 0xFF;
    }

    return I2C2->MDR;                     
}



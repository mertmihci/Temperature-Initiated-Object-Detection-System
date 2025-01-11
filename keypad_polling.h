#include "TM4C123GH6PM.h"
#include <stdint.h>
#include <stdio.h>

#define L_MASK 0x0F // PB0-PB3
#define R_MASK 0x0F // PD0-PD3

extern void OutStr(char*);

volatile uint8_t pressed_key = 0xFF;
volatile uint8_t keypad_read_value = 0;
volatile uint32_t digital_temperature_threshold = 31;  

char buffer[100];

const uint8_t keypad_map[4][4] = {
    { '1', '2', '3', 'A' },
    { '4', '5', '6', 'B' },
    { '7', '8', '9', 'C' },
    { '*', '0', '#', 'D' }
};

void delay_ms(uint32_t delay) {
    for (uint32_t i = 0; i < (2000*delay); i++) {
        __asm("NOP");
    }
}

void Keypad_Init(void) {
		SYSCTL->RCGCGPIO |= (1 << 3) | (1 << 1); 
    while ((SYSCTL->PRGPIO& ((1 << 3) | (1 << 1))) == 0); 

    GPIOB->DIR |= L_MASK;
    GPIOB->DEN |= L_MASK;
		GPIOB->DATA |= 0x0F;

    GPIOD->DIR &= ~R_MASK;
    GPIOD->DEN |= R_MASK;
    GPIOD->PDR |= R_MASK; 
}

uint8_t Keypad_Scan(void) {
    GPIOB->DATA = 0x00;
		while(1){
				for (uint8_t L = 0; L < 4; L++) {
						GPIOB->DATA = (1 << L);
						delay_ms(10);

						uint8_t R = GPIOD->DATA & R_MASK; 

						if(R & 0x1){
							return keypad_map[L][0];
						}
						else if(R & 0x2){
							return keypad_map[L][1];
						}
						else if(R & 0x4){
							return keypad_map[L][2];
						}
						else if(R & 0x8){
							return keypad_map[L][3];
				}
    }
	}
}


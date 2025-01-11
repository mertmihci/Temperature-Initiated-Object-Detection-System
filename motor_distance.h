#include "TM4C123.h"
#include "stdio.h"
#include "stdint.h"
/*
	-----------------------FUNCTIONS TO BE ADDED TO MAIN
float measurments[16];
void objectdetection(void){
		for(int i = 0; i < 16; i++){
					measurments[i] = measure_pulse();
					forwardstep();
		}
		delay_ms(1000);
		turnback();
}
void arrange_leds(void){
		uint8_t state = 0;
		for(int i = 0; i < 16; i++){
				if(measurments[i] < 50.0){
						state |= 0x4;
						break;
				}
				else if(measurments[i] < 75.0){
						state |= 0x2;
				}
				else if(measurments[i] < 100.0){
						state |= 0x1;
				}
		}
		if((state & 0x4) == 0x4){
				GPIOF->DATA = 0x2;
		}
		else if((state & 0x2) == 0x2){
				GPIOF->DATA = 0x4;
		}
		else if((state & 0x1) == 0x1){
				GPIOF->DATA = 0x8;
		}
}
*/
void delay_ms(int milliseconds) {
    for (int i = 0; i < milliseconds; i++) {
        // Busy-wait loop for 1 millisecond (adjust according to your system's clock speed)
        for (volatile int j = 0; j < 1000; j++);
    }
}

void distance_init(void){

		// Configure PB7 for input capture, PB6 for pulse
		SYSCTL->RCGCGPIO |= 0x2;
		__ASM("NOP"); __ASM("NOP"); __ASM("NOP");
		GPIOB->AFSEL &= ~0xC0;
		GPIOB->AFSEL |= 0x80;
    GPIOB->PCTL &= ~0xF0000000;
    GPIOB->PCTL |= 0x70000000;
		GPIOB->DIR |= 0x40;  // PB6 as output (TRIG)
		GPIOB->DIR &= ~0x80; // PB7 as input (ECHO)
    GPIOB->DEN |= 0xC0;

		//Configure F1-3 as outputs in order to drive built-in leds.
		SYSCTL->RCGCGPIO |= 0x20; // turn on bus clock for GPIOF
		__ASM("NOP");__ASM("NOP");__ASM("NOP");
		GPIOF->AFSEL &= ~0xE;  // Regular port function
		GPIOF->AMSEL &= ~0xE; //Disable analog
		GPIOF->DEN |= 0xE;  // Enable PF1-3 pin as a digital pin
		GPIOF->DIR |= 0xE; //set PF1-3 as output
	
		delay_ms(100);
		
		//Configure Timer1A triger generation, Timer1B to capture echo
		SYSCTL->RCGCTIMER	|=0x1; // Start timer0 clock
		__ASM("NOP"); __ASM("NOP"); __ASM("NOP");
		TIMER0->CTL &= ~0x101; //Disable timer during setup
		TIMER0->CFG	= 0x4;  //Set 16 bit split timer mode
		TIMER0->TAMR = 0x2; //Set TIMERA to periodic, count down
		TIMER0->TBMR = 0x17;	//Set TIMERB to capture edge time, count up
		TIMER0->TAILR		= 8000; //Set interval load as LOW
		TIMER0->TAPR = 15; // Timer A divide the clock by 16 to get 1us
		TIMER0->IMR	= 0x1; //Enable timeout interrupt	
		__ASM("CPSIE I");
		NVIC->IPR[0] &= 0x00FFFFFF; //Clear interrupt 19 priority
		NVIC->IPR[0] |= 0x40000000; //Set interrupt 19 priority to 2
		NVIC->ISER[0] |= 0x00080000;	//Enable ISER
}

void TIMER0A_Handler(void){
		GPIOB->DATA ^= 0x40;  // Toggle PB6
		TIMER0->TAILR = ((GPIOB->DATA & 0x40) == 0x40) ? 2000 : 8000;
		TIMER0->ICR = 0x401;
}

float measure_pulse(void) {
		TIMER0->CTL |= 0xF03;
    uint32_t rising_edge, falling_edge;
		int32_t pulse_width;
		double distance;
    // Wait for first rising edge
		while((TIMER0->RIS & 0x400) == 0); 
		rising_edge = TIMER0->TBR / 16 ; // Capture the value of the timer at first rising edge
		TIMER0->ICR = 0x400; // Clear the capture interrupt flag
		TIMER0->CTL &= ~0x1;  // Disable Timer A
		TIMER0->TAV = 0;       // Reset the timer counter to 0
		
		

    // Wait for falling edge
    while ((TIMER0->RIS & 0x400) == 0);
    falling_edge = TIMER0->TBR / 16; // Capture the value of the timer at falling edge
    TIMER0->ICR = 0x400; // Clear the capture interrupt flag


	
    // Calculate measurements
    pulse_width = (falling_edge - rising_edge);
		distance = ((float)pulse_width * 0.034) / 2; // Correct formula
		TIMER0->CTL &= ~0xF03;
		TIMER0->ICR = 0x401;
		if((distance < 0.0)|(distance > 100.0)){
				distance = 100.0;
		}
		return distance;
}

void motor_init(void){
		SYSCTL->RCGCGPIO |= 0x10;
		__ASM("NOP"); __ASM("NOP"); __ASM("NOP");
		GPIOE->AFSEL &= ~0xF;
		GPIOE->AMSEL &= ~0xF;
		GPIOE->DEN |= 0xF;
		GPIOE->DIR |= 0xF;
		GPIOE->DATA &= ~0xF;
}
void forwardstep(void){
		//Uses PE0-3
		uint8_t steps [4] = {0b0001, 0b0010, 0b0100, 0b1000};
		uint8_t step_no = 0;
		for(int i = 0; i < 64 ; i++){
				GPIOE->DATA &= ~0xF;
				GPIOE->DATA |= steps[step_no];
				step_no = (step_no == 3) ? 0 : step_no + 1;
				delay_ms(20);
		}
}
void turnback(void){
		uint8_t steps [4] = {0b0001, 0b0010, 0b0100, 0b1000};
		uint8_t step_no = 3;
		for(int i = 0; i < 1024 ; i++){
				GPIOE->DATA &= ~0xF;
				GPIOE->DATA |= steps[step_no];
				step_no = (step_no == 0) ? 3 : step_no - 1;
				delay_ms(10);						
		}	
}
/*
int main(void){
		motor_init();
		distance_init();
		objectdetection();
		arrange_leds();
		char buffer[50];	
		for(int i = 0; i < 16 ; i++){
				sprintf(buffer, "%d st Distance: %.2f cm \r\4",(i+1) , measurments[i]);
				OutStr(buffer); // Assuming outstr handles terminal output
				delay_ms(10);
		}
		while(1){

		}
}
*/

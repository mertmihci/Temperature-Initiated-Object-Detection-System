#include "utils.h"

void distance_init(void){

		// Configure PF4 for input capture, PF0 for triggering pulse
		SYSCTL->RCGCGPIO |= 0x20;
		__ASM("NOP"); __ASM("NOP"); __ASM("NOP");
		GPIOF->LOCK = 0x4C4F434B;
		GPIOF->CR = 0xFF;
		GPIOF->AFSEL &= ~0b10001;
		GPIOF->AFSEL |= 0x10;
    GPIOF->PCTL &= ~0xF0000;
    GPIOF->PCTL |= 0x70000;
		GPIOF->DIR |= 0x1;  // PF0 as output (TRIG)
		GPIOF->DIR &= ~0x10; // PF4 as input (ECHO)
    GPIOF->DEN |= 0b10001;

		//Configure F1-3 as outputs in order to drive built-in leds.
		GPIOF->AFSEL &= ~0xE;  // Regular port function
		GPIOF->AMSEL &= ~0xE; //Disable analog
		GPIOF->DEN |= 0xE;  // Enable PF1-3 pin as a digital pin
		GPIOF->DIR |= 0xE; //set PF1-3 as output
	
		delay_ms(100);
		
		//Configure Timer0A triger generation
		SYSCTL->RCGCTIMER	|=0x1; // Start Timer0 clock
		__ASM("NOP"); __ASM("NOP"); __ASM("NOP");
		TIMER0->CTL &= ~0x1; //Disable timer during setup
		TIMER0->CFG	= 0x4;  //Set 16 bit split timer mode
		TIMER0->TAMR = 0x2; //Set TIMERA to periodic, count down
		TIMER0->TAILR		= 8000; //Set interval load as LOW
		TIMER0->TAPR = 15; // Timer A divide the clock by 16 to get 1us
		TIMER0->IMR	= 0x1; //Enable timeout interrupt	
		__ASM("CPSIE I");
		NVIC_SetPriority(TIMER0A_IRQn, 0);
		NVIC_EnableIRQ(TIMER0A_IRQn);
		
		//Configure Timer2A capture the echo
		SYSCTL->RCGCTIMER |= 0x4;
		__ASM("NOP"); __ASM("NOP"); __ASM("NOP");
		TIMER2->CTL &= ~0x1; //Disable timer during setup
		TIMER2->CFG	= 0x4;  //Set 16 bit split timer mode
		TIMER2->TAMR = 0x17;         // Capture mode, edge-time, count up
    TIMER2->CTL |= 0xC;         // Capture on both edges (rising and falling)
		TIMER2->CTL |= 0x1;					//start Timer2
}

void TIMER0A_Handler(void){
		print("TIMER0A_Handler");
		GPIOF->DATA ^= 0x1;  // Toggle PF0
		TIMER0->TAILR = ((GPIOF->DATA & 0x1) == 0x1) ? 2000 : 8000;
		TIMER0->ICR = 0x1;
}

float measure_pulse(void) {
		TIMER0->CTL |= 0x3;	//Enable Timer0
    uint32_t rising_edge, falling_edge;
		int32_t pulse_width;
		double distance;
    // Wait for first rising edge
		while((TIMER2->RIS & 0x4) == 0); 
		rising_edge = TIMER2->TAR / 16 ; // Capture the value of the timer at first rising edge
		TIMER2->ICR = 0x4; // Clear the capture interrupt flag od Timer2
		TIMER0->CTL &= ~0x1;  // Disable Timer0
		TIMER0->TAV = 0;       // Reset the Timer0 counter to 0
		
		

    // Wait for falling edge
    while ((TIMER2->RIS & 0x4) == 0);
    falling_edge = TIMER2->TAR / 16; // Capture the value of the timer at falling edge
    TIMER2->ICR = 0x4; // Clear the capture interrupt flag


	
    // Calculate measurements
    pulse_width = (falling_edge - rising_edge);
		distance = ((float)pulse_width * 0.034) / 2; // Correct formula
		TIMER0->ICR = 0x1;
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
		for(int i = 0; i < 32 ; i++){
				GPIOE->DATA &= ~0xF;
				GPIOE->DATA |= steps[step_no];
				step_no = (step_no == 3) ? 0 : step_no + 1;
				delay_ms(10);
		}
}
void turnback(void){
		uint8_t steps [4] = {0b0001, 0b0010, 0b0100, 0b1000};
		uint8_t step_no = 3;
		for(int i = 0; i < 1024 ; i++){
				GPIOE->DATA &= ~0xF;
				GPIOE->DATA |= steps[step_no];
				step_no = (step_no == 0) ? 3 : step_no - 1;
				delay_ms(5);						
		}
		GPIOE->DATA &= ~0xF;
}

void arrange_leds(float* measurments){
		uint8_t state = 0;
		GPIOF->DATA &= ~0xE;
		for(int i = 0; i < 33; i++){
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
				GPIOF->DATA |= 0x2;
		}
		else if((state & 0x2) == 0x2){
				GPIOF->DATA |= 0x4;
		}
		else if((state & 0x1) == 0x1){
				GPIOF->DATA |= 0x8;
		}
}

void objectdetection(float measurments[33]){
		//static float measurments[33];
		for(int i = 0; i < 32; i++){
					measurments[i] = measure_pulse();
					forwardstep();
		}
		measurments[32] = measure_pulse();
		delay_ms(100);
		turnback();
		arrange_leds(measurments);
}
/*
int main(void){
		motor_init();
		distance_init();
		Keypad_Init();
		float measurment_out[33];
		memcpy(measurment_out, objectdetection(), sizeof(measurment_out));
		char buffer[50];	
		for(int i = 0; i < 33 ; i++){
				sprintf(buffer, "%d st Distance: %.2f cm \r\4",(i+1) , measurment_out[i]);
				OutStr(buffer); // Assuming outstr handles terminal output
				delay_ms(10);
		}
		while(1){

				delay_ms(100);
				
		}
}
*/

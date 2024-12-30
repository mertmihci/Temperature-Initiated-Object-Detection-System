#include "TM4C123.h"
#include "stdint.h"
#include "stdio.h"

extern void OutStr(char*);


void delay_ms(int milliseconds) {
    for (int i = 0; i < milliseconds; i++) {
        // Busy-wait loop for 1 millisecond (adjust according to your system's clock speed)
        for (volatile int j = 0; j < 1000; j++);
    }
}

double analog_to_voltage(uint16_t measurment){
		double voltage = (measurment / 4096.0) * 3300.0; // Millivolts
		double degreem = voltage / 10.0; // Convert mV to degrees Celsius
		return degreem;
}


double measure_pulse(void) {
		TIMER0->CTL |= 0xF03;//Enable Timers, Timer B count both edges. Timer A-B freeze on debug.
    uint32_t rising_edge, falling_edge;
		int32_t pulse_width;
		double distance;
    // Wait for first rising edge
		while((TIMER0->RIS & 0x400) == 0); 
		rising_edge = TIMER0->TBR / 16 ; // Capture the value of the timer at first rising edge
		TIMER0->ICR = 0x400; // Clear the capture interrupt flag
		TIMER0->CTL &= ~0x1;  // Disable Timer A
		TIMER0->TAV = 0;       // Reset the timer counter to 0
		GPIOB->DATA &= ~0x40;
		
		

    // Wait for falling edge
    while ((TIMER0->RIS & 0x400) == 0);
    falling_edge = TIMER0->TBR / 16; // Capture the value of the timer at falling edge
    TIMER0->ICR = 0x400; // Clear the capture interrupt flag


	
    // Calculate measurements
    pulse_width = (falling_edge - rising_edge);
		distance = ((double)pulse_width * 0.034) / 2; // Correct formula

		TIMER0->CTL &= ~0xF03;
		TIMER0->TAILR = 8000;
		TIMER0->ICR = 0x401;
		
		if(falling_edge > rising_edge){// Print results (handled by outstr function)
				return distance;
		}
		else{return 100.0;}
}

void GPIOB_Init(void){
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
}

void GPIOC_Init(void){
		//Configure C6 & C7 as inputs of Analog Comparator.
		SYSCTL->RCGCGPIO |= 0x4;
		__asm("NOP"); __asm("NOP"); __asm("NOP");
		GPIOC->LOCK = 0x4C4F434B;
		GPIOC->AFSEL |= 0xC0;
		GPIOC->AMSEL |= 0xC0;
		GPIOC->DEN &= ~0xC0;
		GPIOC->DIR &= ~0xC0;
		GPIOC->PCTL &= ~0xFF000000;
}

void GPIOD_Init(void){
		//Configure D0-3 to drive the step motor.
		SYSCTL->RCGCGPIO |= 0x8;
		__asm("NOP"); __asm("NOP"); __asm("NOP");
		GPIOD->AFSEL &= ~0xF;
		GPIOD->AMSEL &= ~0xF;
		GPIOD->DEN |= 0xF;
		GPIOD->DIR |= 0xF;
}


void GPIOE_Init(void){
		//Configure E3 as analog input. E3 will take the measurment of LM35's signals.
		SYSCTL->RCGCGPIO |= 0x10;
		__asm("NOP"); __asm("NOP"); __asm("NOP");
		
		GPIOE->LOCK = 0x4C4F434B;
		GPIOE->AFSEL |= 0x8;
		GPIOE->AMSEL |= 0x8;
		GPIOE->DEN &= ~0x8;
		GPIOE->DIR &= ~0x8;
}

void GPIOF_Init(void){
		//Configure F1-3 as outputs in order to drive built-in leds.
		SYSCTL->RCGCGPIO |= 0x20; // turn on bus clock for GPIOF
		__ASM("NOP");__ASM("NOP");__ASM("NOP");
		//GPIOF->LOCK = 0x4C4F434B;
		GPIOF->DIR |= 0xF; //set PF0-3 as output
		//1-3
		GPIOF->AFSEL &= ~0xE;  // Regular port function
		GPIOF->AMSEL &= ~0xE; //Disable analog
		GPIOF->DEN       |= 0xE;  // Enable PF1-3 pin as a digital pin
		//0
		GPIOF->AFSEL |= 0x1;
		GPIOF->DEN |= 0x1;
		GPIOF->AMSEL &= ~0x1;
		GPIOF->PCTL &= ~0x0000000F;
		GPIOF->PCTL |= 0x00000009;
}

void ADC0_Init(void){
		//Configure analog comparator to recieve signals of LM35. Configured for using PE3 i.e. AIN0.
		SYSCTL->RCGCADC |= 0x1;
		for(volatile int i = 0; i < 1000; i++);
		ADC0->ACTSS &= ~0x8;
		ADC0->EMUX &= 0xF000;
		ADC0->SSMUX3 = 0;
		ADC0->SSCTL3 = 0x6;
		ADC0->PC = 1;
		ADC0->ACTSS |= 0x8;
}

uint16_t ADC0_Read(void){
		//Read the signals from LM35
		ADC0->PSSI = 0x8;
		while ((ADC0->RIS & 0x8) == 0);
		uint16_t result = ADC0->SSFIFO3 & 0xFFF;
		ADC0->ISC = 0x8;
		return result;
}


void TIMER0_Init(void){
		SYSCTL->RCGCTIMER	|=0x1; // Start timer0 clock
		__ASM("NOP"); __ASM("NOP"); __ASM("NOP");
		TIMER0->CTL &= ~0x101; //Disable timer during setup
		TIMER0->CFG	= 0x4;  //Set 16 bit split timer mode
	
		TIMER0->TAMR = 0x2; //Set TIMERA to periodic, count down
		TIMER0->TBMR = 0x17;	//Set TIMERB to capture edge time, count up
	
		TIMER0->TAILR		= 8; //Set interval load as LOW
	
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

void COMP0_Init(void) {
    SYSCTL->RCGCACMP |= 0x1;  
    while (!(SYSCTL->PPACMP & 0x1)); 
		
		COMP->ACINTEN &= ~0x3;
    COMP->ACMIS |= ~0x3;
	
    COMP->ACCTL0 &= ~0xC;   // Ensure level-triggered interrupts
    COMP->ACCTL0 |= 0x10;   // Use level-triggered interrupts
    COMP->ACCTL0 &= ~0x600; // Use comparator output for interrupts

    COMP->ACINTEN |= 0x1;
		COMP->ACMIS |= 0x1;	
	
		NVIC->ISER[0] |= (1 << 25);
}

void COMP0_Handler(void) {
    OutStr(" COMP0_Handler \r\4");
    for (int i = 0; i < 250000; i++) {}
    COMP->ACMIS |= 0x01;
}



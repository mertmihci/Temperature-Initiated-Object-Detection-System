#include "TM4C123GH6PM.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#pragma once
extern void OutStr(char*);

uint32_t ms;
void delay_ms(uint16_t delay)
{
	ms = 0;
	while(delay > ms);
}
void SysTick_Handler()
{
	ms++;
}

void Init_SysTick()
{
	SysTick->CTRL = 3;
	SysTick->LOAD = 4000 - 1;
}

void print(const char *format, ...)
{
    static char buffer[256];
    static char formatted_buffer[256];
    va_list args;

    va_start(args, format);

    vsnprintf(formatted_buffer, sizeof(formatted_buffer), format, args);

    va_end(args);

    buffer[0] = '\0';
    strncat(buffer, formatted_buffer, sizeof(buffer) - 3);

    strcat(buffer, "\r\4");
	
    OutStr(buffer);
}

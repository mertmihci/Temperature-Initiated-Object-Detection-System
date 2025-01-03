#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include "TM4C123GH6PM.h"

extern void OutStr(char*);

void Init_SysTick(void);
void delay_ms(uint16_t delay);

void print(const char *format, ...);


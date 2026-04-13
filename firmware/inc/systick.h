#ifndef SYSTICK_H
#define SYSTICK_H
#include <stdint.h>

void SysTick_Init(void);
uint32_t Get_Millis(void);
void Delay_ms(uint32_t ms);

#endif
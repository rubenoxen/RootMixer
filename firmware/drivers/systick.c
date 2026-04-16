#include "stm32f411.h"
#include "systick.h"

volatile uint32_t system_millis = 0;

void SysTick_Init(void) {
    SYST_RVR = 96000 - 1;
    SYST_CVR = 0;
    SYST_CSR = 7;
}

void SysTick_Handler(void) {
    system_millis++;
}

uint32_t Get_Millis(void) {
    return system_millis;
}

void Delay_ms(uint32_t ms) {
    uint32_t start = system_millis;
    while ((system_millis - start) < ms);
}
#include <stdint.h>
#include "stm32f411.h"

void SystemClock_Config(void) {
    RCC_CR |= (1 << 16);

    while (!(RCC_CR & (1 << 17)));

    FLASH_ACR = (1 << 8) | (1 << 9) | (1 << 10) | (3 << 0);

    uint32_t pllm = 25;
    uint32_t plln = 192;
    uint32_t pllp = 0;
    uint32_t pllq = 4;

    RCC_PLLCFGR = (pllq << 24) | (1 << 22) | (pllp << 16) | (plln << 6) | pllm;

    RCC_CR |= (1 << 24);

    while (!(RCC_CR & (1 << 25)));

    RCC_CFGR |= (2 << 0);

    while ((RCC_CFGR & (3 << 2)) != (2 << 2));
}
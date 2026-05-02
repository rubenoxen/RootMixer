#include "stm32f411.h"

void SystemClock_Config(void) {
    RCC_CR |= 1;
    RCC_CFGR = 0;
    RCC_CR &= ~(0x010B0000);
    RCC_PLLCFGR = 0x24003010;

    RCC_CR |= (1 << 16);
    while (!(RCC_CR & (1 << 17)));

    FLASH_ACR = (1 << 8) | (1 << 9) | (1 << 10) | (3 << 0);

    RCC_PLLCFGR = (4 << 24) | (1 << 22) | (0 << 16) | (192 << 6) | 25;

    RCC_CR |= (1 << 24);
    while (!(RCC_CR & (1 << 25)));

    RCC_CFGR = (4 << 10) | 2;
    while ((RCC_CFGR & (3 << 2)) != (2 << 2));
}
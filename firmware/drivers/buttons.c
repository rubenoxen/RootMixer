#include "stm32f411.h"
#include "buttons.h"

void Buttons_Init(void) {
    RCC_AHB1ENR |= (1 << 1);
    GPIOB_MODER &= ~(0xFF);
    GPIOB_PUPDR &= ~(0xFF);
    GPIOB_PUPDR |= 0x55;
}

uint8_t Buttons_Read(void) {
    return ~(GPIOB_IDR & 0x0F);
}
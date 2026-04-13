#include "buttons.h"
#include "stm32f411.h"

#define GPIOB_BASE      0x40020400
#define GPIOB_MODER     *((volatile uint32_t *)(GPIOB_BASE + 0x00))
#define GPIOB_IDR       *((volatile uint32_t *)(GPIOB_BASE + 0x10))
#define GPIOB_PUPDR     *((volatile uint32_t *)(GPIOB_BASE + 0x0C))

void Buttons_Init(void) {
    RCC_AHB1ENR |= (1 << 1);

    GPIOB_MODER &= ~(0xFF);

    GPIOB_PUPDR &= ~(0xFF);
    GPIOB_PUPDR |= 0x55;
}

uint8_t Buttons_Read(void) {
    return ~(GPIOB_IDR & 0x0F);
}
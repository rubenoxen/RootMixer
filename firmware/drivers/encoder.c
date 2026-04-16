#include "stm32f411.h"
#include "encoder.h"

void Encoder_Init(void) {
    RCC_AHB1ENR |= (1 << 0);
    RCC_APB2ENR |= (1 << 0);

    GPIOA_MODER &= ~((3 << 16) | (3 << 18));
    GPIOA_MODER |=  ((2 << 16) | (2 << 18));
    GPIOA_AFRH &= ~((0xF << 0) | (0xF << 4));
    GPIOA_AFRH |=  ((1 << 0) | (1 << 4));

    GPIOA_PUPDR &= ~((3 << 16) | (3 << 18));
    GPIOA_PUPDR |=  ((1 << 16) | (1 << 18));

    TIM1_CCMR1 |= (1 << 0) | (1 << 8);
    TIM1_CCMR1 |= (3 << 4) | (3 << 12);
    TIM1_SMCR |= (3 << 0);
    TIM1_ARR = 0xFFFF;
    TIM1_CR1 |= (1 << 0);

    GPIOA_MODER &= ~(3 << 20);
    GPIOA_PUPDR &= ~(3 << 20);
    GPIOA_PUPDR |= (1 << 20);
}

int16_t Encoder_Read(void) {
    return (int16_t)TIM1_CNT;
}

uint8_t Encoder_Button_Read(void) {
    return !(GPIOA_IDR & (1 << 10));
}
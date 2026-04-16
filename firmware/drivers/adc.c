#include <stdint.h>
#include "stm32f411.h"

void ADC_Init(void) {
    RCC_AHB1ENR |= 1; // Clock GPIOA
    GPIOA_MODER |= 0xFF; // PA0-PA3 Analog
    GPIOA_MODER = (GPIOA_MODER & ~(0xFF << 8)) | (0x55 << 8); // PA4-PA7 Out
    RCC_APB2ENR |= (1 << 8); // Clock ADC1
    ADC_CR2 |= 1; // ADON
}

uint16_t ADC_Read(uint8_t channel) {
    ADC_SQR3 = channel;
    ADC_CR2 |= (1 << 30);
    while (!(ADC_SR & (1 << 1)));
    return (uint16_t)ADC_DR;
}
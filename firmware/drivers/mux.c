#include "stm32f411.h"
#include "mux.h"

void MUX_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    GPIOB->MODER &= ~(0xFFU << 24);
    GPIOB->MODER |= (0x55U << 24);

    GPIOB->OSPEEDR |= (0xFFU << 24);

}

void MUX_Select(uint8_t channel) {
    uint32_t reset_mask = (0x0FU << 28);

    uint32_t set_mask = ((channel & 0x0FU) << 12);

    GPIOB->BSRR = reset_mask | set_mask;
}
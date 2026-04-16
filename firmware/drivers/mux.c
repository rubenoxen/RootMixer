#include "stm32f411.h"
#include "mux.h"

void MUX_Init(void) {
    RCC_AHB1ENR |= (1 << 1); // Habilitar reloj GPIOB
    GPIOB_MODER &= ~(0xFFU << 24);
    GPIOB_MODER |= (0x55U << 24);
    GPIOB_OSPEEDR |= (0xFFU << 24);
}

void MUX_Select(uint8_t channel) {
    uint32_t reset_mask = (0x0FU << 28);
    uint32_t set_mask = ((channel & 0x0FU) << 12);
    GPIOB_BSRR = reset_mask | set_mask; // Uso de macro unificada
}
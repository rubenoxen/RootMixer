#include <stdint.h>
#include "stm32f411.h"

void USB_GPIO_Init(void) {
    RCC_AHB1ENR |= (1 << 0);

    GPIOA_MODER &= ~((3 << 22) | (3 << 24));
    GPIOA_MODER |= (2 << 22) | (2 << 24);

    GPIOA_AFRH &= ~((0xF << 12) | (0xF << 16));
    GPIOA_AFRH |= (10 << 12) | (10 << 16);

    GPIOA_OSPEEDR |= (3 << 22) | (3 << 24);
}

void USB_Core_Init(void) {
    RCC_AHB2ENR |= (1 << 7);

    USB_GUSBCFG |= (1 << 30) | (5 << 10);

    while (!(USB_GRSTCTL & (1 << 31)));

    USB_GRSTCTL |= (1 << 0);

    while (USB_GRSTCTL & (1 << 0));

    for (volatile int i = 0; i < 1000; i++);

    USB_GAHBCFG |= (1 << 0);

    USB_GAHBCFG &= ~(1 << 8);
}
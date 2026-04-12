#include <stdint.h>
#include "stm32f411.h"

void SystemClock_Config(void);

int main(void) {
    SystemClock_Config();

    USB_GPIO_Init();
    USB_Core_Init();
    USB_NVIC_Init();
    USB_Device_Init();

    RCC_AHB1ENR |= (1 << 2);

    GPIOC_MODER &= ~(3 << 26);
    GPIOC_MODER |= (1 << 26);

    GPIOC_BSRR = (1 << 29);

    while (1);

    return 0;
}
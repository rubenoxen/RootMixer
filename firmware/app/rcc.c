#include "stm32f411.h"

void SystemClock_Config(void) {
    // 1. Habilitar el Cristal Externo (HSE) de 25MHz
    RCC_CR |= (1 << 16);
    while (!(RCC_CR & (1 << 17))); // Esperar a que sea estable

    // 2. Configurar latencia de Flash para 96MHz (3 Wait States)
    FLASH_ACR = (1 << 8) | (1 << 9) | (1 << 10) | (3 << 0);

    // 3. Configuración del PLL para 96MHz (CPU) y 48MHz (USB)
    // Fuente HSE (bit 22 = 1). M=25, N=192, P=2, Q=4
    // VCO in = 25/25 = 1MHz. VCO out = 1*192 = 192MHz.
    // SYSCLK = 192/2 = 96MHz. USB = 192/4 = 48MHz exactos.
    RCC_PLLCFGR = (4 << 24) | (1 << 22) | (0 << 16) | (192 << 6) | 25;

    // 4. Encender PLL y esperar
    RCC_CR |= (1 << 24);
    while (!(RCC_CR & (1 << 25)));

    // 5. Seleccionar PLL como reloj del sistema y poner APB1 a 48MHz (div 2)
    RCC_CFGR |= (4 << 10) | (2 << 0);
    while ((RCC_CFGR & (3 << 2)) != (2 << 2));
}
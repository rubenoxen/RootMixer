#include "stm32f411.h"
#include "systick.h"
#include "tusb.h"
#include <stdbool.h>
#include <stddef.h>

// --- DEPENDENCIAS DEL ENLAZADOR (LINKER) ---
uint32_t SystemCoreClock = 96000000;

uint32_t board_millis(void) {
    return Get_Millis();
}
uint32_t tusb_time_millis_api(void) {
    return Get_Millis();
}

void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    while (n--) *d++ = *s++;
    return dest;
}

void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;
    while (n--) *p++ = (uint8_t)c;
    return s;
}

size_t strlen(const char *s) {
    const char *p = s;
    while (*p) p++;
    return p - s;
}

// --- CONFIGURACIÓN DE RELOJES (La que faltaba) ---
void SystemClock_Config(void) {
    // 1. Encender el HSI
    RCC_CR |= 1; 
    while (!(RCC_CR & 2)); 
    
    // 2. Resetear buses
    RCC_CFGR = 0; 
    
    // 3. Latencia Flash para 96MHz
    FLASH_ACR = (1 << 8) | (1 << 9) | (1 << 10) | (3 << 0);
    
    // 4. Mates del PLL (96MHz CPU, 48MHz USB)
    RCC_PLLCFGR = (4 << 24) | (0 << 22) | (0 << 16) | (192 << 6) | 16;
    
    // 5. Encender PLL
    RCC_CR |= (1 << 24);
    while (!(RCC_CR & (1 << 25)));
    
    // 6. Enganchar PLL al bus principal
    RCC_CFGR = (4 << 10) | 2;
    while ((RCC_CFGR & (3 << 2)) != (2 << 2));
}

// --- INICIALIZACIÓN HARDWARE USB ---
void USB_Hardware_Init(void) {
    RCC_AHB1ENR |= (1 << 0); 
    RCC_AHB2ENR |= (1 << 7); 
    
    GPIOA_MODER = (GPIOA_MODER & ~(0xF << 22)) | (0xA << 22);
    GPIOA_AFRH = (GPIOA_AFRH & ~(0xFF << 12)) | (0xAA << 12);
    GPIOA_OSPEEDR |= (0xF << 22);

    NVIC_ISER2 |= (1 << 3); 
}

void OTG_FS_IRQHandler(void) {
    tud_int_handler(0);
}

// --- MAIN ---
int main(void) {
    SystemClock_Config();
    SysTick_Init();

    RCC_AHB1ENR |= (1 << 2); 
    GPIOC_MODER = (GPIOC_MODER & ~(3 << 26)) | (1 << 26); 

    USB_Hardware_Init();
    tusb_init(); 

    uint32_t last_blink = Get_Millis();
    bool led_on = false;

    while (1) {
        tud_task(); 

        if (Get_Millis() - last_blink >= 500) {
            last_blink = Get_Millis();
            if (led_on) {
                GPIOC_BSRR = (1 << 29); // Apagar
            } else {
                GPIOC_BSRR = (1 << 13); // Encender
            }
            led_on = !led_on;
        }
    }
}

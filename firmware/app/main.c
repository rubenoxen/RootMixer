#include "stm32f411.h"
#include "systick.h"
#include "tusb.h"
#include "buttons.h"
#include <stdbool.h>
#include <stddef.h>

// --- DEPENDENCIAS DEL ENLAZADOR ---
uint32_t SystemCoreClock = 96000000;
uint32_t board_millis(void) { return Get_Millis(); }
uint32_t tusb_time_millis_api(void) { return Get_Millis(); }
void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dest; const uint8_t *s = (const uint8_t *)src;
    while (n--) *d++ = *s++; return dest;
}
void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;
    while (n--) *p++ = (uint8_t)c; return s;
}
size_t strlen(const char *s) {
    const char *p = s; while (*p) p++; return p - s;
}

// --- CONFIGURACIÓN DE RELOJES ---
void SystemClock_Config(void) {
    RCC_CR |= 1; while (!(RCC_CR & 2)); 
    RCC_CFGR = 0; 
    FLASH_ACR = (1 << 8) | (1 << 9) | (1 << 10) | (3 << 0);
    RCC_PLLCFGR = (4 << 24) | (0 << 22) | (0 << 16) | (192 << 6) | 16;
    RCC_CR |= (1 << 24); while (!(RCC_CR & (1 << 25)));
    RCC_CFGR = (4 << 10) | 2; while ((RCC_CFGR & (3 << 2)) != (2 << 2));
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
void OTG_FS_IRQHandler(void) { tud_int_handler(0); }

// --- MAIN ---
int main(void) {
    SystemClock_Config();
    SysTick_Init();
    Buttons_Init();

    // LED de control (PC13)
    RCC_AHB1ENR |= (1 << 2); 
    GPIOC_MODER = (GPIOC_MODER & ~(3 << 26)) | (1 << 26); 

    USB_Hardware_Init();
    tusb_init(); 

    uint32_t last_blink = Get_Millis();
    bool led_on = false;

    // Variables para controlar el estado del botón sin bloquear el código
    bool last_btn_state = false;
    uint32_t last_btn_time = 0;

    while (1) {
        // 1. Mantener vivo el protocolo USB (VITAL)
        tud_task(); 

        // 2. Parpadeo de supervivencia del LED (cada 500ms)
        if (Get_Millis() - last_blink >= 500) {
            last_blink = Get_Millis();
            if (led_on) GPIOC_BSRR = (1 << 29); 
            else GPIOC_BSRR = (1 << 13); 
            led_on = !led_on;
        }

        // 3. Leer el botón con debounce de 50ms
        if (Get_Millis() - last_btn_time >= 50) {
            bool current_btn_state = Button_Is_Pressed();
            
            // Si el estado ha cambiado
            if (current_btn_state != last_btn_state) {
                last_btn_state = current_btn_state;
                last_btn_time = Get_Millis();
                
                // Si la conexión USB está lista para enviar datos
                if (tud_midi_mounted()) {
                    uint8_t midi_msg[3];
                    if (current_btn_state) {
                        // Botón pulsado: Enviar Note ON (Canal 1, Nota 60/C4, Velocidad 127)
                        midi_msg[0] = 0x90; 
                        midi_msg[1] = 60;   
                        midi_msg[2] = 127;  
                    } else {
                        // Botón soltado: Enviar Note OFF (Canal 1, Nota 60/C4, Velocidad 0)
                        midi_msg[0] = 0x80; 
                        midi_msg[1] = 60;   
                        midi_msg[2] = 0;    
                    }
                    // Enviar los 3 bytes al cable
                    tud_midi_stream_write(0, midi_msg, 3);
                }
            }
        }
    }
}

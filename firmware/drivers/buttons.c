#include "stm32f411.h"
#include "buttons.h"
#include "systick.h"

void Buttons_Init(void) {
    // Habilitar reloj del puerto A (ya lo hace el USB, pero es buena práctica asegurarlo)
    RCC_AHB1ENR |= (1 << 0);
    
    // Configurar PA0 como entrada (00)
    GPIOA_MODER &= ~(3 << 0);
    
    // Activar resistencia Pull-Up interna en PA0 (01)
    GPIOA_PUPDR &= ~(3 << 0);
    GPIOA_PUPDR |= (1 << 0);
}

bool Button_Is_Pressed(void) {
    // Devuelve true si el pin PA0 está a 0V (botón pulsado)
    return !(GPIOA_IDR & (1 << 0));
}

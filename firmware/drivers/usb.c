#include "stm32f411.h"

// ============================================================================
// 1. INICIALIZACIÓN DEL NÚCLEO FÍSICO (PHY Y CLOCKS)
// ============================================================================
void USB_Core_Init(void) {
    // 1. Dar energía a los periféricos necesarios
    RCC_AHB1ENR |= (1 << 0); // Encender reloj del Puerto A (GPIO)
    RCC_AHB2ENR |= (1 << 7); // Encender reloj del núcleo USB OTG_FS

    // 2. Configurar pines físicos PA11 (D-) y PA12 (D+) como antena USB
    // Limpiar modo actual de PA11 y PA12
    GPIOA_MODER &= ~((3 << 22) | (3 << 24));
    // Establecer PA11 y PA12 en Alternate Function (10 en binario)
    GPIOA_MODER |=  ((2 << 22) | (2 << 24));

    // Configurar velocidad máxima (Very High Speed) para no distorsionar los 48MHz
    GPIOA_OSPEEDR |= ((3 << 22) | (3 << 24));

    // Mapear la Función Alternativa 10 (AF10) a los pines PA11 y PA12
    // Se usa AFRH (High) porque los pines son mayores de 7
    GPIOA_AFRH &= ~((0xF << 12) | (0xF << 16)); // Limpiar el bloque
    GPIOA_AFRH |=  ((10  << 12) | (10  << 16)); // Asignar AF10 (0xA)

    // 3. Configuración interna del Core USB
    // Forzar Device Mode (bit 30) y configurar el Turnaround Time (bits 10-13)
    USB_GUSBCFG |= (1 << 30) | (5 << 10);

    // Reset por software del núcleo USB
    while (!(USB_GRSTCTL & (1 << 31))); // Esperar a que el AHB master esté libre
    USB_GRSTCTL |= 1;                   // Disparar Soft Reset
    while (USB_GRSTCTL & 1);            // Esperar a que termine el reset

    // Habilitar interrupciones globales del USB
    USB_GAHBCFG |= 1;

    // 4. EL MOTOR FÍSICO: Encender el PHY y quitar el detector de VBUS
    // PWRDWN a 1 (bit 16) enciende el transceptor. NOVBUSSENS a 1 (bit 21) ignora PA9.
    USB_GCCFG |= (1 << 16) | (1 << 21);
}

// ============================================================================
// 2. INICIALIZACIÓN DEL DISPOSITIVO LÓGICO Y PROTOCOLO
// ============================================================================
void USB_Device_Init(void) {
    // Configurar velocidad (Full Speed = 3)
    USB_DCFG |= 3;

    // Configurar la memoria RAM compartida (FIFOs) para los Endpoints
    USB_GRXFSIZ = 64;                 // RX FIFO (Recepción general)
    USB_DIEPTXF0 = (64 << 16) | 64;   // TX0 FIFO (Control, tamaño y offset)
    USB_DIEPTXF1 = (64 << 16) | 128;  // TX1 FIFO (Datos, tamaño y offset)

    // Limpiar cualquier interrupción fantasma pendiente
    USB_GINTSTS = 0xFFFFFFFF;

    // MÁSCARA DE INTERRUPCIONES (Abrir los oídos de la CPU)
    // Bit 4:  RXFLVLM (Escuchar paquetes entrantes del PC)
    // Bit 11: USBSUSPM (Modo suspensión)
    // Bit 12: USBRST (Reset del bus USB)
    // Bit 13: ENUMDNEM (Fin de la enumeración)
    // Bit 18: IEPINT (Interrupción de Endpoint de Entrada)
    // Bit 19: OEPINT (Interrupción de Endpoint de Salida)
    USB_GINTMSK |= (1 << 12) | (1 << 13) | (1 << 11) | (1 << 19) | (1 << 18) | (1 << 4);

    // EL BOTÓN DE CONEXIÓN: Desactivar el Soft Disconnect (SDIS - Bit 1)
    // Al poner este bit a 0, la resistencia pull-up interna inyecta 3.3V en D+
    // Esto es lo que le grita al Kernel de Linux "¡Estoy aquí!"
    USB_DCTL &= ~(1 << 1);
}

// ============================================================================
// TUS DESCRIPTORES Y HANDLERS DEBEN IR AQUÍ ABAJO
// (DeviceDescriptor, ConfigDescriptor, OTG_FS_IRQHandler, etc.)
// ============================================================================
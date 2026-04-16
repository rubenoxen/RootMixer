#include "stm32f4xx.h"
#include <stdint.h>

// Asumimos que tienes tus funciones de inicialización
extern void SystemClock_Config(void);
extern void ADC1_Init(void);
extern uint16_t ADC1_Read(void);
extern void MUX_Init(void);
extern void MUX_Select(uint8_t channel);

int main(void) {
    SystemClock_Config();
    MUX_Init();
    ADC1_Init(); // Debe configurar PA0 (Canal 0 del ADC)

    // Variables para almacenar el estado del hardware
    uint16_t raw_fader = 0;
    uint16_t raw_pots[4] = {0, 0, 0, 0};
    uint8_t  mute_pressed = 0;

    while (1) {
        // Escaneamos solo las 6 direcciones que has ruteado en tu PCB
        for (uint8_t addr = 0; addr <= 5; addr++) {

            // 1. Cambiar la dirección del hardware (atómico)
            MUX_Select(addr);

            // 2. Settling Time (Retardo bloqueante mínimo)
            // Un bucle vacío de 20 iteraciones consume ~80 ciclos de reloj.
            // A 100MHz, son unos 800 nanosegundos. Tiempo de sobra para el MUX.
            for(volatile uint32_t wait = 0; wait < 20; wait++) {
                __asm__("nop"); // Instrucción ensamblador: No Operation
            }

            // 3. Disparar el ADC y leer (Asumiendo ADC por polling)
            uint16_t adc_value = ADC1_Read();

            // 4. Mapear el valor según la arquitectura física de tu placa hija
            switch (addr) {
                case 0:
                    raw_fader = adc_value;
                    break;
                case 1:
                    raw_pots[0] = adc_value; // POT 1 (Arriba)
                    break;
                case 2:
                    raw_pots[3] = adc_value; // POT 4
                    break;
                case 3:
                    raw_pots[2] = adc_value; // POT 3
                    break;
                case 4:
                    raw_pots[1] = adc_value; // POT 2
                    break;
                case 5:
                    // Botón Mute (Pull-up físico de 10k a 3.3V)
                    // Lógica invertida: Si cae cerca de 0, está pulsado
                    mute_pressed = (adc_value < 500) ? 1 : 0;
                    break;
            }
        }

        // --- AQUÍ VA TU LÓGICA MIDI O UI ---
        // (Evaluar si los valores han cambiado más de un cierto umbral
        // para enviar datos por USB y no saturar el bus)

        // Retardo del escáner principal (ej. escanear a 1 kHz)
        for(volatile uint32_t i = 0; i < 10000; i++);
    }
}
#include "stm32f411.h"
#include "systick.h"
#include "ssd1306.h"
#include "encoder.h"
#include "buttons.h"
#include "i2c.h"
#include "adc.h"
#include "mux.h"
#include "usb.h"
#include "rcc.h"   

// Estructura global del Mixer
typedef struct {
    uint8_t faders[4];       // 4 Canales
    uint8_t pots[4][4];      // [Canal 0-3][Pote 0-3]
    uint8_t mutes;           // Máscara de bits para los 4 botones
    int16_t encoder_val;     // Valor del KY-040
} MixerState_t;

MixerState_t mixer = {0};

// Filtro anti-ruido analógico e inyección USB MIDI
void Process_Analog_To_MIDI(uint16_t raw_adc, uint8_t *current_midi, uint8_t cc_num) {
    uint8_t new_midi = raw_adc >> 5; // Convertir 12-bit (0-4095) a 7-bit MIDI (0-127)

    // Solo enviamos por USB si el valor ha cambiado (evita saturar el bus)
    if (new_midi != *current_midi) {
        *current_midi = new_midi;
        USB_Send_MIDI_CC(cc_num, new_midi);
    }
}

int main(void) {
    // 1. Inicialización de Relojes y Temporizadores
    SystemClock_Config();
    SysTick_Init();

    // 2. Inicialización de Hardware I/O
    MUX_Init();
    ADC_Init();
    Encoder_Init();
    Buttons_Init();

    // 3. Inicialización de Comunicaciones y UI
    I2C_Init();
    SSD1306_Init();

    // 4. Inicialización del Stack USB (Puro Bare Metal)
    USB_GPIO_Init();
    USB_Core_Init();
    USB_Device_Init();
    USB_NVIC_Init();

    // Timestamps para el scheduler no bloqueante
    uint32_t last_analog_scan = Get_Millis();
    uint32_t last_ui_update = Get_Millis();

    // Feedback visual de arranque
    SSD1306_Clear();
    // Aquí puedes dibujar tu logo o líneas iniciales.
    SSD1306_UpdateScreen();

    while (1) {
        uint32_t current_time = Get_Millis();

        // --- TASK 1: MOTOR DE ESCANEO ANALÓGICO (Frecuencia: ~2ms) ---
        if (current_time - last_analog_scan >= 2) {
            last_analog_scan = current_time;
            uint8_t current_mutes = 0;

            // Recorrer las 6 direcciones del multiplexor (0 a 5)
            for (uint8_t addr = 0; addr <= 5; addr++) {

                MUX_Select(addr); // Operación atómica vía BSRR

                // Settling time: Retardo microscópico para estabilizar la capacitancia de la PCB
                for(volatile uint32_t wait = 0; wait < 30; wait++) { __asm__("nop"); }

                // Leer simultáneamente los 4 canales del ADC (Las 4 placas hijas)
                for (uint8_t ch = 0; ch < 4; ch++) {
                    uint16_t adc_val = ADC_Read(ch); // Lee PA0, PA1, PA2, PA3

                    switch (addr) {
                        case 0: // Fader (Asignamos CC 10 al 13)
                            Process_Analog_To_MIDI(adc_val, &mixer.faders[ch], 10 + ch);
                            break;
                        case 1: // POT 1 - Agudos (CC 20, 24, 28, 32)
                            Process_Analog_To_MIDI(adc_val, &mixer.pots[ch][0], 20 + (ch * 4) + 0);
                            break;
                        case 4: // POT 2 - Medios Altos (CC 21, 25, 29, 33)
                            Process_Analog_To_MIDI(adc_val, &mixer.pots[ch][1], 20 + (ch * 4) + 1);
                            break;
                        case 3: // POT 3 - Medios Bajos (CC 22, 26, 30, 34)
                            Process_Analog_To_MIDI(adc_val, &mixer.pots[ch][2], 20 + (ch * 4) + 2);
                            break;
                        case 2: // POT 4 - Graves (CC 23, 27, 31, 35)
                            Process_Analog_To_MIDI(adc_val, &mixer.pots[ch][3], 20 + (ch * 4) + 3);
                            break;
                        case 5: // Botones Mute (Pull-up 10k, se van a GND al pulsar)
                            if (adc_val < 500) {
                                current_mutes |= (1 << ch);
                            }
                            break;
                    }
                }
            }

            // Procesar flancos de los botones de Mute
            if (current_mutes != mixer.mutes) {
                for (uint8_t ch = 0; ch < 4; ch++) {
                    uint8_t was_pressed = (mixer.mutes & (1 << ch));
                    uint8_t is_pressed = (current_mutes & (1 << ch));

                    if (is_pressed && !was_pressed) {
                        USB_Send_MIDI_CC(60 + ch, 127); // Note ON / CC ON
                    } else if (!is_pressed && was_pressed) {
                        USB_Send_MIDI_CC(60 + ch, 0);   // Note OFF / CC OFF
                    }
                }
                mixer.mutes = current_mutes;
            }
        }

        // --- TASK 2: INTERFAZ DE USUARIO (Frecuencia: ~50ms) ---
        if (current_time - last_ui_update >= 50) {
            last_ui_update = current_time;

            int16_t new_enc = Encoder_Read();

            // Si mueves la ruleta KY-040
            if (new_enc != mixer.encoder_val) {
                // Determinar dirección para MIDI (Encoder Relativo)
                if (new_enc > mixer.encoder_val) {
                    USB_Send_MIDI_CC(70, 1); // Giro derecha
                } else {
                    USB_Send_MIDI_CC(70, 127); // Giro izquierda
                }

                mixer.encoder_val = new_enc;

                // Actualizar OLED (Placeholder de diseño)
                SSD1306_Clear();
                // SSD1306_DrawFilledRect(...) -> Aquí dibujarás las barras de volumen o menú
                SSD1306_UpdateScreen();
            }

            // Leer el clic de la ruleta
            if (Encoder_Button_Read()) {
                USB_Send_MIDI_CC(71, 127); // Botón central pulsado
            }
        }
    }
}
#include <stdint.h>
#include "stm32f411.h"
#include "systick.h"
#include "i2c.h"
#include "ssd1306.h"
#include "buttons.h"
#include "encoder.h"

/* --- ESTRUCTURA DE DATOS DEL SISTEMA --- */
typedef struct {
    uint8_t faders[4];
    uint8_t mutes;      // Máscara de bits para los botones
    uint8_t pan;        // 0 a 127
    uint8_t is_dirty;   // Flag para refrescar la pantalla
} MixerState_t;

// Instancia global única del estado de la mesa
MixerState_t mixer = {
    .faders = {0, 0, 0, 0},
    .mutes = 0,
    .pan = 64,          // Centro
    .is_dirty = 1       // Forzar primer dibujado
};

void Mixer_App_Init(void) {
    SystemClock_Config();
    SysTick_Init();
    USB_GPIO_Init();
    USB_Core_Init();
    USB_NVIC_Init();
    USB_Device_Init();
    ADC_Init();
    Buttons_Init();
    Encoder_Init();
    I2C_Init();
    SSD1306_Init();
}

/* El motor gráfico ahora recibe el estado por referencia (puntero),
   esto lo hace modular y reutilizable */
void Draw_Master_UI(MixerState_t *state) {
    SSD1306_Clear();

    for (uint8_t i = 0; i < 4; i++) {
        uint8_t bar_h = (state->faders[i] * 50) / 127;
        uint8_t x_pos = (i * 30) + 5;
        SSD1306_DrawFilledRect(x_pos, 50 - bar_h, 20, bar_h, 1);

        if (state->mutes & (1 << i)) {
            SSD1306_DrawFilledRect(x_pos + 5, 54, 10, 10, 1);
        }
    }

    uint8_t pan_x = (state->pan * 128) / 127;
    SSD1306_DrawFilledRect(0, 0, pan_x, 4, 1);

    SSD1306_UpdateScreen();
}

int main(void) {
    Mixer_App_Init();

    const uint8_t cc_faders[4] = {7, 8, 9, 10};
    const uint8_t cc_buttons[4] = {20, 21, 22, 23};
    const uint8_t cc_pan = 11;

    // Caché de hardware para detectar cambios
    uint8_t last_faders[4] = {255, 255, 255, 255};
    uint8_t last_buttons = 255;
    int16_t last_encoder = Encoder_Read();

    uint32_t last_scan_time = 0;
    uint32_t last_ui_time = 0;

    while (1) {
        uint32_t current_time = Get_Millis();

        // --- TAREA 1: Escáner de Hardware (1000 Hz) ---
        if (current_time - last_scan_time >= 1) {
            last_scan_time = current_time;

            // 1. Faders
            for (uint8_t i = 0; i < 4; i++) {
                uint8_t val = (ADC_Read(i) >> 5);
                if (val != last_faders[i]) {
                    USB_Send_MIDI_CC(cc_faders[i], val);
                    mixer.faders[i] = val;
                    last_faders[i] = val;
                    mixer.is_dirty = 1;
                }
            }

            // 2. Botones Mute
            uint8_t current_btn = Buttons_Read();
            if (current_btn != last_buttons) {
                for (uint8_t i = 0; i < 4; i++) {
                    uint8_t was_pressed = (last_buttons & (1 << i));
                    uint8_t is_pressed = (current_btn & (1 << i));

                    if (is_pressed && !was_pressed) {
                        USB_Send_MIDI_CC(cc_buttons[i], 127);
                    } else if (!is_pressed && was_pressed) {
                        USB_Send_MIDI_CC(cc_buttons[i], 0);
                    }
                }
                mixer.mutes = current_btn;
                last_buttons = current_btn;
                mixer.is_dirty = 1;
            }

            // 3. Encoder (Paneo)
            int16_t current_enc = Encoder_Read();
            if (current_enc != last_encoder) {
                int16_t delta = (current_enc - last_encoder) / 4;
                if (delta != 0) {
                    int16_t new_pan = mixer.pan + delta;
                    if (new_pan > 127) new_pan = 127;
                    if (new_pan < 0) new_pan = 0;

                    if (new_pan != mixer.pan) {
                        mixer.pan = new_pan;
                        USB_Send_MIDI_CC(cc_pan, mixer.pan);
                        mixer.is_dirty = 1;
                    }
                    last_encoder = current_enc;
                }
            }
        }

        // --- TAREA 2: Refresco de Pantalla (30 FPS) ---
        if ((current_time - last_ui_time >= 33) && mixer.is_dirty) {
            last_ui_time = current_time;
            Draw_Master_UI(&mixer); // Pasamos el estado
            mixer.is_dirty = 0;
        }
    }
}
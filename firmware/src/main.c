#include <stdint.h>
#include "stm32f411.h"
#include "systick.h"
#include "i2c.h"
#include "ssd1306.h"
#include "buttons.h"

// Variables de Estado de la App
uint8_t fader_state[4] = {0};
uint8_t button_state = 0;
uint8_t ui_dirty = 1;

void Mixer_App_Init(void) {
    SystemClock_Config();
    SysTick_Init();
    USB_GPIO_Init();
    USB_Core_Init();
    USB_NVIC_Init();
    USB_Device_Init();
    ADC_Init();
    Buttons_Init();
    I2C_Init();
    SSD1306_Init();
}

void Draw_Master_UI(void) {
    SSD1306_Clear();
    for (uint8_t i = 0; i < 4; i++) {
        uint8_t bar_h = (fader_state[i] * 50) / 127;
        uint8_t x_pos = (i * 30) + 5;
        SSD1306_DrawFilledRect(x_pos, 50 - bar_h, 20, bar_h, 1);

        if (button_state & (1 << i)) {
            SSD1306_DrawFilledRect(x_pos + 5, 54, 10, 10, 1);
        }
    }
    SSD1306_UpdateScreen();
}

int main(void) {
    Mixer_App_Init();

    uint8_t cc_faders[4] = {7, 8, 9, 10};
    uint8_t cc_buttons[4] = {20, 21, 22, 23};
    uint8_t last_faders[4] = {255, 255, 255, 255};
    uint8_t last_buttons = 255;

    // Controladores de tiempo (Time tracking)
    uint32_t last_scan_time = 0;
    uint32_t last_ui_time = 0;

    while (1) {
        uint32_t current_time = Get_Millis();

        // TAREA 1: Escanear Hardware (1000 veces por segundo / 1ms)
        if (current_time - last_scan_time >= 1) {
            last_scan_time = current_time;

            // Escáner ADC
            for (uint8_t i = 0; i < 4; i++) {
                uint8_t val = (ADC_Read(i) >> 5);
                if (val != last_faders[i]) {
                    USB_Send_MIDI_CC(cc_faders[i], val);
                    fader_state[i] = val;
                    last_faders[i] = val;
                    ui_dirty = 1;
                }
            }

            // Escáner Digital
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
                button_state = current_btn;
                last_buttons = current_btn;
                ui_dirty = 1;
            }
        }

        // TAREA 2: Refresco de Pantalla (Max 30 FPS / 33ms)
        if ((current_time - last_ui_time >= 33) && ui_dirty) {
            last_ui_time = current_time;
            Draw_Master_UI();
            ui_dirty = 0;
        }
    }
}
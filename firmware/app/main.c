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

typedef struct {
    uint8_t faders[4];
    uint8_t pots[4][4];
    uint8_t mutes;
    int16_t encoder_val;
} MixerState_t;

MixerState_t mixer = {0};

void Process_Analog_To_MIDI(uint16_t raw_adc, uint8_t *current_midi, uint8_t cc_num) {
    uint8_t new_midi = raw_adc >> 5;
    if (new_midi != *current_midi) {
        *current_midi = new_midi;
        USB_Send_MIDI_CC(cc_num, new_midi);
    }
}

int main(void) {
    // 0. EL CEPO FÍSICO: Empezamos con el LED apagado (PC13 a HIGH)
    RCC_AHB1ENR |= (1 << 2);
    GPIOC_MODER = (GPIOC_MODER & ~(3 << 26)) | (1 << 26);
    GPIOC_BSRR = (1 << 13);

    // 1. Relojes
    SystemClock_Config();
    SysTick_Init();

    // 2. Hardware I/O (Descomentado pero ignorado para no bloquear)
    MUX_Init();
    ADC_Init();
    Encoder_Init();
    Buttons_Init();

    // 3. USB (Bare Metal)
    USB_Core_Init();
    USB_Device_Init();

    // Activar interrupción OTG_FS (IRQ 67)
    NVIC_ISER2 |= (1 << 3);

    uint32_t last_analog_scan = Get_Millis();
    uint32_t last_ui_update = Get_Millis();

    while (1) {
        uint32_t current_time = Get_Millis();

        if (current_time - last_analog_scan >= 2) {
            last_analog_scan = current_time;
            uint8_t current_mutes = 0;

            for (uint8_t addr = 0; addr <= 5; addr++) {
                MUX_Select(addr);
                for(volatile uint32_t wait = 0; wait < 30; wait++) { __asm__("nop"); }

                for (uint8_t ch = 0; ch < 4; ch++) {
                    uint16_t adc_val = ADC_Read(ch);
                    switch (addr) {
                        case 0: Process_Analog_To_MIDI(adc_val, &mixer.faders[ch], 10 + ch); break;
                        case 1: Process_Analog_To_MIDI(adc_val, &mixer.pots[ch][0], 20 + (ch * 4) + 0); break;
                        case 4: Process_Analog_To_MIDI(adc_val, &mixer.pots[ch][1], 20 + (ch * 4) + 1); break;
                        case 3: Process_Analog_To_MIDI(adc_val, &mixer.pots[ch][2], 20 + (ch * 4) + 2); break;
                        case 2: Process_Analog_To_MIDI(adc_val, &mixer.pots[ch][3], 20 + (ch * 4) + 3); break;
                        case 5: if (adc_val < 500) { current_mutes |= (1 << ch); } break;
                    }
                }
            }

            if (current_mutes != mixer.mutes) {
                for (uint8_t ch = 0; ch < 4; ch++) {
                    uint8_t was_pressed = (mixer.mutes & (1 << ch));
                    uint8_t is_pressed = (current_mutes & (1 << ch));

                    if (is_pressed && !was_pressed) USB_Send_MIDI_CC(60 + ch, 127);
                    else if (!is_pressed && was_pressed) USB_Send_MIDI_CC(60 + ch, 0);
                }
                mixer.mutes = current_mutes;
            }
        }

        if (current_time - last_ui_update >= 50) {
            last_ui_update = current_time;
            int16_t new_enc = Encoder_Read();

            if (new_enc != mixer.encoder_val) {
                if (new_enc > mixer.encoder_val) USB_Send_MIDI_CC(70, 1);
                else USB_Send_MIDI_CC(70, 127);
                mixer.encoder_val = new_enc;
            }

            if (Encoder_Button_Read()) {
                USB_Send_MIDI_CC(71, 127);
            }
        }
    }
}
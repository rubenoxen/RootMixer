#include <stdint.h>
#include "stm32f411.h"

int main(void) {
    SystemClock_Config();
    USB_GPIO_Init();
    USB_Core_Init();
    USB_NVIC_Init();
    USB_Device_Init();
    ADC_Init();

    uint8_t last[4] = {255, 255, 255, 255};
    uint8_t cc[4] = {7, 8, 9, 10};

    while (1) {
        for (uint8_t i = 0; i < 4; i++) {
            uint8_t val = (ADC_Read(i) >> 5);
            if (val != last[i]) {
                USB_Send_MIDI_CC(cc[i], val);
                last[i] = val;
            }
        }
        for (volatile int d = 0; d < 10000; d++);
    }
}
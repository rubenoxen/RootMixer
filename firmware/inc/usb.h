#ifndef USB_H
#define USB_H

#include <stdint.h>

void USB_GPIO_Init(void);
void USB_Core_Init(void);
void USB_Device_Init(void);
void USB_NVIC_Init(void);
void USB_Write_EP0(const uint8_t *data, uint32_t len);
void USB_Send_MIDI_CC(uint8_t cc_num, uint8_t value);

#endif
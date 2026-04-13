#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>

void SSD1306_Init(void);
void SSD1306_Clear(void);
void SSD1306_DrawFilledRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);
void SSD1306_UpdateScreen(void);

#endif
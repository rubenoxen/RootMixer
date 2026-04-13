#include "ssd1306.h"
#include "i2c.h"
#include <string.h>

#define OLED_ADDR 0x78

static uint8_t SSD1306_Buffer[1024];

static void OLED_Cmd(uint8_t cmd) {
    I2C_Start();
    I2C_Address(OLED_ADDR);
    I2C_Write(0x00);
    I2C_Write(cmd);
    I2C_Stop();
}

void SSD1306_Init(void) {
    const uint8_t init_cmds[] = {
        0xAE, 0x20, 0x00, 0xC8, 0x81, 0xCF, 0xA1, 0xA6, 0xA8, 0x3F,
        0xD3, 0x00, 0xD5, 0x80, 0xD9, 0xF1, 0xDA, 0x12, 0x8D, 0x14, 0xAF
    };
    for(uint32_t i = 0; i < sizeof(init_cmds); i++) {
        OLED_Cmd(init_cmds[i]);
    }
    SSD1306_Clear();
    SSD1306_UpdateScreen();
}

void SSD1306_Clear(void) {
    for (int i = 0; i < 1024; i++) {
        SSD1306_Buffer[i] = 0x00;
    }
}

void SSD1306_DrawFilledRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color) {
    for (uint8_t i = 0; i < w; i++) {
        for (uint8_t j = 0; j < h; j++) {
            uint8_t px = x + i;
            uint8_t py = y + j;
            if (px >= 128 || py >= 64) continue;

            uint16_t index = px + (py / 8) * 128;
            if (color) {
                SSD1306_Buffer[index] |= (1 << (py % 8));
            } else {
                SSD1306_Buffer[index] &= ~(1 << (py % 8));
            }
        }
    }
}

void SSD1306_UpdateScreen(void) {
    OLED_Cmd(0x21); OLED_Cmd(0); OLED_Cmd(127);
    OLED_Cmd(0x22); OLED_Cmd(0); OLED_Cmd(7);

    I2C_Start();
    I2C_Address(OLED_ADDR);
    I2C_Write(0x40);
    for(uint16_t i = 0; i < 1024; i++) {
        I2C_Write(SSD1306_Buffer[i]);
    }
    I2C_Stop();
}
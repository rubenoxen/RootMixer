#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

void Encoder_Init(void);
int16_t Encoder_Read(void);
uint8_t Encoder_Button_Read(void); // Para leer el "clic" de la ruleta KY-040

#endif
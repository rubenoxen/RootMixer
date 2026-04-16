#include "stm32f411.h"
#include "i2c.h"

void I2C_Init(void) {
    RCC_AHB1ENR |= (1 << 1);
    RCC_APB1ENR |= (1 << 21);

    GPIOB_MODER |= (2 << 12) | (2 << 14);

    GPIOB_OTYPER |= (1 << 6) | (1 << 7);

    GPIOB_AFRL |= (4 << 24) | (4 << 28);

    I2C_CR1 |= (1 << 15);
    I2C_CR1 &= ~(1 << 15);

    I2C_CR2 |= 42;
    I2C_CCR = 210;
    I2C_TRISE = 43;

    I2C_CR1 |= (1 << 0);
}

void I2C_Start(void) {
    I2C_CR1 |= (1 << 8);
    while (!(I2C_SR1 & (1 << 0)));
}

void I2C_Address(uint8_t address) {
    I2C_DR = address;
    while (!(I2C_SR1 & (1 << 1)));
    (void)I2C_SR2;
}

void I2C_Write(uint8_t data) {
    while (!(I2C_SR1 & (1 << 7)));
    I2C_DR = data;
    while (!(I2C_SR1 & (1 << 2)));
}

void I2C_Stop(void) {
    I2C_CR1 |= (1 << 9);
}
#ifndef STM32F411_H
#define STM32F411_H

#include <stdint.h>

/* --- MEMORY MAP --- */
#define FLASH_BASE_REG  0x40023C00
#define RCC_BASE        0x40023800
#define GPIOA_BASE      0x40020000
#define GPIOB_BASE      0x40020400
#define GPIOC_BASE      0x40020800
#define USB_OTG_FS_BASE 0x50000000
#define ADC1_BASE       0x40012000
#define TIM1_BASE       0x40010000
#define NVIC_ISER2      *((volatile uint32_t *)(0xE000E108))

/* --- RCC & FLASH --- */
#define FLASH_ACR       *((volatile uint32_t *)(FLASH_BASE_REG + 0x00))
#define RCC_CR          *((volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_PLLCFGR     *((volatile uint32_t *)(RCC_BASE + 0x04))
#define RCC_CFGR        *((volatile uint32_t *)(RCC_BASE + 0x08))
#define RCC_AHB1ENR     *((volatile uint32_t *)(RCC_BASE + 0x30))
#define RCC_AHB2ENR     *((volatile uint32_t *)(RCC_BASE + 0x34))
#define RCC_APB1ENR     *((volatile uint32_t *)(RCC_BASE + 0x40))
#define RCC_APB2ENR     *((volatile uint32_t *)(RCC_BASE + 0x44))

/* --- GPIO --- */
#define GPIOA_MODER     *((volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR       *((volatile uint32_t *)(GPIOA_BASE + 0x14))
#define GPIOA_OSPEEDR   *((volatile uint32_t *)(GPIOA_BASE + 0x08))
#define GPIOA_AFRH      *((volatile uint32_t *)(GPIOA_BASE + 0x24))
#define GPIOB_MODER     *((volatile uint32_t *)(GPIOB_BASE + 0x00))
#define GPIOB_IDR       *((volatile uint32_t *)(GPIOB_BASE + 0x10))
#define GPIOB_PUPDR     *((volatile uint32_t *)(GPIOB_BASE + 0x0C))
#define GPIOC_MODER     *((volatile uint32_t *)(GPIOC_BASE + 0x00))
#define GPIOC_BSRR      *((volatile uint32_t *)(GPIOC_BASE + 0x18))

/* --- USB OTG FS GLOBAL --- */
#define USB_GAHBCFG     *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x008))
#define USB_GUSBCFG     *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x00C))
#define USB_GRSTCTL     *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x010))
#define USB_GINTSTS     *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x014))
#define USB_GINTMSK     *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x018))
#define USB_GRXSTSP     *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x020))
#define USB_GRXFSIZ     *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x024))
#define USB_DIEPTXF0    *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x028))
#define USB_DIEPTXF1    *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x104))

/* --- USB DEVICE --- */
#define USB_DCFG        *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x800))
#define USB_DCTL        *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x804))
#define USB_DSTS        *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x808))
#define USB_DIEPMSK     *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x810))
#define USB_DOEPMSK     *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x814))
#define USB_DAINT       *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x818))
#define USB_DAINTMSK    *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x81C))

/* --- USB ENDPOINTS --- */
#define USB_DIEPCTL0    *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x900))
#define USB_DIEPINT0    *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x908))
#define USB_DIEPTSIZ0   *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x910))
#define USB_DIEPCTL1    *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x920))
#define USB_DIEPINT1    *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x928))
#define USB_DIEPTSIZ1   *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x930))
#define USB_DOEPCTL0    *((volatile uint32_t *)(USB_OTG_FS_BASE + 0xB00))
#define USB_DOEPINT0    *((volatile uint32_t *)(USB_OTG_FS_BASE + 0xB08))
#define USB_DOEPTSIZ0   *((volatile uint32_t *)(USB_OTG_FS_BASE + 0xB10))
#define USB_DFIFO0      *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x1000))
#define USB_DFIFO1      *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x2000))

/* --- ADC --- */
#define ADC_SR          *((volatile uint32_t *)(ADC1_BASE + 0x00))
#define ADC_CR2         *((volatile uint32_t *)(ADC1_BASE + 0x08))
#define ADC_SQR3        *((volatile uint32_t *)(ADC1_BASE + 0x34))
#define ADC_DR          *((volatile uint32_t *)(ADC1_BASE + 0x4C))

/* --- TIMERS (Encoders) --- */
#define TIM1_CR1        *((volatile uint32_t *)(TIM1_BASE + 0x00))
#define TIM1_SMCR       *((volatile uint32_t *)(TIM1_BASE + 0x08))
#define TIM1_CCMR1      *((volatile uint32_t *)(TIM1_BASE + 0x18))
#define TIM1_CCER       *((volatile uint32_t *)(TIM1_BASE + 0x20))
#define TIM1_CNT        *((volatile uint32_t *)(TIM1_BASE + 0x24))
#define TIM1_ARR        *((volatile uint32_t *)(TIM1_BASE + 0x2C))

/* --- I2C1 REGISTERS --- */
#define I2C1_BASE       0x40005400
#define I2C_CR1         *((volatile uint32_t *)(I2C1_BASE + 0x00))
#define I2C_CR2         *((volatile uint32_t *)(I2C1_BASE + 0x04))
#define I2C_OAR1        *((volatile uint32_t *)(I2C1_BASE + 0x08))
#define I2C_DR          *((volatile uint32_t *)(I2C1_BASE + 0x10))
#define I2C_SR1         *((volatile uint32_t *)(I2C1_BASE + 0x14))
#define I2C_SR2         *((volatile uint32_t *)(I2C1_BASE + 0x18))
#define I2C_CCR         *((volatile uint32_t *)(I2C1_BASE + 0x1C))
#define I2C_TRISE       *((volatile uint32_t *)(I2C1_BASE + 0x20))

/* --- PROTOTYPES --- */
void SystemClock_Config(void);
void USB_GPIO_Init(void);
void USB_Core_Init(void);
void USB_Device_Init(void);
void USB_NVIC_Init(void);
void USB_Write_EP0(const uint8_t *data, uint32_t len);
void USB_Send_MIDI_CC(uint8_t cc_num, uint8_t value);
void OTG_FS_IRQHandler(void);
void ADC_Init(void);
uint16_t ADC_Read(uint8_t channel);
void MUX_Select(uint8_t channel);
void Buttons_Init(void);
uint8_t Buttons_Read(void);
void Encoder_Init(void);
int16_t Encoder_Read(void);

#endif
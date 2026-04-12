#ifndef STM32F411_H
#define STM32F411_H

#include <stdint.h>

#define FLASH_BASE_REG  0x40023C00
#define RCC_BASE        0x40023800
#define GPIOA_BASE      0x40020000
#define GPIOC_BASE      0x40020800

#define FLASH_ACR       *((volatile uint32_t *)(FLASH_BASE_REG + 0x00))

#define RCC_CR          *((volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_PLLCFGR     *((volatile uint32_t *)(RCC_BASE + 0x04))
#define RCC_CFGR        *((volatile uint32_t *)(RCC_BASE + 0x08))
#define RCC_AHB1ENR     *((volatile uint32_t *)(RCC_BASE + 0x30))
#define RCC_AHB2ENR     *((volatile uint32_t *)(RCC_BASE + 0x34))

#define GPIOA_MODER     *((volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_OSPEEDR   *((volatile uint32_t *)(GPIOA_BASE + 0x08))
#define GPIOA_AFRH      *((volatile uint32_t *)(GPIOA_BASE + 0x24))

#define GPIOC_MODER     *((volatile uint32_t *)(GPIOC_BASE + 0x00))
#define GPIOC_BSRR      *((volatile uint32_t *)(GPIOC_BASE + 0x18))

#define USB_OTG_FS_BASE 0x50000000
#define USB_GAHBCFG     *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x008))
#define USB_GUSBCFG     *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x00C))
#define USB_GRSTCTL     *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x010))
#define USB_GINTSTS     *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x014))
#define USB_GINTMSK     *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x018))

void SystemClock_Config(void);
void USB_GPIO_Init(void);
void USB_Core_Init(void);

#endif

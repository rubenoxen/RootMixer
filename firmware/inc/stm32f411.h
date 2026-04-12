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
#define USB_GRXFSIZ     *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x024))
#define USB_DIEPTXF0    *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x028))
#define USB_DIEPTXF1    *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x104))
#define USB_DCFG        *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x800))
#define USB_DCTL        *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x804))
#define USB_DSTS        *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x808))
#define USB_DIEPMSK     *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x810))
#define USB_DOEPMSK     *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x814))
#define USB_DAINT       *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x818))
#define USB_DAINTMSK    *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x81C))
#define USB_DIEPCTL0    *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x900))
#define USB_DOEPCTL0    *((volatile uint32_t *)(USB_OTG_FS_BASE + 0xB00))
#define USB_DOEPTSIZ0   *((volatile uint32_t *)(USB_OTG_FS_BASE + 0xB10))
#define USB_GRXSTSP     *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x020))
#define USB_DFIFO0      *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x1000))
#define USB_DIEPINT0    *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x908))
#define USB_DOEPINT0    *((volatile uint32_t *)(USB_OTG_FS_BASE + 0xB08))

#define NVIC_ISER2      *((volatile uint32_t *)(0xE000E108))
#define USB_DIEPTSIZ0   *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x910))

void USB_Write_EP0(const uint8_t *data, uint32_t len);
void SystemClock_Config(void);
void USB_GPIO_Init(void);
void USB_Core_Init(void);
void USB_Device_Init(void);
void USB_NVIC_Init(void);
void OTG_FS_IRQHandler(void);

#endif

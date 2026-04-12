#include <stdint.h>
#include "stm32f411.h"

typedef struct {
    uint8_t  bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} __attribute__((packed)) USB_SetupPacket;


const uint8_t DeviceDescriptor[] = {
    18,
    0x01,
    0x00, 0x02,
    0x00,
    0x00,
    0x00,
    64,
    0x83, 0x04,
    0x11, 0x57,
    0x00, 0x02,
    0,
    0,
    0,
    1
};

const uint8_t ConfigDescriptor[] = {
    0x09, 0x02, 0x56, 0x00,
    0x02, 0x01, 0x00,
    0x80, 0x32,

    0x09, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,

    0x09, 0x24, 0x01, 0x00, 0x01, 0x09, 0x00, 0x01, 0x01,

    0x09, 0x04, 0x01, 0x00, 0x02, 0x01, 0x03, 0x00, 0x00,

    0x07, 0x24, 0x01, 0x00, 0x01, 0x41, 0x00,

    0x06, 0x24, 0x02, 0x01, 0x01, 0x00,

    0x09, 0x24, 0x03, 0x01, 0x02, 0x01, 0x01, 0x01, 0x00,

    0x09, 0x05, 0x01, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00,

    0x05, 0x25, 0x01, 0x01, 0x01,

    0x09, 0x05, 0x81, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00,

    0x05, 0x25, 0x01, 0x01, 0x02
};

#define USB_DCFG        *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x800))
#define USB_DCTL        *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x804))
#define USB_DSTS        *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x808))
#define USB_DIEPMSK     *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x810))
#define USB_DOEPMSK     *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x814))

#define USB_GRXFSIZ     *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x024))
#define USB_DIEPTXF0    *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x028))
#define USB_DIEPTXF1    *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x104))
#define USB_DIEPTXF2    *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x108))

void USB_GPIO_Init(void) {
    RCC_AHB1ENR |= (1 << 0);

    GPIOA_MODER &= ~((3 << 22) | (3 << 24));
    GPIOA_MODER |= (2 << 22) | (2 << 24);

    GPIOA_AFRH &= ~((0xF << 12) | (0xF << 16));
    GPIOA_AFRH |= (10 << 12) | (10 << 16);

    GPIOA_OSPEEDR |= (3 << 22) | (3 << 24);
}

void USB_Core_Init(void) {
    RCC_AHB2ENR |= (1 << 7);

    USB_GUSBCFG |= (1 << 30) | (5 << 10);

    while (!(USB_GRSTCTL & (1 << 31)));

    USB_GRSTCTL |= (1 << 0);

    while (USB_GRSTCTL & (1 << 0));

    for (volatile int i = 0; i < 1000; i++);

    USB_GAHBCFG |= (1 << 0);

    USB_GAHBCFG &= ~(1 << 8);
}

void USB_Device_Init(void) {
    USB_DCFG |= (3 << 0);

    USB_GRXFSIZ = 64;

    USB_DIEPTXF0 = (64 << 16) | 64;

    USB_DIEPTXF1 = (64 << 16) | 128;

    USB_GINTSTS = 0xFFFFFFFF;

    USB_GINTMSK |= (1 << 12) | (1 << 11) | (1 << 19) | (1 << 18);

    USB_DCTL &= ~(1 << 1);
}

void USB_NVIC_Init(void) {
    NVIC_ISER2 |= (1 << 3);
}

void OTG_FS_IRQHandler(void) {
    uint32_t gintsts = USB_GINTSTS;

    if (gintsts & (1 << 12)) {
        USB_GINTSTS = (1 << 12);

        USB_DCFG &= ~(0x7F << 4);

        USB_GRSTCTL = (0 << 6) | (1 << 5);
        while (USB_GRSTCTL & (1 << 5));

        USB_GRSTCTL = (1 << 4);
        while (USB_GRSTCTL & (1 << 4));

        USB_DAINTMSK |= (1 << 0) | (1 << 16);

        USB_DOEPMSK |= (1 << 3) | (1 << 0);
        USB_DIEPMSK |= (1 << 0) | (1 << 3);

        USB_DOEPTSIZ0 = (1 << 29) | (1 << 19) | 24;
    }

    if (gintsts & (1 << 11)) {
        USB_GINTSTS = (1 << 11);
    }

    if (gintsts & (1 << 4)) {

        uint32_t rx_status = USB_GRXSTSP;

        uint32_t endpoint = rx_status & 0xF;
        uint32_t byte_count = (rx_status >> 4) & 0x7FF;
        uint32_t packet_status = (rx_status >> 17) & 0x0F;

        if (endpoint == 0 && packet_status == 6) {

            uint32_t word1 = USB_DFIFO0;
            uint32_t word2 = USB_DFIFO0;

            uint32_t setup_data[2] = {word1, word2};
            USB_SetupPacket *setup = (USB_SetupPacket *)setup_data;

            if (setup->bRequest == 0x06) {
                uint8_t desc_type = (setup->wValue >> 8);

                if (desc_type == 1) {
                    uint32_t len = setup->wLength > sizeof(DeviceDescriptor) ? sizeof(DeviceDescriptor) : setup->wLength;
                    USB_Write_EP0(DeviceDescriptor, len);
                }
                else if (desc_type == 2) {
                    uint32_t len = setup->wLength > sizeof(ConfigDescriptor) ? sizeof(ConfigDescriptor) : setup->wLength;
                    USB_Write_EP0(ConfigDescriptor, len);
                }
            }

            else if (setup->bRequest == 0x05) {
                uint32_t new_address = setup->wValue;
                USB_DCFG &= ~(0x7F << 4);
                USB_DCFG |= (new_address << 4);
                USB_Write_EP0(0, 0);
            }

            else if (setup->bRequest == 0x09) {

                *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x81C)) |= (1 << 1) | (1 << 17);
                *((volatile uint32_t *)(USB_OTG_FS_BASE + 0x920)) = (1 << 31) | (2 << 18) | (1 << 22) | (1 << 28);
                *((volatile uint32_t *)(USB_OTG_FS_BASE + 0xB20)) = (1 << 31) | (2 << 18) | (1 << 26);

                USB_Write_EP0(0, 0);
            }
        }

        else if (packet_status == 2) {
            uint32_t words_to_read = (byte_count + 3) / 4;
            for (uint32_t i = 0; i < words_to_read; i++) {
                volatile uint32_t dummy_read = USB_DFIFO0;
                (void)dummy_read;
            }
        }
    }
}

void USB_Write_EP0(const uint8_t *data, uint32_t len) {
    USB_DIEPTSIZ0 = (1 << 19) | len;

    USB_DIEPCTL0 |= (1 << 31) | (1 << 26);

    if (len > 0) {
        uint32_t words = (len + 3) / 4;
        for (uint32_t i = 0; i < words; i++) {
            USB_DFIFO0 = *((uint32_t *)(data + (i * 4)));
        }
    }
}
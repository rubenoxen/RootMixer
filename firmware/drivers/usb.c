#include "stm32f411.h"

typedef struct { uint8_t T; uint8_t R; uint16_t V; uint16_t I; uint16_t L; } __attribute__((packed)) SETUP;

const uint8_t DeviceDescriptor[] = { 18,1,0,2,0,0,0,64,0x83,4,0x11,0x57,0,2,0,0,0,1 };
const uint8_t ConfigDescriptor[] = { 9,2,86,0,2,1,0,0x80,50,9,4,0,0,0,1,1,0,0,9,0x24,1,0,1,9,0,1,1,9,4,1,0,2,1,3,0,0,7,0x24,1,0,1,0x41,0,6,0x24,2,1,1,0,9,0x24,3,1,2,1,1,1,0,9,5,1,2,64,0,0,0,0,5,0x25,1,1,1,9,5,0x81,2,64,0,0,0,0,5,0x25,1,1,2 };

void USB_GPIO_Init(void) {
    RCC_AHB1ENR |= 1;
    GPIOA_MODER = (GPIOA_MODER & ~(0xF<<22)) | (0xA<<22);
    GPIOA_AFRH = (GPIOA_AFRH & ~(0xFF<<12)) | (0xAA<<12);
    GPIOA_OSPEEDR |= (0xF<<22);
}

void USB_Core_Init(void) {
    RCC_AHB2ENR |= (1<<7);
    USB_GUSBCFG |= (1<<30) | (5<<10);
    while (!(USB_GRSTCTL & (1<<31)));
    USB_GRSTCTL |= 1;
    while (USB_GRSTCTL & 1);
    USB_GAHBCFG |= 1;
}

void USB_Device_Init(void) {
    USB_DCFG |= 3;
    USB_GRXFSIZ = 64;
    USB_DIEPTXF0 = (64<<16)|64;
    USB_DIEPTXF1 = (64<<16)|128;
    USB_GINTSTS = 0xFFFFFFFF;
    USB_GINTMSK |= (1<<12)|(1<<11)|(1<<19)|(1<<18);
    USB_DCTL &= ~2;
}

void USB_NVIC_Init(void) { NVIC_ISER2 |= (1<<3); }

void USB_Write_EP0(const uint8_t *d, uint32_t l) {
    USB_DIEPTSIZ0 = (1<<19)|l;
    USB_DIEPCTL0 |= (1<<31)|(1<<26);
    for (uint32_t i=0; i<(l+3)/4; i++) USB_DFIFO0 = *((uint32_t*)(d + i*4));
}

void USB_Send_MIDI_CC(uint8_t n, uint8_t v) {
    if (USB_DIEPCTL1 & (1<<31)) return;
    USB_DIEPTSIZ1 = (1<<19)|4;
    USB_DIEPCTL1 |= (1<<31)|(1<<26);
    USB_DFIFO1 = 0x0B | (0xB0<<8) | (n<<16) | (v<<24);
}

void OTG_FS_IRQHandler(void) {
    uint32_t s = USB_GINTSTS;
    if (s & (1<<12)) {
        USB_GINTSTS = (1<<12); USB_DCFG &= ~0x7F0;
        USB_GRSTCTL = 0x20; while(USB_GRSTCTL&0x20);
        USB_GRSTCTL = 0x10; while(USB_GRSTCTL&0x10);
        USB_DAINTMSK |= 0x10001; USB_DOEPMSK |= 9; USB_DIEPMSK |= 9;
        USB_DOEPTSIZ0 = 0x20080018;
    }
    if (s & (1<<4)) {
        uint32_t p = USB_GRXSTSP;
        if ((p&0xF)==0 && ((p>>17)&0xF)==6) {
            uint32_t d[2] = {USB_DFIFO0, USB_DFIFO0};
            SETUP *st = (SETUP*)d;
            if (st->R == 6) {
                if (st->V>>8 == 1) USB_Write_EP0(DeviceDescriptor, 18);
                else if (st->V>>8 == 2) USB_Write_EP0(ConfigDescriptor, 86);
            } else if (st->R == 5) {
                USB_DCFG = (USB_DCFG & ~0x7F0) | (st->V << 4);
                USB_Write_EP0(0,0);
            } else if (st->R == 9) {
                USB_DAINTMSK |= 0x20002;
                USB_DIEPCTL1 = 0xC4080000;
                USB_Write_EP0(0,0);
            }
        } else for (uint32_t i=0; i<(((p>>4)&0x7FF)+3)/4; i++) (void)USB_DFIFO0;
    }
    if (s & (1<<18)) {
        if (USB_DAINT & 1) { USB_DIEPINT0=0xFF; USB_DOEPTSIZ0=0x20080040; USB_DOEPCTL0|=0x84000000; }
        if (USB_DAINT & 2) USB_DIEPINT1=0xFF;
    }
    USB_GINTSTS = s;
}
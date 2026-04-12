#include <stdint.h>

extern uint32_t _estack;
extern uint32_t _la_data;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

int main(void);

void OTG_FS_IRQHandler(void);

void Reset_Handler(void) {
    uint32_t *src = &_la_data;
    uint32_t *dest = &_sdata;

    while (dest < &_edata) {
        *dest++ = *src++;
    }

    dest = &_sbss;
    while (dest < &_ebss) {
        *dest++ = 0;
    }

    main();

    while (1);
}

void Default_Handler(void) {
    while (1);
}

__attribute__ ((section (".isr_vector")))
uint32_t *isr_vectors[] = {
    (uint32_t *)&_estack,
    (uint32_t *)Reset_Handler,
    (uint32_t *)Default_Handler,
    (uint32_t *)Default_Handler,

    [4 ... 82] = (uint32_t *)Default_Handler,

    [83] = (uint32_t *)OTG_FS_IRQHandler
};
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "pico-cart.pio.h"
#include "hardware/vreg.h"
#include "hardware/sync.h"

#define GPIO_DIR 8
#define GPIO_CLOCK 16
#define GPIO_LD 17
#define GPIO_ADDRESS 18
#define GPIO_DATA 0
#define GPIO_RD 11

void __time_critical_func(mainloop)(uint addr_sm, uint data_sm);

int main()
{
    stdio_init_all();
	setup_default_uart();

    vreg_set_voltage(VREG_VOLTAGE_1_25);
    set_sys_clock_khz(384000, true);

    PIO pio = pio0;
    PIO pio2 = pio1;

    uint offset = pio_add_program(pio, &cart_address_program);
    uint addr_sm = pio_claim_unused_sm(pio, true);
    pio_address_init(pio, addr_sm, offset, GPIO_ADDRESS, GPIO_CLOCK, GPIO_LD);

    offset = pio_add_program(pio2, &cart_dir_data_program);
    uint data_sm = pio_claim_unused_sm(pio2, true);
    pio_data_init(pio2, data_sm, offset, GPIO_RD, GPIO_DATA, GPIO_DIR);

    pio_enable_sm_mask_in_sync(pio, (1 << addr_sm));
    pio_enable_sm_mask_in_sync(pio2, (1 << data_sm));

    pio_sm_put(pio2, data_sm, 0x00);
    mainloop(addr_sm, data_sm);

}

void __time_critical_func(mainloop)(uint addr_sm, uint data_sm) {
    save_and_disable_interrupts();
    uint32_t addresses[256] = {0};
    uint8_t data[0x10000];
    data[0xFFFC] = 0x01;
    data[0xFFFD] = 0x80;
    data[0x8001] = 0x5C;
    data[0x8002] = 0x01;
    data[0x8003] = 0x80;
    data[0x8004] = 0x81;
    uint32_t adr = 0;
    uint8_t i = 0;

    pio_sm_clear_fifos(pio0, addr_sm);
    pio_sm_clear_fifos(pio1, data_sm);
	while (1)
	{        
        adr = (pio_sm_get_blocking(pio0, addr_sm) & 0xFFFF);
        if(adr >= 0x8000)
        {
            pio_sm_put(pio1, data_sm, data[adr&0xFFFF]);
        }
    }
}
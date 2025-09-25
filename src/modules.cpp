#include <hardware/structs/sio.h>
#include <hardware/i2c.h>
#include <pico/stdlib.h>

#include "pins.hpp"

static const uint32_t msg_len = 1;

void callback(uint gpio, uint32_t events) {
    (void)gpio;
    (void)events;
    uint32_t const *b = buttons;
    uint32_t tmp = ~sio_hw->gpio_in;
    uint8_t val =
	btp(b[0], tmp, 0) |
	btp(b[1], tmp, 1) |
	btp(b[2], tmp, 2) |
	btp(b[3], tmp, 3) |
	btp(b[4], tmp, 4) |
	btp(b[5], tmp, 5);
	
    i2c_write_blocking(i2c1, I2C_SLAVE_ADDRESS, &val, msg_len, false);
}

int main() {
    gpio_init_mask(all_buttons | bit(I2C1_SDA_PIN) | bit(I2C1_SCL_PIN));
    gpio_set_function(I2C1_SDA_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C1_SDA_PIN);

    gpio_set_function(I2C1_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C1_SCL_PIN);
    i2c_init(i2c1, I2C_BAUDRATE);
    
    for (uint32_t a : buttons) {
	gpio_pull_up(a);
	gpio_set_irq_enabled_with_callback(a,
					   GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE,
					   true,
					   callback);
    }

    for(;;);
    return 0;
}

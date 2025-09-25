#include <hardware/structs/sio.h>
#include <hardware/i2c.h>
#include <hardware/spi.h>
#include <pico/i2c_slave.h>
#include <pico/multicore.h>
#include <pico/stdlib.h>

#include "pins.hpp"

static const uint32_t msg_len = 1;

uint32_t btns = 0;

void btn_press(uint gpio, uint32_t events) {
    (void)gpio;
    (void)events;
    uint32_t const *b = buttons;
    uint32_t tmp = ~sio_hw->gpio_in;
    btns = (btns & 0xFFFFFFF8) |
	btp(b[0], tmp, 0) |
	btp(b[1], tmp, 1) |
	btp(b[2], tmp, 2);
}

void i2c_slave(i2c_inst_t *i2c, i2c_slave_event_t event) {
    switch(event) {
    case I2C_SLAVE_RECEIVE:
	if (i2c == i2c0) {
	    btns = ((btns & 0xFFFFFE07) | (((uint32_t)(i2c_read_byte_raw(i2c)) << 3) & 0x000001F8));
	} else {
	    btns = ((btns & 0xFFFF81FF) | (((uint32_t)(i2c_read_byte_raw(i2c)) << 9) & 0x00007E00));
	}
	break;
    case I2C_SLAVE_FINISH:
	break;
    default:
	break;
    }
}

void spi_stuff() {
    spi_init(spi0, 1000 * 1000);
    spi_set_slave(spi0, true);
    gpio_set_function(SPI0_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_TX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_CSn_PIN, GPIO_FUNC_SPI);
    //bi_decl(bi_4pins_with_func(SPI0_RX_PIN, SPI0_TX_PIN, SPI0_SCK_PIN, SPI0_CSn_PIN, GPIO_FUNC_SPI));

    uint32_t* val = (uint32_t*)multicore_fifo_pop_blocking();

    for (;;) {
	spi_write_blocking(spi0, (uint8_t*)val, 4);
    }
}

int main() {
    gpio_init_mask(
	bit(I2C0_SDA_PIN) |
	bit(I2C0_SCL_PIN) |
	bit(I2C1_SDA_PIN) |
	bit(I2C1_SCL_PIN) |
        all_buttons
	);

    /* I2C */
    gpio_set_function(I2C0_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C0_SCL_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C1_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C1_SCL_PIN, GPIO_FUNC_I2C);
    
    gpio_pull_up(I2C0_SDA_PIN);
    gpio_pull_up(I2C0_SCL_PIN);
    gpio_pull_up(I2C1_SDA_PIN);
    gpio_pull_up(I2C1_SCL_PIN);
    
    i2c_init(i2c0, I2C_BAUDRATE);
    i2c_init(i2c1, I2C_BAUDRATE);

    i2c_slave_init(i2c0, I2C_SLAVE_ADDRESS, &i2c_slave);
    i2c_slave_init(i2c1, I2C_SLAVE_ADDRESS, &i2c_slave);

    /* Buttons */
    for (uint32_t a : buttons) {
	gpio_pull_up(a);
	gpio_set_irq_enabled_with_callback(a,
					   GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE,
					   true,
					   btn_press);
    }

    /* SPI */
    multicore_launch_core1(spi_stuff);
    multicore_fifo_push_blocking((uint32_t)&btns);
    
    for (;;);
	
    return 0;
}

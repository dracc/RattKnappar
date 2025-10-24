#include <hardware/structs/sio.h>
#include <hardware/dma.h>
#include <hardware/i2c.h>
#include <hardware/spi.h>
#include <hardware/clocks.h>
#include <pico/i2c_slave.h>
#include <pico/multicore.h>
#include <pico/stdlib.h>

#include "pins.hpp"

static const uint32_t msg_len = 1;

volatile uint32_t btns = 0;

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
  uint32_t rx_buf;
  spi_init(spi0, (42 * 1000 * 1000) / 32);

  spi_set_slave(spi0, true);
  gpio_set_function(SPI0_RX_PIN, GPIO_FUNC_SPI);
  gpio_set_function(SPI0_SCK_PIN, GPIO_FUNC_SPI);
  gpio_set_function(SPI0_TX_PIN, GPIO_FUNC_SPI);
  gpio_set_function(SPI0_CSn_PIN, GPIO_FUNC_SPI);

  spi_set_format(spi0, 16, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
  //bi_decl(bi_4pins_with_func(SPI0_RX_PIN, SPI0_TX_PIN, SPI0_SCK_PIN, SPI0_CSn_PIN, GPIO_FUNC_SPI));
  const uint32_t dma_tx = dma_claim_unused_channel(true);
  const uint32_t dma_rx = dma_claim_unused_channel(true);

  // Configure TX DMA
  auto c = dma_channel_get_default_config(dma_tx);
  channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
  // Use a 4 byte ring buffer
  channel_config_set_ring(&c, false, 2);
  channel_config_set_dreq(&c, spi_get_dreq(spi0, true));
  dma_channel_configure(dma_tx, &c, &spi_get_hw(spi0)->dr,
                        &btns, -1, false);

  // Configure RX DMA (Unused for now)
  c = dma_channel_get_default_config(dma_rx);
  channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
  channel_config_set_dreq(&c, spi_get_dreq(spi0, false));
  channel_config_set_read_increment(&c, false);
  channel_config_set_write_increment(&c, true);
  channel_config_set_ring(&c, true, 2);
  dma_channel_configure(dma_rx, &c, (uint8_t*)&rx_buf, &spi_get_hw(spi0)->dr, -1, false);

  dma_start_channel_mask((1u << dma_tx) | (1u << dma_rx));

  for (;;) {
  }
}

int main() {
  // Underclock baseclk, enables running the peri_clk at 42 MHz.
  set_sys_clock_khz(84000, true);
  clock_configure(clk_peri, 9, CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
                  84000000, 84000000);
  gpio_init_mask(
                 bit(I2C0_SDA_PIN) | bit(I2C0_SCL_PIN) |
                 bit(I2C1_SDA_PIN) | bit(I2C1_SCL_PIN) |
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

  for (;;) {
  }

  return 0;
}

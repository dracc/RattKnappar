#ifndef __PINS_HPP
#define __PINS_HPP

#define bit(x) (uint32_t(1<<x))

#define btp(x,y,z) ((((1<<x)&y)>>x)<<z)

#define B1 3
#define B2 4
#define B3 9
#define B4 12
#define B5 14
#define B6 28

#define HORN 9
#define L_PADDLE 10
#define R_PADDLE 11

#ifdef __MIDDLE
uint32_t const buttons[] = {
    HORN, L_PADDLE, R_PADDLE
};

const uint32_t all_buttons =
    1 << HORN |
    1 << L_PADDLE |
    1 << R_PADDLE;
#else
uint32_t const buttons[] = {
    B1, B2, B3, B4, B5, B6
};

const uint32_t all_buttons =
    1 << B1 |
    1 << B2 |
    1 << B3 |
    1 << B4 |
    1 << B5 |
    1 << B6;
#endif // __MIDDLE

const uint32_t I2C_SLAVE_ADDRESS = 0x17;
const uint32_t I2C_BAUDRATE = 100000; // 100 kHz
const uint32_t I2C0_SDA_PIN = 4;
const uint32_t I2C0_SCL_PIN = 5;
const uint32_t I2C1_SDA_PIN = 26;
const uint32_t I2C1_SCL_PIN = 27;

const uint32_t SPI0_RX_PIN = 0;
const uint32_t SPI0_CSn_PIN = 1;
const uint32_t SPI0_SCK_PIN = 2;
const uint32_t SPI0_TX_PIN = 3;


#endif

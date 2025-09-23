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

const uint32_t I2C_SLAVE_ADDRESS = 0x17;
const uint32_t I2C_BAUDRATE = 100000; // 100 kHz
const uint32_t I2C_MASTER_SDA_PIN = 26;
const uint32_t I2C_MASTER_SCL_PIN = 27;

#endif

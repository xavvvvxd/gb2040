#pragma once

#include <cstdint>

#include "hardware/spi.h"

#include "core/graphics.h"

#define SPI_PORT spi1

namespace GB2040::Platform::ILI9341
{

enum class Command {
    NOP = 0x00,
    SWRESET = 0x01,
    SLPOUT = 0x11,
    GAMMA = 0x26,
    DISPON = 0x29,
    CASET = 0x2A,
    PASET = 0x2B,
    RAMWR = 0x2C,
    MADCTL = 0x36,
    VSCRO = 0x37,
    COLMOD = 0x3A,
    FRCTL1 = 0xB1,
    DFCTL = 0xB6,
    PWR1 = 0xC0,
    PWR2 = 0xC1,
    VCOM1 = 0xC5,
    VCOM2 = 0xC7,
    PWRCTLA = 0xCB,
    PWRCTLB = 0xCF,
    GPCTL = 0xE0,
    GNCTL = 0xE1,
    DTCTLA = 0xE8,
    DTCTLB = 0xEA,
    PWRSEQ = 0xED,
    G3EN = 0xF2,
    PUMPRC = 0xF7,
};

class ILI9341 {
public:
    ILI9341(uint, uint, uint, uint, uint, uint);

    void clear(void);
    void drawFramebuffer(GB2040::Core::Framebuffer&, uint, uint);

    void waitDMA(void);
private:
    void init(void);
    void sendCmd(Command, std::initializer_list<uint8_t> = {});
    void sendData(void*, size_t);
    void txBlock(uint, uint, uint, uint, void*, size_t);

    uint pinSck;
    uint pinSda;
    uint pinCs;
    uint pinDc;
    uint pinRst;
    uint pinBl;

    int dmaChannel;
};

} // namespace GB2040::Platform::ILI9341
#include "platform/pico/ili9341.h"

#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/clocks.h"

namespace GB2040::Platform::ILI9341
{

ILI9341::ILI9341(uint sck, uint sda, uint cs, uint dc, uint rst, uint bl) {
    uint32_t gpioMask = (1 << sck) | (1 << sda) | (1 << cs) | (1 << dc) | (1 << rst) | (1 << bl);
    gpio_init_mask(gpioMask);
    gpio_set_dir_masked(gpioMask, gpioMask);

    pinSck = sck;
    pinSda = sda;
    pinCs = cs;
    pinDc = dc;
    pinRst = rst;
    pinBl = bl;

    gpio_put(cs, false);
    gpio_put(bl, true);
    
    gpio_set_function(sck, GPIO_FUNC_SPI);
    gpio_set_function(sda, GPIO_FUNC_SPI);

    spi_init(SPI_PORT, 62'500'000); // 62.5 MHz
    spi_set_format(SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    dmaChannel = dma_claim_unused_channel(true);

    init();
}

void ILI9341::init(void) {
    gpio_put(pinRst, 0);
    sleep_ms(50);
    gpio_put(pinRst, 1);
    sleep_ms(50);
    sendCmd(Command::SWRESET);
    sleep_ms(120);
    sendCmd(Command::PWRCTLB, { 0x00, 0xC1, 0x30 });
    sendCmd(Command::PWRSEQ, { 0x64, 0x03, 0x12, 0x81 });
    sendCmd(Command::DTCTLA, { 0x85, 0x00, 0x78 });
    sendCmd(Command::PWRCTLA, { 0x39, 0x2C, 0x00, 0x34, 0x02 });
    sendCmd(Command::PUMPRC, { 0x20 });
    sendCmd(Command::DTCTLB, { 0x00, 0x00 });
    sendCmd(Command::PWR1, { 0x23 });
    sendCmd(Command::PWR2, { 0x10 });
    sendCmd(Command::VCOM1, { 0x3E, 0x28 });
    sendCmd(Command::VCOM2, { 0x86 });
    sendCmd(Command::MADCTL, { 0xE0 });
    sendCmd(Command::COLMOD, { 0x55 });
    sendCmd(Command::FRCTL1, { 0x00, 0x18 });
    sendCmd(Command::DFCTL, { 0x08, 0x82, 0x27 });
    sendCmd(Command::G3EN, { 0x00 });
    sendCmd(Command::GAMMA, { 0x01 });
    sendCmd(Command::GPCTL, { 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00 });
    sendCmd(Command::GNCTL, { 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F });
    sendCmd(Command::SLPOUT);
    sleep_ms(100);
    sendCmd(Command::DISPON);
    sleep_ms(100);
}

void ILI9341::sendCmd(Command command, std::initializer_list<uint8_t> params) {
    gpio_put(pinDc, false);
    uint8_t byte = static_cast<uint8_t>(command);
    spi_write_blocking(SPI_PORT, &byte, 1);

    if (params.size()) {
        gpio_put(pinDc, true);
        spi_write_blocking(SPI_PORT, params.begin(), params.size());
    }
}

void ILI9341::sendData(void* data, size_t count) {
    // start DMA transfer
    gpio_put(pinDc, true);
    dma_channel_config c = dma_channel_get_default_config(dmaChannel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);
    channel_config_set_dreq(&c, spi_get_dreq(SPI_PORT, true));

    dma_channel_configure(
        dmaChannel,
        &c,
        &spi_get_hw(SPI_PORT)->dr,
        data,
        count,
        true
    );
}

void ILI9341::drawFramebuffer(GB2040::Core::Framebuffer& fb, uint xOffset, uint yOffset) {
    if (dma_channel_is_busy(dmaChannel)) waitDMA();

    txBlock(xOffset,
            yOffset,
            xOffset + GB_WIDTH - 1,
            yOffset + GB_HEIGHT - 1,
            fb.data(), 
            fb.size() * 2);
}

void ILI9341::txBlock(uint x1, uint y1, uint x2, uint y2, void* data, size_t count) {
    sendCmd(Command::CASET, { static_cast<uint8_t>(x1 >> 8),
                              static_cast<uint8_t>(x1 & 0xFF), 
                              static_cast<uint8_t>(x2 >> 8), 
                              static_cast<uint8_t>(x2 & 0xFF) });
    sendCmd(Command::PASET, { static_cast<uint8_t>(y1 >> 8),
                              static_cast<uint8_t>(y1 & 0xFF),
                              static_cast<uint8_t>(y2 >> 8),
                              static_cast<uint8_t>(y2 & 0xFF) });
    sendCmd(Command::RAMWR);
    sendData(data, count);
}

void ILI9341::clear(void) {
    static uint8_t pixel = 0x00;

    if (dma_channel_is_busy(dmaChannel)) waitDMA();

    sendCmd(Command::CASET, { 0x00, 0x00, 0x01, 0x3F });
    sendCmd(Command::PASET, { 0x00, 0x00, 0x00, 0xEF });
    sendCmd(Command::RAMWR);

    gpio_put(pinDc, true);

    dma_channel_config c = dma_channel_get_default_config(dmaChannel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, false);
    channel_config_set_dreq(&c, spi_get_dreq(SPI_PORT, true));

    dma_channel_configure(
        dmaChannel,
        &c,
        &spi_get_hw(SPI_PORT)->dr,
        &pixel,
        240 * 320 * 2,
        true
    );

    waitDMA();
}

void ILI9341::waitDMA(void) {
    dma_channel_wait_for_finish_blocking(dmaChannel);
}

} // namespace GB2040::Platform::ILI9341

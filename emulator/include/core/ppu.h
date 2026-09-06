#pragma once

#include "core/graphics.h"
#include "core/mmu.h"

#include <cstdint>

#define VRAM_SIZE      8192

#define OAM_SIZE       160

namespace GB2040::Core
{

enum class PPUMode : uint8_t { HBLANK, VBLANK, OAM_SCAN, PIXEL_TRANSFER };
enum class PPULayer : uint8_t { BACKGROUND, WINDOW };

class Console;

struct Sprite {
    uint8_t y;
    uint8_t x;
    uint8_t tileIdx;
    uint8_t attrs;
    uint8_t oamIdx;
};
    
class PPU {
public:
    PPU(Console& console);

    void tick(size_t);
    void renderScanline(void);

    uint8_t readVram(uint16_t);
    void writeVram(uint16_t, uint8_t);

    uint8_t readCramBg(void);
    void writeCramBg(uint8_t);

    uint8_t readCramObj(void);
    void writeCramObj(uint8_t);

    uint8_t readOam(uint16_t);
    void writeOam(uint16_t, uint8_t);

    uint8_t readStat(void);
private:
    static constexpr Colour dmgLut[4] {
        0xFFFF,
        0x55AD,
        0xAA52,
        0x0000
    };

    uint16_t getMapBase(PPULayer);

    void renderScanlineLayer(PPULayer);
    void renderScanlineObjects(void);
    void renderScanlinePixel(PPULayer, uint8_t x, uint16_t rowOffset);
    void sortSprites(Sprite*, size_t);

    void hBlank(void);
    void vBlank(void);
    void oamScan(void);
    void pixelTransfer(void);

    void oamDma(uint8_t);

    friend MMU; // for IO ports

    Console& console;

    Framebuffer*& framebuffer;

    bool prevHBlank = false;
    bool prevVBlank = false;
    bool prevOam = false;
    bool prevLyc = false;

    PPUMode mode = PPUMode::HBLANK;
    uint32_t modeClock = 0;

    Sprite sprites[40];

    uint8_t bgLineIndices[GB_WIDTH];
    bool objectPixelsDrawn[GB_WIDTH];

    uint8_t lcdc = 0x00;
    uint8_t stat = 0x00;

    uint8_t ly = 0;
    uint8_t lyc = 0;

    uint8_t wly = 0;

    uint8_t scx = 0;
    uint8_t scy = 0;

    uint8_t wx = 0;
    uint8_t wy = 0;

    // GB palette registers
    uint8_t bgp = 0x00;
    uint8_t obp0 = 0x00;
    uint8_t obp1 = 0x00;

    uint8_t vram[VRAM_SIZE];
    uint8_t oam[OAM_SIZE];
};

} // namespace GB2040::Core
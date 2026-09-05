#include "core/ppu.h"
#include "core/graphics.h"
#include "core/console.h"

#include <cstdint>
#include <cstring>
#include <algorithm>

namespace GB2040::Core
{

PPU::PPU(Console& console)
: console(console), framebuffer(console.platform->getBackBuffer()) {
    // initialise VRAM
    memset(vram, 0, VRAM_SIZE);
    memset(oam, 0, OAM_SIZE);
}

void PPU::tick(size_t cycles) {
    modeClock += cycles;

    if (!(lcdc & 0x80)) {
        // ppu disabled
        modeClock = 0;
        ly = 0;

        return;
    }

    switch (mode) {
        case PPUMode::HBLANK:
            hBlank();
            break;
        case PPUMode::VBLANK:
            vBlank();
            break;
        case PPUMode::OAM_SCAN:
            oamScan();
            break;
        case PPUMode::PIXEL_TRANSFER:
            pixelTransfer();
            break;
    }

    // STAT
    bool hBlankStat = (mode == PPUMode::HBLANK)   && (stat & 0x08);
    bool vBlankStat = (mode == PPUMode::VBLANK)   && (stat & 0x10);
    bool oamStat    = (mode == PPUMode::OAM_SCAN) && (stat & 0x20);
    bool lycStat    = (ly == lyc)                 && (stat & 0x40);

    if ((hBlankStat && !prevHBlank) ||
        (vBlankStat && !prevVBlank) ||
        (oamStat    && !prevOam)    ||
        (lycStat    && !prevLyc)) {

        console.requestInterrupt(Interrupt::STAT);
    }

    prevHBlank = hBlankStat;
    prevVBlank = vBlankStat;
    prevOam = oamStat;
    prevLyc = lycStat;
}

void PPU::hBlank(void) {
    memset(objectPixelsDrawn, 0, sizeof(objectPixelsDrawn));

    if (modeClock >= 204) {
        modeClock -= 204;
        ly++;
        if (ly == 144) {
            wly = 0;

            console.requestInterrupt(Interrupt::VBLANK);
            mode = PPUMode::VBLANK;
            console.platform->draw();
        } else {
            mode = PPUMode::OAM_SCAN;
        }
    }
}

void PPU::vBlank(void) {
    if (modeClock >= 456) {
        modeClock -= 456;
        ly++;
        if (ly > 153) {
            ly = 0;
            mode = PPUMode::OAM_SCAN;
        }
    }
}

void PPU::oamScan(void) {
    if (modeClock >= 80) {
        modeClock -= 80;

        for (int i = 0; i < 160; i += 4) {
            sprites[i >> 2].y = oam[i];
            sprites[i >> 2].x = oam[i + 1];
            sprites[i >> 2].tileIdx = oam[i + 2];
            sprites[i >> 2].attrs = oam[i + 3];
            sprites[i >> 2].oamIdx = i >> 2;
        }

        mode = PPUMode::PIXEL_TRANSFER;
    }
}

void PPU::pixelTransfer(void) {
    if (modeClock >= 172) {
        modeClock -= 172;
        renderScanline();

        mode = PPUMode::HBLANK;
    }
}

void PPU::renderScanline(void) {
    if (lcdc & 0x01) { // bg & window enabled
        renderScanlineLayer(PPULayer::BACKGROUND);

        if (lcdc & 0x20) { // window enabled
            renderScanlineLayer(PPULayer::WINDOW);
        }
    } else {
        for (int x = 0; x < GB_WIDTH; x++) {
            framebuffer->setPixel(x, ly, dmgLut[0]);
            bgLineIndices[x] = 0;
        }
    }

    if (lcdc & 0x02) { // objects enabled
        renderScanlineObjects();
    }
}

void PPU::renderScanlineLayer(PPULayer layer) {
    if (layer == PPULayer::WINDOW && ly < wy) return;

    bool drewWindowPixel = false;

    for (int x = 0; x < GB_WIDTH; x++) {
        if (layer == PPULayer::WINDOW) {
            if (x + 7 < wx) continue;

            // window is completely offscreen
            if (wx < 7 || wx > 166 || wy > 143) continue;

            drewWindowPixel = true;
        }
        
        renderScanlinePixel(layer, x);
    }

    if (layer == PPULayer::WINDOW && drewWindowPixel) wly++;
}

void PPU::renderScanlinePixel(PPULayer layer, uint8_t x) {
    uint16_t mapBase = getMapBase(layer);

    uint8_t bgX, bgY;
    if (layer == PPULayer::WINDOW) {
        bgX = x - (wx - 7);
        bgY = wly;
    } else {
        bgX = scx + x;
        bgY = scy + ly;
    }

    uint8_t tileY = bgY >> 3;
    uint8_t tileX = bgX >> 3;

    uint8_t pixelX = bgX & 7;
    uint8_t pixelY = bgY & 7;

    uint8_t tileId = vram[mapBase + tileY * 32 + tileX];

    uint16_t tileAddr;
    if (lcdc & 0x10) { // unsigned addressing
        tileAddr = 0x0 + tileId * 16;
    } else { // signed addressing
        tileAddr = 0x1000 + (int8_t)tileId * 16;
    }

    tileAddr += pixelY * 2;

    // GB tile data is split into 2 bytes, one for each bitplane (low bit followed by high bit)
    uint8_t low = vram[tileAddr];
    uint8_t high = vram[tileAddr + 1];

    // get relevant bits from bitplanes
    uint8_t highBit = (high >> (7 - pixelX)) & 1;
    uint8_t lowBit = (low >> (7 - pixelX)) & 1;
    uint8_t colourIdx = (highBit << 1) | lowBit;

    bgLineIndices[x] = colourIdx;

    Colour finalColour = dmgLut[(bgp >> colourIdx * 2) & 0x03];

    framebuffer->setPixel(x, ly, finalColour);
}

void PPU::renderScanlineObjects(void) {
    uint8_t spriteHeight = lcdc & 0x04 ? 16 : 8;

    uint8_t spritesLoaded = 0;
    Sprite lineSprites[10];
    for (uint8_t i = 0; i < 40; i++) {
        Sprite& sprite = sprites[i];

        if (sprite.y == 0 || sprite.y >= GB_HEIGHT + 16) continue; // sprite is offscreen
        if (spritesLoaded >= 10) continue; // only draw 10 sprites per scanline
        if ((ly - (sprite.y - 16)) > spriteHeight - 1 || ly < sprite.y - 16) continue; // if no part of the sprite intersects with this line
        lineSprites[spritesLoaded] = sprite;
        spritesLoaded++;
    }

    sortSprites(lineSprites, spritesLoaded);
    
    for (int i = 0; i < spritesLoaded; i++) {
        Sprite& sprite = lineSprites[i];
        uint8_t tileIdx = sprite.tileIdx;

        bool verFlip = sprite.attrs & 0x40;

        uint8_t lineInTile = ly - (sprite.y - 16);
        if (verFlip) lineInTile = spriteHeight - 1 - lineInTile;

        uint8_t tileLine = lineInTile % 8;
        uint16_t tileAddr = (spriteHeight == 16) ? (tileIdx & 0xFE) * 16 : tileIdx * 16;
        if (spriteHeight == 16 && lineInTile >= 8) tileAddr += 16;
        tileAddr += tileLine * 2;

        uint8_t low = vram[tileAddr];
        uint8_t high = vram[tileAddr + 1];

        bool priority = sprite.attrs & 0x80;
        
        for (uint8_t relX = 0; relX < 8; relX++) {
            uint8_t absX = sprite.x - 8 + relX;
            bool visible = absX < GB_WIDTH;

            if (visible && !objectPixelsDrawn[absX] && (priority == false || bgLineIndices[absX] == 0)) {
                bool horFlip = sprite.attrs & 0x20;

                uint8_t bitPos = horFlip ? relX : 7 - relX;
                uint8_t highBit = (high >> bitPos) & 1;
                uint8_t lowBit = (low >> bitPos) & 1;

                uint8_t colourIdx = highBit << 1 | lowBit;

                uint8_t obp = sprite.attrs & 0x10 ? obp1 : obp0;
                Colour finalColour = dmgLut[(obp >> colourIdx * 2) & 0x03];

                if (colourIdx) {
                    framebuffer->setPixel(absX, ly, finalColour);
                    objectPixelsDrawn[absX] = true;
                }
            }
        }
    }
}

void PPU::sortSprites(Sprite* sprites, size_t count) {
    std::sort(sprites, sprites + count, [](const Sprite& a, const Sprite& b) {
        if (a.x == b.x) return a.oamIdx < b.oamIdx;
        return a.x < b.x;
    });
}

uint16_t PPU::getMapBase(PPULayer layer) {
    if (layer == PPULayer::BACKGROUND) {
        return lcdc & 0x08 ? 0x1C00 : 0x1800;
    } else {
        return lcdc & 0x40 ? 0x1C00 : 0x1800;
    }
}

void PPU::oamDma(uint8_t high) {
    for (uint8_t i = 0; i <= 0x9F; i++) {
        oam[i] = console.mmu.read8(high << 8 | i);
    }

    console.cpu.yieldCycles(160 * 4); // DMA takes 160 cycles to run
}

uint8_t PPU::readVram(uint16_t addr) {
    // a lot of issues arise when this check is included
    // while it is *kind of* the behaviour that real hardware has
    // it's apparently a lot more nuanced, and it requires
    // a higher cycle granularity to emulate properly

    // if (mode == PPUMode::PIXEL_TRANSFER) {
    //     return 0xFF;
    // }
    return vram[addr];
}

void PPU::writeVram(uint16_t addr, uint8_t val) {
    // if (mode == PPUMode::PIXEL_TRANSFER) {
    //     return;
    // }
    vram[addr] = val;
}

uint8_t PPU::readOam(uint16_t addr) {
    if (mode == PPUMode::OAM_SCAN || mode == PPUMode::PIXEL_TRANSFER) {
        return 0xFF;
    }

    return oam[addr];
}

void PPU::writeOam(uint16_t addr, uint8_t val) {
    if (mode == PPUMode::OAM_SCAN || mode == PPUMode::PIXEL_TRANSFER) {
        return;
    }

    oam[addr] = val;
}

uint8_t PPU::readStat(void) {
    uint8_t res = stat & 0x7B;
    res |= static_cast<uint8_t>(mode) & 0x03;
    res |= ly == lyc ? 0x04 : 0x00;
    res |= 0x80;
    return res;
}

} // namespace GB2040::Core
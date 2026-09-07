#include "core/ppu.h"
#include "core/graphics.h"
#include "core/console.h"

#include <cstdint>
#include <cstring>

namespace GB2040::Core
{

// colourIdx = ((tileDecodeByte[high] << 1) | tileDecodeByte[low]) >> (x * 2) & 0x03
static constexpr uint16_t tileDecodeByte[256] = {
    0x0000, 0x0001, 0x0004, 0x0005, 0x0010, 0x0011, 0x0014, 0x0015,
    0x0040, 0x0041, 0x0044, 0x0045, 0x0050, 0x0051, 0x0054, 0x0055,
    0x0100, 0x0101, 0x0104, 0x0105, 0x0110, 0x0111, 0x0114, 0x0115,
    0x0140, 0x0141, 0x0144, 0x0145, 0x0150, 0x0151, 0x0154, 0x0155,
    0x0400, 0x0401, 0x0404, 0x0405, 0x0410, 0x0411, 0x0414, 0x0415,
    0x0440, 0x0441, 0x0444, 0x0445, 0x0450, 0x0451, 0x0454, 0x0455,
    0x0500, 0x0501, 0x0504, 0x0505, 0x0510, 0x0511, 0x0514, 0x0515,
    0x0540, 0x0541, 0x0544, 0x0545, 0x0550, 0x0551, 0x0554, 0x0555,
    0x1000, 0x1001, 0x1004, 0x1005, 0x1010, 0x1011, 0x1014, 0x1015,
    0x1040, 0x1041, 0x1044, 0x1045, 0x1050, 0x1051, 0x1054, 0x1055,
    0x1100, 0x1101, 0x1104, 0x1105, 0x1110, 0x1111, 0x1114, 0x1115,
    0x1140, 0x1141, 0x1144, 0x1145, 0x1150, 0x1151, 0x1154, 0x1155,
    0x1400, 0x1401, 0x1404, 0x1405, 0x1410, 0x1411, 0x1414, 0x1415,
    0x1440, 0x1441, 0x1444, 0x1445, 0x1450, 0x1451, 0x1454, 0x1455,
    0x1500, 0x1501, 0x1504, 0x1505, 0x1510, 0x1511, 0x1514, 0x1515,
    0x1540, 0x1541, 0x1544, 0x1545, 0x1550, 0x1551, 0x1554, 0x1555,
    0x4000, 0x4001, 0x4004, 0x4005, 0x4010, 0x4011, 0x4014, 0x4015,
    0x4040, 0x4041, 0x4044, 0x4045, 0x4050, 0x4051, 0x4054, 0x4055,
    0x4100, 0x4101, 0x4104, 0x4105, 0x4110, 0x4111, 0x4114, 0x4115,
    0x4140, 0x4141, 0x4144, 0x4145, 0x4150, 0x4151, 0x4154, 0x4155,
    0x4400, 0x4401, 0x4404, 0x4405, 0x4410, 0x4411, 0x4414, 0x4415,
    0x4440, 0x4441, 0x4444, 0x4445, 0x4450, 0x4451, 0x4454, 0x4455,
    0x4500, 0x4501, 0x4504, 0x4505, 0x4510, 0x4511, 0x4514, 0x4515,
    0x4540, 0x4541, 0x4544, 0x4545, 0x4550, 0x4551, 0x4554, 0x4555,
    0x5000, 0x5001, 0x5004, 0x5005, 0x5010, 0x5011, 0x5014, 0x5015,
    0x5040, 0x5041, 0x5044, 0x5045, 0x5050, 0x5051, 0x5054, 0x5055,
    0x5100, 0x5101, 0x5104, 0x5105, 0x5110, 0x5111, 0x5114, 0x5115,
    0x5140, 0x5141, 0x5144, 0x5145, 0x5150, 0x5151, 0x5154, 0x5155,
    0x5400, 0x5401, 0x5404, 0x5405, 0x5410, 0x5411, 0x5414, 0x5415,
    0x5440, 0x5441, 0x5444, 0x5445, 0x5450, 0x5451, 0x5454, 0x5455,
    0x5500, 0x5501, 0x5504, 0x5505, 0x5510, 0x5511, 0x5514, 0x5515,
    0x5540, 0x5541, 0x5544, 0x5545, 0x5550, 0x5551, 0x5554, 0x5555,
};

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
        uint16_t rowOffset = ly * GB_WIDTH;
        for (int x = 0; x < GB_WIDTH; x++) {
            framebuffer->fb[rowOffset + x] = dmgLut[0];
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
    uint16_t rowOffset = ly * GB_WIDTH;

    uint16_t mapBase = getMapBase(layer);

    uint8_t bgY;
    if (layer == PPULayer::WINDOW) {
        bgY = wly;
    } else {
        bgY = scy + ly;
    }
    uint8_t tileY = bgY >> 3;
    uint8_t pixelY = bgY & 7;

    uint8_t cachedTileId = 0xFF;
    uint16_t cachedTileAddr = 0;
    uint8_t cachedLow = 0;
    uint8_t cachedHigh = 0;

    for (int x = 0; x < GB_WIDTH; x++) {
        if (layer == PPULayer::WINDOW) {
            if (x + 7 < wx) continue;
            if (wx < 7 || wx > 166 || wy > 143) continue;
            drewWindowPixel = true;
        }

        uint8_t bgX;
        if (layer == PPULayer::WINDOW) {
            bgX = x - (wx - 7);
        } else {
            bgX = scx + x;
        }

        uint8_t tileX = bgX >> 3;
        uint8_t pixelX = bgX & 7;

        uint8_t tileId = vram[mapBase + tileY * 32 + tileX];

        if (tileId != cachedTileId) {
            if (lcdc & 0x10) {
                cachedTileAddr = tileId * 16;
            } else {
                cachedTileAddr = 0x1000 + (int8_t)tileId * 16;
            }
            cachedTileAddr += pixelY * 2;

            cachedLow = vram[cachedTileAddr];
            cachedHigh = vram[cachedTileAddr + 1];
            cachedTileId = tileId;
        }

        uint8_t colourIdx = ((tileDecodeByte[cachedHigh] << 1) | tileDecodeByte[cachedLow]) >> ((7 - pixelX) * 2) & 0x03;

        bgLineIndices[x] = colourIdx;
        framebuffer->fb[rowOffset + x] = dmgLut[(bgp >> colourIdx * 2) & 0x03];
    }

    if (layer == PPULayer::WINDOW && drewWindowPixel) wly++;
}

void PPU::renderScanlinePixel(PPULayer layer, uint8_t x, uint16_t rowOffset) {
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

    uint8_t colourIdx = ((tileDecodeByte[high] << 1) | tileDecodeByte[low]) >> ((7 - pixelX) * 2) & 0x03;

    bgLineIndices[x] = colourIdx;

    Colour finalColour = dmgLut[(bgp >> colourIdx * 2) & 0x03];

    framebuffer->fb[rowOffset + x] = finalColour;
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
    
    uint16_t rowOffset = ly * GB_WIDTH;

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
        bool horFlip = sprite.attrs & 0x20;

        int16_t spriteScreenX = sprite.x - 8;
        uint8_t startRelX = 0;
        uint8_t endRelX = 8;
        if (spriteScreenX < 0) startRelX = -spriteScreenX;
        if (spriteScreenX + 8 > GB_WIDTH) endRelX = GB_WIDTH - spriteScreenX;

        for (uint8_t relX = startRelX; relX < endRelX; relX++) {
            uint8_t absX = spriteScreenX + relX;

            if (!objectPixelsDrawn[absX] && (priority == false || bgLineIndices[absX] == 0)) {

                uint8_t bitPos = horFlip ? relX : 7 - relX;
                uint8_t colourIdx = ((tileDecodeByte[high] << 1) | tileDecodeByte[low]) >> (bitPos * 2) & 0x03;

                uint8_t obp = sprite.attrs & 0x10 ? obp1 : obp0;
                Colour finalColour = dmgLut[(obp >> colourIdx * 2) & 0x03];

                if (colourIdx) {
                    framebuffer->fb[rowOffset + absX] = finalColour;
                    objectPixelsDrawn[absX] = true;
                }
            }
        }
    }
}

void PPU::sortSprites(Sprite* sprites, size_t count) {
    for (size_t i = 1; i < count; i++) {
        Sprite key = sprites[i];
        size_t j = i;
        while (j > 0 && (sprites[j-1].x > key.x || 
               (sprites[j-1].x == key.x && sprites[j-1].oamIdx > key.oamIdx))) {
            sprites[j] = sprites[j-1];
            j--;
        }
        sprites[j] = key;
    }
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
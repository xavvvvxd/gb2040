#include "core/mbc.h"
#include "core/console.h"
#include "platform/platform.h"

#include <cstdint>
#include <cstring>

namespace GB2040::Core
{

using GB2040::Platform::RAMSource;

MBC5::MBC5(Console& console, ROMSource* romSource, CartridgeHeader& cartHeader)
: console(console), romSource(romSource), header(cartHeader) {
    header = cartHeader;

    switch (cartHeader.ramSize) {
        case 0x02:
            ramSize = 0x2000;
            break;
        case 0x03:
            ramSize = 0x8000;
            break;
        case 0x04:
            ramSize = 0x20000;
            break;
    }

    ramSource = console.platform->getSave(ramSize);
}

uint8_t MBC5::read8(uint16_t addr) {
    if (0x0 <= addr && addr <= 0x3FFF) { // fixed ROM bank 0
        uint8_t v;
        romSource->read(addr, &v, 1);
        return v;
    } else if (0x4000 <= addr && addr <= 0x7FFF) { // switchable ROM
        uint32_t romAddr = romBank * 0x4000 + (addr - 0x4000);
        uint8_t v;
        romSource->read(romAddr, &v, 1);
        return v;
    } else if (0xA000 <= addr && addr <= 0xBFFF) {
        if (!ramEnabled) return 0xFF;

        uint32_t ramAddr = ramBank * 0x2000 + (addr - 0xA000);
        uint8_t v;
        ramSource->read(ramAddr, &v, 1);
        return v;
    }

    return 0xFF;
}

void MBC5::write8(uint16_t addr, uint8_t val) {
    if (0x0 <= addr && addr <= 0x1FFF) {
        if (header.ramSize == 0) return;
        ramEnabled = ((val & 0x0F) == 0x0A);
        return;
    } else if (0x2000 <= addr && addr <= 0x2FFF) {
        uint16_t maxBank = (header.romSize * 1024 / 0x4000) - 1;
        romBank = (romBank & 0x100) | val;

        romBank &= maxBank;
        return;
    } else if (0x3000 <= addr && addr <= 0x3FFF) {
        uint16_t maxBank = (header.romSize * 1024 / 0x4000) - 1;
        romBank = (romBank & 0xFF) | ((val & 0x01) << 8);

        romBank &= maxBank;
        return;
    } else if (0x4000 <= addr && addr <= 0x5FFF) {
        if (val <= 0x0F) {
            uint8_t maxRamBank = (ramSize / 0x2000) - 1;
            ramBank = val & maxRamBank;
        }
    }

    if (0xA000 <= addr && addr <= 0xBFFF) {
        if (!ramEnabled) return;

        uint32_t ramAddr = ramBank * 0x2000 + (addr - 0xA000);
        ramSource->write8(ramAddr, &val, 1);
    }
}

void MBC5::save(void) {
    if (header.cartType != CartType::MBC5_RAM_BATTERY &&
         header.cartType != CartType::MBC5_RUMBLE_RAM_BATTERY) return;

    console.platform->saveData(ramSource);
}

} // namespace GB2040::Core

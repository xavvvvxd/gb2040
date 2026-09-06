#include "core/mbc.h"
#include "core/console.h"
#include "platform/platform.h"

#include <cstdint>
#include <cstring>

namespace GB2040::Core
{

using GB2040::Platform::RAMSource;

MBC1::MBC1(Console& console, ROMSource* romSource, CartridgeHeader& cartHeader)
: console(console), romSource(romSource), header(cartHeader) {
    header = cartHeader;

    switch (cartHeader.ramSize) {
        case 0x02:
            ramSize = 0x2000;
            break;
        case 0x03:
            ramSize = 0x8000;
            break;
    }

    ramSource = console.platform->getSave(ramSize);
}

uint8_t MBC1::read8(uint16_t addr) {
    if (0x0 <= addr && addr <= 0x3FFF) { // fixed ROM bank 0
        uint32_t bank = (mode == 0) ? 0 : ((ramBank & 0x03) << 5);
        uint32_t romAddr = bank * 0x4000 + addr;

        uint8_t v;
        romSource->read8(romAddr, &v, 1);
        return v;
    } else if (0x4000 <= addr && addr <= 0x7FFF) { // switchable ROM
        uint32_t bank = romBank & 0x1F;
        bank |= (mode == 0) ? ((ramBank & 0x03) << 5) : 0;
        if ((bank & 0x1F) == 0) bank = 1;

        uint32_t romAddr = bank * 0x4000 + (addr - 0x4000);
        uint8_t v;
        romSource->read8(romAddr, &v, 1);
        return v;
    } else if (0xA000 <= addr && addr <= 0xBFFF) {
        if (!ramEnabled) return 0xFF;

        uint32_t bank = (mode == 1) ? ramBank & 0x03 : 0;
        uint32_t ramAddr = bank * 0x2000 + (addr - 0xA000);
        if (ramAddr >= ramSize) return 0xFF;
        uint8_t v;
        ramSource->read8(ramAddr, &v, 1);
        return v;
    }

    return 0xFF;
}

void MBC1::write8(uint16_t addr, uint8_t val) {
    if (0x0 <= addr && addr <= 0x1FFF) {
        if (header.ramSize == 0) return;
        ramEnabled = ((val & 0x0F) == 0x0A);
        return;
    } else if (0x2000 <= addr && addr <= 0x3FFF) {
        romBank = (romBank & 0x60) | (val & 0x1F);
        if ((romBank & 0x1F) == 0) romBank = 1;
        return;
    } else if (0x4000 <= addr && addr <= 0x5FFF) {
        ramBank = val & 0x03;
        return;
    } else if (0x6000 <= addr && addr <= 0x7FFF) {
        mode = val & 0x01;
    } else if (0xA000 <= addr && addr <= 0xBFFF) {
        if (!ramEnabled) return;

        uint32_t bank = (mode == 1) ? ramBank & 0x03 : 0;
        uint32_t ramAddr = bank * 0x2000 + (addr - 0xA000);
        if (ramAddr >= ramSize) return;
        ramSource->write8(ramAddr, &val, 1);
    }
}

void MBC1::save(void) {
    if (header.cartType != CartType::MBC1_RAM_BATTERY) return;

    console.platform->saveData(ramSource);
}

} // namespace GB2040::Core

#include "core/mbc.h"
#include "core/console.h"
#include "platform/platform.h"

#include <cstdint>
#include <cstring>

namespace GB2040::Core
{

MBC2::MBC2(Console& console, GB2040::Platform::ROMSource* romSource, CartType cartType)
: console(console), romSource(romSource) {
    this->cartType = cartType;

    ramSource = console.platform->getSave(256);
}

uint8_t MBC2::read8(uint16_t addr) {
    if (0x0 <= addr && addr <= 0x3FFF) {
        uint8_t v;
        romSource->read(addr, &v, 1);
        return v;
    } else if (0x4000 <= addr && addr <= 0x7FFF) {
        uint32_t romAddr = romBank * 0x4000 + (addr - 0x4000);
        uint8_t v;
        romSource->read(romAddr, &v, 1);
        return v;
    } else if (0xA000 <= addr && addr <= 0xBFFF && ramEnabled) {
        // return RAM instead
        addr = (addr - 0xA000) & 0x1FF; // echo
        
        uint8_t v;
        ramSource->read(addr / 2, &v, 1);
        return (v >> (addr % 2 * 4)) & 0x0F;
    } else return 0xFF;
}

void MBC2::write8(uint16_t addr, uint8_t val) {
    if (0x0 <= addr && addr <= 0x3FFF) {
        bool optSelect = addr & 0x0100;

        if (optSelect) {
            romBank = (val & 0x0F) ? (val & 0x0F) : 1;
        } else {
            ramEnabled = (val & 0x0F) == 0x0A;
        }
    } else if (0xA000 <= addr && addr <= 0xBFFF && ramEnabled) {
        addr = (addr - 0xA000) & 0x1FF;
        uint8_t shift = addr % 2 == 0 ? 0 : 4;

        uint8_t v;
        ramSource->read(addr / 2, &v, 1);

        v &= ~(0x0F << shift);
        v |= (val & 0x0F) << shift;
        ramSource->write8(addr / 2, &v, 1);
        return;
    }
}

void MBC2::save() {
    if (cartType == CartType::MBC2_BATTERY) {
        console.platform->saveData(ramSource);
    }
}

} // namespace GB2040::Core

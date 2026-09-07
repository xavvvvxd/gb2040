#include "core/mbc.h"
#include "core/console.h"
#include "platform/platform.h"

#include <cstdint>
#include <cstring>

namespace GB2040::Core
{

NoMBC::NoMBC(Console& console, GB2040::Platform::ROMSource* romSource, CartType cartType)
: console(console), romSource(romSource) {
    memset(ram, 0, sizeof(ram));
}

uint8_t NoMBC::read8(uint16_t addr) {
    if (0xA000 <= addr && addr <= 0xBFFF) {
        // return RAM instead
        return ram[addr - 0xA000];
    }

    uint8_t value;
    romSource->read(addr, &value, 1);

    return value;
}

void NoMBC::write8(uint16_t addr, uint8_t val) {
    if (0xA000 <= addr && addr <= 0xBFFF) {
        ram[addr - 0xA000] = val;
        return;
    }

    // no MBC registers (no MBC lmao)
}

} // namespace GB2040::Core

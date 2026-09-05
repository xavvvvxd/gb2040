#include "core/mmu.h"
#include "core/mbc.h"
#include "core/console.h"

#include <cstring>

namespace GB2040::Core
{

MMU::MMU(Console& console)
: console(console) {
    memset(internalWram, 0, WRAM_SIZE);
    memset(hram, 0, HRAM_SIZE);
}

uint8_t MMU::read8(uint16_t addr) {
    if (0x0 <= addr && addr <= 0xFF && bootRomMapped) { // boot ROM
        return console.bootRom[addr];
    } else if (0x0 <= addr && addr <= 0x7FFF) { // ROM
        return console.mbc->read8(addr);
    } else if (0x8000 <= addr && addr <= 0x9FFF) { // VRAM
        return console.ppu.readVram(addr - 0x8000);
    } else if (0xA000 <= addr && addr <= 0xBFFF) { // external RAM
        return console.mbc->read8(addr);
    } else if (0xC000 <= addr && addr <= 0xDFFF) { // work RAM (always bank 0)
        return internalWram[addr - 0xC000];
    } else if (0xE000 <= addr && addr <= 0xFDFF) { // echo RAM
        return read8(addr - 0x2000);
    } else if (0xFE00 <= addr && addr <= 0xFE9F) { // OAM
        return console.ppu.readOam(addr - 0xFE00);
    } else if (0xFF00 <= addr && addr <= 0xFF7F) { // I/O registers
        return readIo(addr - 0xFF00);
    } else if (0xFF80 <= addr && addr <= 0xFFFE) { // high RAM
        return hram[addr - 0xFF80];
    } else if (addr == 0xFFFF) {
        return console.cpu.ie;
    } else { // reserved space, etc
        return 0xFF;
    }
}

uint16_t MMU::read16(uint16_t addr) {
    uint8_t lo = read8(addr);
    uint8_t hi = read8(addr + 1);

    return (hi << 8) | lo;
}

void MMU::write8(uint16_t addr, uint8_t val) {
    if (0x0 <= addr && addr <= 0x100 && bootRomMapped) {
        return; // discard attempted writes to boot ROM, SHOULD never happen but ya never know
    } else if (0x0 <= addr && addr <= 0x7FFF) { // ROM
        console.mbc->write8(addr, val);
    } else if (0x8000 <= addr && addr <= 0x9FFF) { // VRAM
        console.ppu.writeVram(addr - 0x8000, val);
    } else if (0xA000 <= addr && addr <= 0xBFFF) { // external RAM
        console.mbc->write8(addr, val);
    } else if (0xC000 <= addr && addr <= 0xDFFF) { // work RAM
        internalWram[addr - 0xC000] = val;
    } else if (0xE000 <= addr && addr <= 0xFDFF) { // echo RAM
        write8(addr - 0x2000, val); // unsupported by nintendo
    } else if (0xFE00 <= addr && addr <= 0xFE9F) { // OAM
        console.ppu.writeOam(addr - 0xFE00, val);
    } else if (0xFF00 <= addr && addr <= 0xFF7F) { // I/O registers
        writeIo(addr - 0xFF00, val);
    } else if (0xFF80 <= addr && addr <= 0xFFFE) { // high RAM
        hram[addr - 0xFF80] = val;
    } else if (addr == 0xFFFF) {
        console.cpu.ie = val;
    } else { // reserved space, etc
        return;
    }
}

void MMU::write16(uint16_t addr, uint16_t val) {
    write8(addr, val & 0xFF);
    write8(addr + 1, val >> 8);
}

} // namespace GB2040::Core
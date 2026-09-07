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
    switch (addr >> 12) {
        case 0x0: // 0x0000-0x0FFF
        case 0x1: // 0x1000-0x1FFF
        case 0x2: // 0x2000-0x2FFF
        case 0x3: // 0x3000-0x3FFF
        case 0x4: // 0x4000-0x4FFF
        case 0x5: // 0x5000-0x5FFF
        case 0x6: // 0x6000-0x6FFF
        case 0x7: // 0x7000-0x7FFF
            if (addr <= 0xFF && bootRomMapped) return console.bootRom[addr];
            if (romData && addr < 0x4000 && addr < romSize) return romData[addr];
            return console.mbc->read8(addr);
        case 0x8: // 0x8000-0x8FFF
        case 0x9: // 0x9000-0x9FFF
            return console.ppu.readVram(addr - 0x8000);
        case 0xA: // 0xA000-0xAFFF
        case 0xB: // 0xB000-0xBFFF
            return console.mbc->read8(addr);
        case 0xC: // 0xC000-0xCFFF
        case 0xD: // 0xD000-0xDFFF
            return internalWram[addr - 0xC000];
        case 0xE: // 0xE000-0xEFFF (echo of 0xC000-0xCFFF)
            return internalWram[addr - 0xE000];
        case 0xF: { // 0xF000-0xFFFF
            if (0xFDFF >= addr && addr >= 0xE000) { // 0xE000-0xFDFF echo range
                return internalWram[addr - 0xE000];
            }
            uint16_t lo12 = addr & 0xFFF;
            if (lo12 >= 0xE00 && lo12 <= 0xE9F) return console.ppu.readOam(addr - 0xFE00);
            if (lo12 >= 0xF00 && lo12 <= 0xF7F) return readIo(lo12 - 0xF00);
            if (lo12 >= 0xF80 && lo12 <= 0xFFE) return hram[lo12 - 0xF80];
            if (lo12 == 0xFFF) return console.cpu.ie;
            return 0xFF;
        }
    }
    return 0xFF;
}

uint16_t MMU::read16(uint16_t addr) {
    if (0xC000 <= addr && addr <= 0xDFFE) {
        return internalWram[addr - 0xC000] | (internalWram[addr - 0xC000 + 1] << 8);
    }

    if (0xFF80 <= addr && addr <= 0xFFFD) {
        return hram[addr - 0xFF80] | (hram[addr - 0xFF80 + 1] << 8);
    }
    
    uint8_t lo = read8(addr);
    uint8_t hi = read8(addr + 1);
    return (hi << 8) | lo;
}

void MMU::write8(uint16_t addr, uint8_t val) {
    switch (addr >> 12) {
        case 0x0: // 0x0000-0x0FFF
        case 0x1: // 0x1000-0x1FFF
        case 0x2: // 0x2000-0x2FFF
        case 0x3: // 0x3000-0x3FFF
        case 0x4: // 0x4000-0x4FFF
        case 0x5: // 0x5000-0x5FFF
        case 0x6: // 0x6000-0x6FFF
        case 0x7: // 0x7000-0x7FFF
            if (addr <= 0x100 && bootRomMapped) return;
            console.mbc->write8(addr, val);
            return;
        case 0x8: // 0x8000-0x8FFF
        case 0x9: // 0x9000-0x9FFF
            console.ppu.writeVram(addr - 0x8000, val);
            return;
        case 0xA: // 0xA000-0xAFFF
        case 0xB: // 0xB000-0xBFFF
            console.mbc->write8(addr, val);
            return;
        case 0xC: // 0xC000-0xCFFF
        case 0xD: // 0xD000-0xDFFF
            internalWram[addr - 0xC000] = val;
            return;
        case 0xE: // 0xE000-0xEFFF (echo of 0xC000-0xCFFF)
            internalWram[addr - 0xE000] = val;
            return;
        case 0xF: { // 0xF000-0xFFFF
            if (0xFDFF >= addr && addr >= 0xE000) { // 0xE000-0xFDFF echo range
                internalWram[addr - 0xE000] = val;
                return;
            }
            uint16_t lo12 = addr & 0xFFF;
            if (lo12 >= 0xE00 && lo12 <= 0xE9F) { console.ppu.writeOam(addr - 0xFE00, val); return; }
            if (lo12 >= 0xF00 && lo12 <= 0xF7F) { writeIo(lo12 - 0xF00, val); return; }
            if (lo12 >= 0xF80 && lo12 <= 0xFFE) { hram[lo12 - 0xF80] = val; return; }
            if (lo12 == 0xFFF) { console.cpu.ie = val; return; }
            return;
        }
    }
}

void MMU::write16(uint16_t addr, uint16_t val) {
    if (0xC000 <= addr && addr <= 0xDFFE) {
        internalWram[addr - 0xC000] = val & 0xFF;
        internalWram[addr - 0xC000 + 1] = val >> 8;
        return;
    }

    if (0xFF80 <= addr && addr <= 0xFFFD) {
        hram[addr - 0xFF80] = val & 0xFF;
        hram[addr - 0xFF80 + 1] = val >> 8;
        return;
    }

    write8(addr, val & 0xFF);
    write8(addr + 1, val >> 8);
}

} // namespace GB2040::Core
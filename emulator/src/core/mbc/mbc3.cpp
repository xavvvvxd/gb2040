#include "core/mbc.h"
#include "core/console.h"
#include "platform/platform.h"

#include <cstdint>
#include <cstring>

namespace GB2040::Core
{

using GB2040::Platform::RAMSource;

MBC3::MBC3(Console& console, ROMSource* romSource, CartType cartType)
: console(console), romSource(romSource) {
    this->cartType = cartType;

    ramSource = console.platform->getSave(32768 + sizeof(RTC));

    rtc = parseRTC();
    rtcLatched = RTC{};
}

uint8_t MBC3::read8(uint16_t addr) {
    if (0x0 <= addr && addr <= 0x3FFF) { // fixed ROM bank 0
        uint8_t v;
        romSource->read8(addr, &v, 1);
        return v;
    } else if (0x4000 <= addr && addr <= 0x7FFF) { // switchable ROM
        uint32_t romAddr = romBank * 0x4000 + (addr - 0x4000);
        uint8_t v;
        romSource->read8(romAddr, &v, 1);
        return v;
    } else if (0xA000 <= addr && addr <= 0xBFFF) {
        if (!ramEnabled) return 0xFF;
        if (rtcSelected) return readRTC(rtcReg);

        uint32_t ramAddr = ramBank * 0x2000 + (addr - 0xA000);
        uint8_t v;
        ramSource->read8(ramAddr, &v, 1);
        return v;
    }

    return 0xFF;
}

void MBC3::write8(uint16_t addr, uint8_t val) {
    if (0x0 <= addr && addr <= 0x1FFF) {
        ramEnabled = ((val & 0x0F) == 0x0A);
        return;
    } else if (0x2000 <= addr && addr <= 0x3FFF) {
        romBank = val & 0x7F;
        if (romBank == 0) romBank = 1;
        return;
    } else if (0x4000 <= addr && addr <= 0x5FFF) {
        if (val <= 0x03) {
            ramBank = val;
            rtcSelected = false;
        } else if (0x08 <= val && val <= 0x0C) {
            rtcSelected = true;
            rtcReg = val;
        }

        return;
    } else if (0x6000 <= addr && addr <= 0x7FFF) {
        bool hasRtc = (cartType == CartType::MBC3_TIMER_BATTERY ||
                       cartType == CartType::MBC3_TIMER_RAM_BATTERY);

        if (hasRtc && !rtcLatchPrep && val == 0x01) {
            tickRTC();
            rtcLatched = rtc;
            rtcLatchValid = true;
        }

        rtcLatchPrep = val == 0;
        return;
    }

    if (0xA000 <= addr && addr <= 0xBFFF) {
        if (!ramEnabled) return;
        if (rtcSelected) {
            writeRTC(rtcReg, val);
            return;
        }

        uint32_t ramAddr = ramBank * 0x2000 + (addr - 0xA000);
        ramSource->write8(ramAddr, &val, 1);
    }
}

void MBC3::save(void) {
    if (cartType != CartType::MBC3_RAM_BATTERY &&
         cartType != CartType::MBC3_TIMER_BATTERY &&
          cartType != CartType::MBC3_TIMER_RAM_BATTERY) return;

    ramSource->write8(
        ramSource->size() - sizeof(RTC),
        reinterpret_cast<uint8_t*>(&rtc),
        sizeof(RTC)
    );

    console.platform->saveData(ramSource);
}

RTC MBC3::parseRTC(void) {
    RTC parsed{};
    ramSource->read8(ramSource->size() - sizeof(RTC), reinterpret_cast<uint8_t*>(&parsed), sizeof(RTC));

    if (parsed.lastUpdateUs == 0) {
        parsed.lastUpdateUs = console.platform->getClock();
    }

    return parsed;
}

void MBC3::tickRTC(void) {
    if (rtc.halt) return;

    uint64_t now = console.platform->getClock(); // us
    uint64_t elapsed = now - rtc.lastUpdateUs;

    if (elapsed >= 1e6) {
        uint64_t seconds = elapsed / 1e6;
        rtc.lastUpdateUs += elapsed;

        for (uint64_t i = 0; i < seconds; i++) {
            bool carryMinutes = false;
            bool carryHours   = false;
            bool carryDays    = false;

            if (rtc.seconds < 59) rtc.seconds++;
            else if (rtc.seconds == 59) {
                rtc.seconds = 0;
                carryMinutes = true;
            } else {
                rtc.seconds = (rtc.seconds + 1) & 0x3F;
            }

            if (carryMinutes) {
                if (rtc.minutes < 59) rtc.minutes++;
                else if (rtc.minutes == 59) {
                    rtc.minutes = 0;
                    carryHours = true;
                } else {
                    rtc.minutes = (rtc.minutes + 1) & 0x3F;
                }
            }

            if (carryHours) {
                if (rtc.hours < 23) rtc.hours++;
                else if (rtc.hours == 23) {
                    rtc.hours = 0;
                    carryDays = true;
                } else {
                    rtc.hours = (rtc.hours + 1) & 0x1F;
                }
            }

            if (carryDays) {
                rtc.days++;

                if (rtc.days >= 512) {
                    rtc.days = 0;
                    rtc.carry = true;
                }
            }
        }
    }
}

uint8_t MBC3::readRTC(uint8_t reg) {
    tickRTC();

    RTC* r = rtcLatchValid ? &rtcLatched : &rtc;

    switch (reg) {
        case 0x08: return r->seconds;
        case 0x09: return r->minutes;
        case 0x0A: return r->hours;
        case 0x0B: return r->days & 0xFF;
        case 0x0C: return ((r->days & 0x0100) >> 8) | ((r->halt & 0x01) << 6) | ((r->carry & 0x01) << 7);
    }

    return 0xFF;
}

void MBC3::writeRTC(uint8_t reg, uint8_t val) {
    tickRTC();

    switch (reg) {
        case 0x08:
            rtc.seconds = val & 0x3F;
            break;
        case 0x09:
            rtc.minutes = val & 0x3F;
            break;
        case 0x0A:
            rtc.hours = val & 0x1F;
            break;
        case 0x0B:
            rtc.days = (rtc.days & 0x100) | val;
            break;
        case 0x0C:
            bool oldHalt = rtc.halt;

            rtc.days = (rtc.days & 0xFF) | ((val & 0x01) << 8);
            rtc.halt = (val & 0x40) != 0;
            rtc.carry = (val & 0x80) != 0;

            if (oldHalt && !rtc.halt) {
                rtc.lastUpdateUs = console.platform->getClock();
            }

            if (!oldHalt && rtc.halt) {
                rtc.lastUpdateUs = console.platform->getClock();
            }

            break;
    }
}

} // namespace GB2040::Core

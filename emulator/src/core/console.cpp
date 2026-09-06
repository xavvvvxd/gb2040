#include "core/console.h"

#include "core/cpu.h"
#include "core/mmu.h"
#include "core/mbc.h"

#include <cstdint>
#include <cstring>
#include <cstdio>

namespace GB2040::Core {

Console::Console(Platform* platform, ROMSource* romSource)
: platform(platform),
  cpu(*this),
  mmu(*this),
  timer(*this),
  ppu(*this),
  apu(*this),
  mode(GBMode::DMG),
  input(0xFF) {
    romSource->read8(0x104, reinterpret_cast<uint8_t*>(&header), sizeof(CartridgeHeader));

    switch (header.cartType) {
        case CartType::MBC1:
        case CartType::MBC1_RAM:
        case CartType::MBC1_RAM_BATTERY:
            mbc = new MBC1(*this, romSource, header);
            break;
        case CartType::MBC2:
        case CartType::MBC2_BATTERY:
            mbc = new MBC2(*this, romSource, header.cartType);
            break;
        case CartType::MBC3:
        case CartType::MBC3_RAM:
        case CartType::MBC3_RAM_BATTERY:
        case CartType::MBC3_TIMER_BATTERY:
        case CartType::MBC3_TIMER_RAM_BATTERY:
            mbc = new MBC3(*this, romSource, header.cartType);
            break;
        case CartType::MBC5:
        case CartType::MBC5_RAM:
        case CartType::MBC5_RAM_BATTERY:
        case CartType::MBC5_RUMBLE:
        case CartType::MBC5_RUMBLE_RAM:
        case CartType::MBC5_RUMBLE_RAM_BATTERY:
            mbc = new MBC5(*this, romSource, header);
            break;
        default:
            mbc = new NoMBC(*this, romSource, header.cartType);
            break;
    }
}

void Console::run(void) {
    running = true;

    uint64_t target = platform->getClock();

    int frames = 0;
    uint64_t fpsTimer = target;
    int fps = 0;

    while (running) {
        doTicks(CYCLES_PER_FRAME); // one frame
        frames++;

        uint64_t now = platform->getClock();
        if (now - fpsTimer >= 1e6) {
            fps = frames;
            frames = 0;
            fpsTimer += 1e6;

            printf("FPS: %d\n", fps);
        }

        target += GB_FRAME_TIME_US;

        running = platform->doEvents(*this);

        now = platform->getClock();
        if (target > now) platform->wait(target - now);
        else target = now;
    }
}

size_t Console::doTicks(size_t cycles) {
    size_t cyclesPassed = 0;
    while (cyclesPassed < cycles) {
        cyclesPassed += tick();
    }

    return cyclesPassed;
}

size_t Console::tick(void) {
    size_t cycles = cpu.tick();
    timer.incTimers(cycles);
    ppu.tick(cycles);
    apu.tick(cycles);

    return cycles;
}

void Console::requestInterrupt(Interrupt interrupt) {
    cpu.intFlag |= (1 << static_cast<int>(interrupt));
}

void Console::pressButton(Button button) {
    bool old = input & (1 << static_cast<int>(button));
    input &= ~(1 << static_cast<int>(button)); 

    if (old) requestInterrupt(Interrupt::JOYPAD);
}

void Console::releaseButton(Button button) {
    input |= (1 << static_cast<int>(button));
}

uint8_t Console::getInputRegister(void) {
    uint8_t joypad = 0xC0;

    joypad |= inputSelectDpad << 4;
    joypad |= inputSelectButtons << 5;

    if (!inputSelectButtons) joypad |= input & 0x0F;
    else if (!inputSelectDpad) joypad |= (input >> 4) & 0x0F;
    else joypad |= 0x0F;

    return joypad;
}

void Console::save(void) {
    mbc->save();
}
	
}
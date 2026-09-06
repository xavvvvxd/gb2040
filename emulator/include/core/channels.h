#pragma once

#include "mmu.h"

#include <cstdint>
#include <array>

namespace GB2040::Core
{

class APU;

class PulseChannel {
public:
    void tick(void);
    void lenTick(void);
    uint8_t readReg(uint8_t);
    void writeReg(uint8_t, uint8_t);

    uint8_t out(void);

    void sweepTick(void);
    void envTick(void);

    void disable(void);
private:
    static constexpr std::array<uint8_t, 4> dutyPatterns {0b00000001, 0b10000001, 0b10000111, 0b01111110};

    void init(void);
    uint16_t nextSweep(void);
    uint16_t getFreq(void);

    friend MMU;

    uint8_t sweepPace = 0x00; // NR10
    bool sweepDir = 0x00; // NR10
    uint8_t sweepStep = 0x00; // NR10

    uint8_t duty = 0x00; // NR11 / NR21
    uint8_t length = 0x00; // NR11 / NR21

    uint8_t envInitVolume = 0x00; // NR12 / NR22
    bool envDir = false; // NR12 / NR22
    uint8_t envPeriod = 0x00;

    uint16_t freq = 0x0000; // NR13/NR14 / NR23/NR24
    
    uint8_t volume = 0x00;
    uint8_t dutyPos = 0x00;
    uint16_t timer = freq;
    uint16_t lengthTimer = 0x00;
    uint16_t envelopeTimer = 0x00;
    uint16_t sweepTimer = 0x00;
    uint16_t sweepShadow = 0x00;
    
    bool envelopeActive = false;
    bool lengthEnable = false;
    bool sweepEnabled = false;
    bool dacEnabled = false;
    bool enabled = false;
};

class WaveChannel {
public:
    void tick(void);
    void lenTick(void);
    uint8_t readReg(uint8_t);
    void writeReg(uint8_t, uint8_t);

    uint8_t out(void);

    void disable(void);
private:
    void init(void);

    uint16_t getFreq(void);

    friend MMU;

    uint8_t outputSample = 0x00;

    uint8_t length = 0x00; // NR31

    uint8_t outputLevel = 0x00;
    uint16_t freq = 0x0000;
    uint8_t waveform[0x10] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    uint8_t waveformPtr = 0x00;

    uint16_t timer = 0x00;
    uint16_t lengthTimer = 0x00;

    bool lengthEnable = false;
    bool dacEnabled = false;
    bool enabled = false;
};

class NoiseChannel {
public:
    void tick(void);
    void lenTick(void);
    uint8_t readReg(uint8_t);
    void writeReg(uint8_t, uint8_t);

    uint8_t out(void);
    
    void envTick(void);

    void disable(void);
private:
    static constexpr std::array<uint8_t, 8> divisors { 8, 16, 32, 48, 64, 80, 96, 112 };

    void init(void);

    friend MMU;

    uint8_t length = 0x00;

    uint8_t envInitVolume = 0x00;
    bool envDir = false;
    uint8_t envPeriod = 0x00;

    uint8_t clockShift = 0x00;
    bool lfsrWidth = false;
    uint8_t clockDiv = 0x00;

    uint8_t volume = 0x00;
    uint16_t timer = 0x00;
    uint16_t lengthTimer = 0x00;
    uint16_t envelopeTimer = 0x00;
    uint16_t lfsr = 0x00;

    bool envelopeActive = false;
    bool lengthEnable = false;
    bool dacEnabled = false;
    bool enabled = false;
};

} // namespace GB2040::Core

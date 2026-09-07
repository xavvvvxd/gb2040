#pragma once

#include "channels.h"
#include "audio.h"
#include "mmu.h"

#include <cstdint>
#include <cmath>

#define SAMPLE_RATE 48000.0f
#define SAMPLE_FREQ 4194304.0f / SAMPLE_RATE

namespace GB2040::Core {

class Console;

class APU {
public:
    APU(Console&);

    void tick(size_t);
    void setEnabled(bool);

private:
    StereoSample mix(uint8_t, uint8_t, uint8_t, uint8_t);

    friend MMU; // IO ports

    bool enabled = false;

    uint8_t lVolume = 7;
    uint8_t rVolume = 7;

    uint8_t pan = 0;

    Console& console;
    PulseChannel pulse1, pulse2;
    WaveChannel wave;
    NoiseChannel noise;

    uint8_t divApu = 0;
    uint32_t divApuTimer = 8192;

    float sampleTimer = SAMPLE_FREQ;
};

} // namespace GB2040::Core
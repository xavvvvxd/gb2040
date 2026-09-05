#include "core/apu.h"
#include "core/audio.h"
#include "core/console.h"

#include <cstdint>
#include <cmath>
#include <cstring>
#include <algorithm>

namespace GB2040::Core
{

APU::APU(Console& console) : 
console(console) {
    setEnabled(false);
};

void APU::tick(size_t cycles) {
    if (!enabled) return;

    while (cycles > 0) {
        cycles--;
        divApuTimer--;
        sampleTimer--;

        if (divApuTimer <= 0) {
            divApuTimer = 8192;
            switch (divApu) {
                case 2:
                case 6: pulse1.sweepTick(); break;
                case 0:
                case 4:
                    pulse1.lenTick();
                    pulse2.lenTick();
                    wave.lenTick();
                    noise.lenTick();
                    break;
                case 7:
                    pulse1.envTick();
                    pulse2.envTick();
                    noise.envTick();
                    break;
            }

            divApu++;
            if (divApu >= 8) divApu = 0;
        }

        pulse1.tick();
        pulse2.tick();
        wave.tick();
        noise.tick();

        if (sampleTimer <= 0.0f) {
            sampleTimer += SAMPLE_FREQ;
            uint16_t l = 0, r = 0;

            StereoSample sample = mix(pulse1.out(), pulse2.out(), wave.out(), noise.out());
            
            console.platform->pushSamples(&sample, 1);
        }
    }
}

void APU::setEnabled(bool enabled) {
    this->enabled = enabled;

    if (!enabled) {
        pulse1.disable();
        pulse2.disable();
        wave.disable();
        noise.disable();

        divApu = 0;
        // Turning the APU off, however, does not affect [...] the DIV-APU counter.
        sampleTimer = SAMPLE_FREQ;
        lVolume = 7;
        rVolume = 7;
        pan = 0;
    }
}

StereoSample APU::mix(uint8_t pulse1, uint8_t pulse2, uint8_t wave, uint8_t noise) {
    StereoSample sample { 128, 128 };

    if (!enabled) return sample;

    uint16_t lSum = 0.0f, rSum = 0.0f;

    if (pan & 0x01) lSum += pulse1;
    if (pan & 0x02) lSum += pulse2;
    if (pan & 0x04) lSum += wave;
    if (pan & 0x08) lSum += noise;

    if (pan & 0x10) rSum += pulse1;
    if (pan & 0x20) rSum += pulse2;
    if (pan & 0x40) rSum += wave;
    if (pan & 0x80) rSum += noise;

    sample.l = std::min(255, static_cast<int>(lSum * (lVolume / 7.0f)));
    sample.r = std::min(255, static_cast<int>(rSum * (rVolume / 7.0f)));

    return sample;
}

} // namespace GB2040::Core

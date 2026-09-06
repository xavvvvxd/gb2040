#include "core/channels.h"

#include <cstdint>
#include <cstring>

namespace GB2040::Core
{

void WaveChannel::tick(void) {
    timer--;

    if (timer == 0) {
        timer = getFreq();

        if (!enabled || !dacEnabled) {
            outputSample = 0;
            return;
        }

        waveformPtr = (waveformPtr + 1) & 31;

        uint8_t waveSample = waveform[waveformPtr >> 1];
        bool nibble = waveformPtr & 1;

        if (nibble) waveSample >>= 4;

        waveSample &= 0x0F;

        if (outputLevel) waveSample >>= outputLevel - 1;
        else waveSample = 0;

        outputSample = waveSample;
    };
}

void WaveChannel::lenTick(void) {
    if (lengthTimer > 0 && lengthEnable) {
        lengthTimer--;
        if (lengthTimer == 0) enabled = false;
    }
}

uint16_t WaveChannel::getFreq(void) {
    return (2048 - freq) * 2;
}

uint8_t WaveChannel::out(void) {
    return outputSample;
}

uint8_t WaveChannel::readReg(uint8_t idx) {
    switch (idx) {
        case 0x0:
            return 0x7F | ((dacEnabled & 0x01) << 7);
        case 0x1:
            return 0xFF; // write-only
        case 0x2:
            return 0x1F | ((outputLevel & 0x03) << 6) | 0x80;
        case 0x3:
            return 0xFF; // write-only
        case 0x4:
            return ((freq >> 8) & 0x07) | 0x38 | ((lengthEnable & 0x01) << 6) | 0x80;
        default:
            if (0x16 <= idx && idx <= 0x25) {
                return waveform[idx - 0x16];
            }
    }

    return 0xFF;
}

void WaveChannel::writeReg(uint8_t idx, uint8_t val) {
    switch (idx) {
        case 0x0:
            dacEnabled = (val >> 7) & 0x01;
            break;
        case 0x1:
            length = val;
            break;
        case 0x2:
            outputLevel = (val >> 5) & 0x03;
            break;
        case 0x3:
            freq = (freq & 0x0700) | val;
            break;
        case 0x4:
            freq = (freq & 0x00FF) | ((val & 0x07) << 8);
            lengthEnable = (val >> 6) & 0x01;

            if (val & 0x80) init();
            break;
        default:
            if (0x16 <= idx && idx <= 0x25) {
                waveform[idx - 0x16] = val;
            }
    }
}

void WaveChannel::init(void) {
    enabled = true;
    timer = getFreq();
    waveformPtr = 0;

    if (lengthTimer == 0) lengthTimer = 256;
}

void WaveChannel::disable(void) {
    uint8_t waveformTemp[0x10];
    memcpy(waveformTemp, waveform, sizeof(waveform));

    *this = WaveChannel{};

    memcpy(waveform, waveformTemp, sizeof(waveform));
}

} // namespace GB2040::Core

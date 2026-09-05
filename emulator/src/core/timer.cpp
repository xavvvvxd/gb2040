#include "core/timer.h"
#include "core/console.h"

namespace GB2040::Core
{

Timer::Timer(Console& console)
: console(console) {  }
    
void Timer::incTimers(size_t cycles) {
    sysCounter += cycles;

    bool timerEnabled = tac & 0x04;
    if (!timerEnabled) return;
    
    uint8_t old = tima;

    uint16_t freqRates[4] = { 1024, 16, 64, 256 };
    uint16_t freq = freqRates[tac & 0x03];

    timaCounter += cycles;
    bool timaOverflowN = false;

    while (timaCounter >= freq) {
        timaCounter -= freq;

        if (tima == 0xFF) {
            tima = 0x00;
            timaOverflowN = true;
        } else {
            tima++;
        }
    }

    if (timaOverflow) {
        timaOverflow = false;
        tima = tma;
        console.requestInterrupt(Interrupt::TIMER);
    }

    if (timaOverflowN) timaOverflow = true;
}

uint8_t Timer::getDiv(void) {
    return sysCounter >> 8;
}

void Timer::resetSysCounter(void) {
    sysCounter = 0;
}

} // namespace GB2040::Core

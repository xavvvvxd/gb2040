#pragma once

#include <cstdint>
#include <vector>

namespace GB2040::Core
{

typedef uint16_t Colour;

#define GB_WIDTH  160
#define GB_HEIGHT 144

class Framebuffer {
public:
    Colour fb[GB_HEIGHT * GB_WIDTH];

    inline void clear() {
        Colour white = 0xFFFF;

        std::fill(fb, fb + sizeof(fb), white);
    }

    inline void setPixel(unsigned int x, unsigned int y, Colour colour) {
        fb[y * GB_WIDTH + x] = colour;
    }

    inline Colour getPixel(unsigned int x, unsigned int y) {
        return fb[y * GB_WIDTH + x];
    }

    inline Colour* data() { return fb; }
    inline size_t size() { return GB_WIDTH * GB_HEIGHT; }
};

} // namespace GB2040::Core

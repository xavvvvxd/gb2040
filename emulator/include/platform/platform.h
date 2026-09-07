#pragma once

#include <cstdint>
#include <string>
#include <chrono>

#include "core/graphics.h"
#include "core/audio.h"

namespace GB2040::Core { class Console; }

namespace GB2040::Platform
{

class ROMSource {
public:
    virtual ~ROMSource(void);

    virtual void read(uint32_t, uint8_t*, size_t) = 0;
    virtual size_t size(void) = 0;
    virtual const uint8_t* data(void) { return nullptr; }
};

class RAMSource : public ROMSource {
public:
    virtual void write8(uint32_t, uint8_t*, size_t) = 0;
};

class Platform {
public:
    virtual ~Platform(void);

    virtual void init(int, char**) = 0;
    virtual void run(void) = 0;
    virtual void deinit(void) = 0;
    virtual void wait(uint64_t) = 0;
    virtual uint64_t getClock(void) = 0;
    virtual bool doEvents(GB2040::Core::Console&) = 0;
    virtual GB2040::Core::Framebuffer*& getBackBuffer(void) { return back; }
    virtual void draw(void) = 0;
    virtual void pushSamples(GB2040::Core::StereoSample*, size_t) = 0;
    virtual ROMSource* selectROM(void) = 0;
    virtual RAMSource* getSave(size_t) = 0;
    virtual void saveData(RAMSource*) = 0;
protected:
    GB2040::Core::Framebuffer fbA{};
    GB2040::Core::Framebuffer fbB{};

    GB2040::Core::Framebuffer* front;
    GB2040::Core::Framebuffer* back;

    Platform() : back(&fbA), front(&fbB) {  }
};
    
Platform* createPlatform(void); // platform-specific factory

} // namespace GB2040::Platform
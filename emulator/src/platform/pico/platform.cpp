#include "platform/platform.h"
#include "platform/pico/config.h"
#include "platform/pico/ili9341.h"

#include "core/graphics.h"
#include "core/audio.h"
#include "core/console.h"

#include "pico/stdlib.h"
#include "pico/platform.h"
#include "pico/time.h"
#include "hardware/dma.h"
#include "hardware/clocks.h"

#include <cstdint>
#include <cstring>
#include <string>

#ifdef HAS_EMBEDDED_ROM
#include "embedded_rom.h"
#endif

// Workaround to https://github.com/raspberrypi/pico-sdk/issues/1368
// void* __dso_handle = 0;
// void* _fini = 0;

namespace GB2040::Platform
{

ROMSource::~ROMSource(void) = default;

Platform::~Platform(void) = default;

#ifdef HAS_EMBEDDED_ROM
class EmbeddedROM : public ROMSource {
public:
    EmbeddedROM(void) {  }

    ~EmbeddedROM(void) = default;

    void read8(uint32_t addr, uint8_t* buffer, size_t size) {
        for (size_t i = 0; i < size; i++) {
            uint32_t a = addr + i;
            buffer[i] = (a < embedded_rom_size) ? embedded_rom[a] : 0xFF;
        }
    }

    size_t size(void) {
        return embedded_rom_size;
    }
};
#else
class NOPROM : public ROMSource {
public:
    NOPROM(void) {  }

    ~NOPROM(void) = default;

    void read8(uint32_t addr, uint8_t* buffer, size_t size) {
        memset(buffer, 0xFF, size);
    }

    size_t size(void) {
        return 32768;
    }
};
#endif

using namespace GB2040::Platform::Pico;

class PicoPlatform : public Platform {
public:
    PicoPlatform(void) : display(PIN_DISPLAY_SCK, PIN_DISPLAY_SDA, PIN_DISPLAY_CS, PIN_DISPLAY_DC, PIN_DISPLAY_RST, PIN_DISPLAY_BL) {  }

    void init(int argc, char** argv) override {
        bool success;
        stdio_init_all();
        printf("Init");

        success = set_sys_clock_khz(300000, true);

        if (!success) {
            printf("Failed to set system clock\n");
        }

        success = clock_configure(clk_peri, 0,
                        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
                        clock_get_hz(clk_sys),
                        clock_get_hz(clk_sys));
        
        if (!success) {
            printf("Failed to configure peripheral clock\n");
        }

        display.clear();
    }

    void run(void) override {
        using namespace GB2040::Core;

#ifdef HAS_EMBEDDED_ROM
        EmbeddedROM* romSource = new EmbeddedROM();
#else
        NOPROM* romSource = new NOPROM();
#endif
        Console* console = new Console(this, romSource);

        console->run();

        deinit();
    }

    void deinit(void) override {
        while (true) {
            tight_loop_contents();
        }
    }

    void wait(uint64_t us) override {
        sleep_us(us);
    }

    bool doEvents(GB2040::Core::Console& console) override {
        return true;
    }

    uint64_t getClock(void) {
        return time_us_64();
    }

    void draw(void) override {
        std::swap(front, back);

        display.drawFramebuffer(*front, 320 / 2 - GB_WIDTH / 2, 240 / 2 - GB_HEIGHT / 2);
    }

    ROMSource* selectROM(void) override {
#ifdef HAS_EMBEDDED_ROM
        return new EmbeddedROM();
#else
        return new NOPROM();
#endif
    }

    RAMSource* getSave(size_t size) override {
        return nullptr;
    }

    void saveData(RAMSource* data) override {
        
    }

    void pushSamples(GB2040::Core::StereoSample* samples, size_t count) override {
        
    }

private:
    int argc;
    char** argv;

    ILI9341::ILI9341 display;

    std::string romPath;

    bool audioEnabled = true;
};

Platform* createPlatform(void) {
    return new PicoPlatform();
}

} // namespace GB2040::Platform

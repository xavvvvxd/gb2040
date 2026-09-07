#include "platform/platform.h"

#include "core/graphics.h"
#include "core/audio.h"
#include "core/console.h"

#include <cstdint>
#include <string>
#include <array>
#include <filesystem>
#include <stdio.h>
#include <fstream>
#include <SDL3/SDL.h>

namespace GB2040::Platform
{

ROMSource::~ROMSource(void) = default;

class RAMROM : public ROMSource {
public:
    RAMROM(std::vector<uint8_t>& rom) : rom(rom) {  }

    ~RAMROM(void) = default;

    void read8(uint32_t addr, uint8_t* buffer, size_t size) {
        memcpy(buffer, rom.data() + addr, size);
    }

    size_t size(void) {
        return rom.size();
    }

    const uint8_t* data(void) override {
        return rom.data();
    }
private:
    std::vector<uint8_t> rom;
};

class RAMRAM : public RAMSource {
public:
    RAMRAM(std::vector<uint8_t>& source) : sram(source) {  }

    ~RAMRAM(void) = default;

    void read8(uint32_t addr, uint8_t* buffer, size_t size) {
        memcpy(buffer, sram.data() + addr, size);
    }

    size_t size(void) {
        return sram.size();
    }

    void write8(uint32_t addr, uint8_t* buffer, size_t size) {
        memcpy(sram.data() + addr, buffer, size);
    }

private:
    std::vector<uint8_t> sram;
};

Platform::~Platform(void) = default;

class DesktopPlatform : public Platform {
public:
    void init(int argc, char** argv) override {
        this->argc = argc;
        this->argv = argv;

        printf("SDL v%d\n", SDL_GetVersion());

        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS)) {
            printf("Error initialising SDL: %s", SDL_GetError());
            exit(1);
        }

        window = SDL_CreateWindow("gb2040", 480, 432, SDL_WINDOW_RESIZABLE);
        if (!window) {
            printf("Error initialising SDL: %s", SDL_GetError());
            exit(1);
        }

        renderer = SDL_CreateRenderer(window, nullptr);
        if (!renderer) {
            printf("Error initialising SDL: %s", SDL_GetError());
            exit(1);
        }

        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, GB_WIDTH, GB_HEIGHT);

        if (!texture) {
            printf("Error initialising SDL: %s", SDL_GetError());
            exit(1);
        }

        SDL_SetRenderLogicalPresentation(renderer, 160, 144, SDL_RendererLogicalPresentation::SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
        SDL_SetTextureScaleMode(texture, SDL_ScaleMode::SDL_SCALEMODE_NEAREST);

        // set up audio
        SDL_AudioSpec want {  };
        want.freq = 48000;
        want.format = SDL_AUDIO_U8;
        want.channels = 2;

        audioStream = SDL_OpenAudioDeviceStream(
            SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
            &want,
            NULL,
            nullptr
        );

        if (!audioStream) {
            printf("Error initialising SDL: %s", SDL_GetError());
            exit(1);
        }

        SDL_ResumeAudioStreamDevice(audioStream);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    void run(void) override {
        using namespace GB2040::Core;

        RAMROM* romSource = selectROM();

        Console* console = new Console(this, romSource);
        console->run();

        console->save();

        deinit();
    }

    void deinit(void) override {
        if (texture) SDL_DestroyTexture(texture);
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        if (audioStream) SDL_DestroyAudioStream(audioStream);
        SDL_Quit();
    }

    void wait(uint64_t us) override {
        SDL_DelayNS(us * 1000);
    }

    bool doEvents(GB2040::Core::Console& console) override {
        SDL_Event e;

        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_EVENT_QUIT: return false;
                case SDL_EVENT_KEY_DOWN: {
                    using GB2040::Core::Button;

                    SDL_Keycode key = e.key.key;
                    
                    switch (key) {
                        case SDLK_F11:
                            fullscreen = !fullscreen;
                            SDL_SetWindowFullscreen(window, fullscreen);
                            break;
                        case SDLK_M:
                            audioEnabled = !audioEnabled;
                            SDL_SetAudioStreamGain(audioStream, audioEnabled ? 1.0f : 0.0f);
                            break;
                        case SDLK_Z: console.pressButton(Button::A); break;
                        case SDLK_X: console.pressButton(Button::B); break;
                        case SDLK_RETURN: console.pressButton(Button::START); break;
                        case SDLK_SPACE: console.pressButton(Button::SELECT); break;
                        case SDLK_UP: console.pressButton(Button::UP); break;
                        case SDLK_DOWN: console.pressButton(Button::DOWN); break;
                        case SDLK_LEFT: console.pressButton(Button::LEFT); break;
                        case SDLK_RIGHT: console.pressButton(Button::RIGHT); break;
                    }
                    break;
                } case SDL_EVENT_KEY_UP: {
                    using GB2040::Core::Button;

                    SDL_Keycode key = e.key.key;
                    
                    switch (key) {
                        case SDLK_Z: console.releaseButton(Button::A); break;
                        case SDLK_X: console.releaseButton(Button::B); break;
                        case SDLK_RETURN: console.releaseButton(Button::START); break;
                        case SDLK_SPACE: console.releaseButton(Button::SELECT); break;
                        case SDLK_UP: console.releaseButton(Button::UP); break;
                        case SDLK_DOWN: console.releaseButton(Button::DOWN); break;
                        case SDLK_LEFT: console.releaseButton(Button::LEFT); break;
                        case SDLK_RIGHT: console.releaseButton(Button::RIGHT); break;
                    }
                    break;
                }
            }
        }

        return true;
    }

    uint64_t getClock(void) {
        using namespace std::chrono;
        return duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
    }

    void draw(void) override {
        std::swap(front, back);

        void* texPixels;
        int pitch;
        SDL_LockTexture(texture, NULL, &texPixels, &pitch);

        GB2040::Core::Colour* dst = (GB2040::Core::Colour*)texPixels;
        for (int y = 0; y < texture->h; y++) {
            GB2040::Core::Colour* row = dst + y * (pitch / sizeof(GB2040::Core::Colour));
            for (int x = 0; x < texture->w; x++) {
                GB2040::Core::Colour pixelLE = front->getPixel(x, y);
                row[x] = (pixelLE << 8) | (pixelLE >> 8);
            }
        }

        SDL_UnlockTexture(texture);

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    RAMROM* selectROM(void) override {
        // just take args
        if (argc < 2) {
            printf("Error: requires ROM path arg");
            exit(1);
        }

        std::ifstream file(argv[1], std::ios::binary | std::ios::ate);
        if (!file) { perror("Failed to read ROM"); exit(1); }

        romPath = argv[1];

        auto size = file.tellg();
        std::vector<uint8_t> buffer(size);

        file.seekg(0);
        file.read(reinterpret_cast<char*>(buffer.data()), size);

        RAMROM* romSource = new RAMROM(buffer);

        return romSource;
    }

    RAMRAM* getSave(size_t size) override {
        std::vector<uint8_t> buffer(size, 0);

        std::filesystem::path p(romPath);
        p.replace_extension(".sav");
        std::ifstream file(p, std::ios::binary);

        if (!file) {
            perror("Could not load save file. Defaulting to 0s.");
            
            return new RAMRAM(buffer);
        }

        file.read(reinterpret_cast<char*>(buffer.data()), size);

        return new RAMRAM(buffer);
    }

    void saveData(RAMSource* data) override {
        RAMRAM* ram = static_cast<RAMRAM*>(data);

        size_t size = data->size();

        std::filesystem::path p(romPath);
        p.replace_extension(".sav");
        std::ofstream file(p, std::ios::binary);
        if (!file) { perror("Could not write save data."); return; }

        file.seekp(0);

        std::vector<uint8_t> buffer(data->size());
        data->read8(0, buffer.data(), data->size());
        file.write(reinterpret_cast<char*>(buffer.data()), buffer.size());
    }

    void pushSamples(GB2040::Core::StereoSample* samples, size_t count) override {
        int queued = SDL_GetAudioStreamAvailable(audioStream) / sizeof(GB2040::Core::StereoSample);

        if (queued < SAMPLE_RATE / 10) {
            size_t padCount = (SAMPLE_RATE / 10) - queued;
            std::vector<GB2040::Core::StereoSample> silence(padCount, { 128, 128 });
            SDL_PutAudioStreamData(audioStream, silence.data(), padCount * sizeof(GB2040::Core::StereoSample));
        }

        SDL_PutAudioStreamData(audioStream, samples, count * sizeof(GB2040::Core::StereoSample));
    }

private:
    int argc;
    char** argv;

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;
    SDL_AudioStream* audioStream = nullptr;

    std::string romPath;

    bool fullscreen = false;
    bool audioEnabled = true;
};

Platform* createPlatform(void) {
    return new DesktopPlatform();
}

} // namespace GB2040::Platform

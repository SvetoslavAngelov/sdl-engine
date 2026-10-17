#pragma once

#include <SDL3/SDL.h>
#include <cstdint>

#include "geometry/scene.h"

class SDLInterface {
public:
    explicit SDLInterface(uint32_t width = 960, uint32_t height = 540);
    ~SDLInterface();

    SDLInterface(SDLInterface const&) = delete;
    SDLInterface& operator=(SDLInterface const&) = delete;

    bool OnInit();
    bool Run();

private:
    enum class FrameResult { Presented, Skipped, Failed };

    bool CreatePipeline();
    bool UploadGeometry();
    void BuildGeometry();
    void ProcessInputEvents(bool& running);
    FrameResult Render();

    SDL_Window* window_ = nullptr;
    SDL_GPUDevice* device_ = nullptr;
    SDL_GPUGraphicsPipeline* pipeline_ = nullptr;
    SDL_GPUBuffer* vertexBuffer_ = nullptr;
    bool sdlInitialised_ = false;
    bool windowClaimed_ = false;
    bool ready_ = false;
    uint32_t width_, height_;
    uint32_t vertexCount_ = 0;
    Scene scene_;
};

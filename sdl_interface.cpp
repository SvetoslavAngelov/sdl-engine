#include "sdl_interface.h"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <type_traits>
#include <vector>

namespace {
bool ReportError(char const* operation) {
    std::cerr << operation << ": " << SDL_GetError() << '\n';
    return false;
}

SDL_GPUShader* LoadShader(SDL_GPUDevice* device, char const* filename,
                          SDL_GPUShaderFormat format, SDL_GPUShaderStage stage,
                          char const* entrypoint) {
    const char* base = SDL_GetBasePath();
    if (!base) {
        ReportError("Find executable directory");
        return nullptr;
    }
    const std::string path = std::string(base) + "shaders/" + filename;
    size_t size = 0;
    void* code = SDL_LoadFile(path.c_str(), &size);
    if (!code) {
        ReportError(path.c_str());
        return nullptr;
    }

    SDL_GPUShaderCreateInfo info{};
    info.code = static_cast<Uint8 const*>(code);
    info.code_size = size;
    info.entrypoint = entrypoint;
    info.format = format;
    info.stage = stage;
    SDL_GPUShader* shader = SDL_CreateGPUShader(device, &info);
    if (!shader) ReportError(filename);
    SDL_free(code);
    return shader;
}
} // namespace

SDLInterface::SDLInterface(uint32_t width, uint32_t height)
    : width_{width}, height_{height} {}

bool SDLInterface::OnInit() {
    if (sdlInitialised_) {
        std::cerr << "Renderer has already been initialised.\n";
        return false;
    }
    if (width_ == 0 || height_ == 0 ||
        width_ > static_cast<uint32_t>(std::numeric_limits<int>::max()) ||
        height_ > static_cast<uint32_t>(std::numeric_limits<int>::max())) {
        std::cerr << "Window dimensions must be positive and fit in an int.\n";
        return false;
    }
    if (!SDL_Init(SDL_INIT_VIDEO)) return ReportError("Initialise SDL video");
    sdlInitialised_ = true;
    window_ = SDL_CreateWindow("SDL Geometry", static_cast<int>(width_),
        static_cast<int>(height_), SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (!window_) return ReportError("Create window");

    device_ = SDL_CreateGPUDevice(
        SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_MSL, false, nullptr);
    if (!device_) return ReportError("Create GPU device");
    if (!SDL_ClaimWindowForGPUDevice(device_, window_)) {
        return ReportError("Claim GPU window");
    }
    windowClaimed_ = true;
    if (!SDL_SetGPUSwapchainParameters(device_, window_,
            SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_VSYNC)) {
        return ReportError("Configure GPU presentation");
    }

    BuildGeometry();
    if (!CreatePipeline() || !UploadGeometry()) return false;
    ready_ = true;
    std::cout << "Renderer: SDL_GPU / " << SDL_GetGPUDeviceDriver(device_)
              << std::endl;
    return true;
}

bool SDLInterface::CreatePipeline() {
    static_assert(std::is_standard_layout_v<vertex>);
    static_assert(std::is_trivially_copyable_v<vertex>);
    const SDL_GPUShaderFormat formats = SDL_GetGPUShaderFormats(device_);
    const bool spirv = (formats & SDL_GPU_SHADERFORMAT_SPIRV) != 0;
    if (!spirv && !(formats & SDL_GPU_SHADERFORMAT_MSL)) {
        std::cerr << "GPU driver does not accept the bundled SPIR-V or MSL shaders.\n";
        return false;
    }
    const SDL_GPUShaderFormat format = spirv ? SDL_GPU_SHADERFORMAT_SPIRV
                                             : SDL_GPU_SHADERFORMAT_MSL;
    SDL_GPUShader* vertexShader = LoadShader(device_,
        spirv ? "triangle.vert.spv" : "triangle.metal", format,
        SDL_GPU_SHADERSTAGE_VERTEX, spirv ? "main" : "vertex_main");
    if (!vertexShader) return false;
    SDL_GPUShader* fragmentShader = LoadShader(device_,
        spirv ? "triangle.frag.spv" : "triangle.metal", format,
        SDL_GPU_SHADERSTAGE_FRAGMENT, spirv ? "main" : "fragment_main");
    if (!fragmentShader) {
        SDL_ReleaseGPUShader(device_, vertexShader);
        return false;
    }

    SDL_GPUVertexBufferDescription buffer{};
    buffer.slot = 0;
    buffer.pitch = sizeof(vertex);
    buffer.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    const SDL_GPUVertexAttribute attributes[] = {
        {0, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(vertex, position_)},
        {1, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4, offsetof(vertex, colour_)}
    };
    SDL_GPUColorTargetDescription target{};
    target.format = SDL_GetGPUSwapchainTextureFormat(device_, window_);

    SDL_GPUGraphicsPipelineCreateInfo info{};
    info.vertex_shader = vertexShader;
    info.fragment_shader = fragmentShader;
    info.vertex_input_state.vertex_buffer_descriptions = &buffer;
    info.vertex_input_state.num_vertex_buffers = 1;
    info.vertex_input_state.vertex_attributes = attributes;
    info.vertex_input_state.num_vertex_attributes = 2;
    info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    info.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    info.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
    info.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
    info.rasterizer_state.enable_depth_clip = true;
    info.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;
    info.target_info.color_target_descriptions = &target;
    info.target_info.num_color_targets = 1;
    pipeline_ = SDL_CreateGPUGraphicsPipeline(device_, &info);
    if (!pipeline_) ReportError("Create graphics pipeline");
    SDL_ReleaseGPUShader(device_, fragmentShader);
    SDL_ReleaseGPUShader(device_, vertexShader);
    return pipeline_ != nullptr;
}

bool SDLInterface::UploadGeometry() {
    auto const& vertices = scene_.GetGeometry();
    if (vertices.empty() || vertices.size() % 3 != 0 ||
        vertices.size() > std::numeric_limits<Uint32>::max() / sizeof(vertex)) {
        std::cerr << "Expected a nonempty triangle list fitting in a GPU buffer.\n";
        return false;
    }
    const Uint32 bytes = static_cast<Uint32>(vertices.size() * sizeof(vertex));
    vertexCount_ = static_cast<Uint32>(vertices.size());
    SDL_GPUBufferCreateInfo bufferInfo{};
    bufferInfo.size = bytes;
    bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    vertexBuffer_ = SDL_CreateGPUBuffer(device_, &bufferInfo);
    if (!vertexBuffer_) return ReportError("Create vertex buffer");

    SDL_GPUTransferBufferCreateInfo transferInfo{};
    transferInfo.size = bytes;
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    SDL_GPUTransferBuffer* transfer = SDL_CreateGPUTransferBuffer(device_, &transferInfo);
    if (!transfer) return ReportError("Create transfer buffer");
    void* mapped = SDL_MapGPUTransferBuffer(device_, transfer, false);
    if (!mapped) {
        ReportError("Map transfer buffer");
        SDL_ReleaseGPUTransferBuffer(device_, transfer);
        return false;
    }
    SDL_memcpy(mapped, vertices.data(), bytes);
    SDL_UnmapGPUTransferBuffer(device_, transfer);

    SDL_GPUCommandBuffer* commands = SDL_AcquireGPUCommandBuffer(device_);
    if (!commands) {
        ReportError("Acquire upload command buffer");
        SDL_ReleaseGPUTransferBuffer(device_, transfer);
        return false;
    }
    SDL_GPUCopyPass* copy = SDL_BeginGPUCopyPass(commands);
    if (!copy) {
        ReportError("Begin upload copy pass");
        SDL_CancelGPUCommandBuffer(commands);
        SDL_ReleaseGPUTransferBuffer(device_, transfer);
        return false;
    }
    const SDL_GPUTransferBufferLocation source{transfer, 0};
    const SDL_GPUBufferRegion destination{vertexBuffer_, 0, bytes};
    SDL_UploadToGPUBuffer(copy, &source, &destination, false);
    SDL_EndGPUCopyPass(copy);
    const bool submitted = SDL_SubmitGPUCommandBuffer(commands);
    if (!submitted) ReportError("Submit geometry upload");
    // SDL defers destruction until pending GPU work no longer uses the buffer.
    SDL_ReleaseGPUTransferBuffer(device_, transfer);
    return submitted;
}

bool SDLInterface::Run() {
    if (!ready_) {
        std::cerr << "Renderer is not ready.\n";
        return false;
    }
    bool running = true;
    while (running) {
        ProcessInputEvents(running);
        if (!running) break;
        const FrameResult result = Render();
        if (result == FrameResult::Failed) return false;
        // Vsync paces rendered frames; avoid spinning while minimised.
        if (result == FrameResult::Skipped) SDL_Delay(16);
    }
    return true;
}

SDLInterface::FrameResult SDLInterface::Render() {
    if (SDL_GetWindowFlags(window_) & SDL_WINDOW_MINIMIZED) return FrameResult::Skipped;
    SDL_GPUCommandBuffer* commands = SDL_AcquireGPUCommandBuffer(device_);
    if (!commands) {
        ReportError("Acquire frame command buffer");
        return FrameResult::Failed;
    }
    SDL_GPUTexture* swapchain = nullptr;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(commands, window_, &swapchain,
                                               &width_, &height_)) {
        ReportError("Acquire swapchain texture");
        SDL_CancelGPUCommandBuffer(commands);
        return FrameResult::Failed;
    }
    if (!swapchain) {
        if (!SDL_CancelGPUCommandBuffer(commands)) {
            ReportError("Cancel unused frame");
            return FrameResult::Failed;
        }
        return FrameResult::Skipped;
    }
    SDL_GPUColorTargetInfo colour{};
    colour.texture = swapchain;
    colour.clear_color = {240 / 255.0f, 240 / 255.0f, 240 / 255.0f, 1.0f};
    colour.load_op = SDL_GPU_LOADOP_CLEAR;
    colour.store_op = SDL_GPU_STOREOP_STORE;
    SDL_GPURenderPass* pass = SDL_BeginGPURenderPass(commands, &colour, 1, nullptr);
    if (!pass) {
        ReportError("Begin render pass");
        // Acquiring a swapchain texture commits us to submission, not cancellation.
        SDL_SubmitGPUCommandBuffer(commands);
        return FrameResult::Failed;
    }
    SDL_BindGPUGraphicsPipeline(pass, pipeline_);
    const SDL_GPUViewport viewport{0, 0, static_cast<float>(width_),
                                   static_cast<float>(height_), 0, 1};
    SDL_SetGPUViewport(pass, &viewport);
    const SDL_GPUBufferBinding binding{vertexBuffer_, 0};
    SDL_BindGPUVertexBuffers(pass, 0, &binding, 1);
    SDL_DrawGPUPrimitives(pass, vertexCount_, 1, 0, 0);
    SDL_EndGPURenderPass(pass);
    if (!SDL_SubmitGPUCommandBuffer(commands)) {
        ReportError("Submit frame");
        return FrameResult::Failed;
    }
    return FrameResult::Presented;
}

void SDLInterface::ProcessInputEvents(bool& running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT ||
            (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
             event.window.windowID == SDL_GetWindowID(window_))) {
            running = false;
        }
    }
}

void SDLInterface::BuildGeometry() {
    scene_ = Scene(std::vector<vertex>{
        {vec3d{0.0f, 0.5f, 0.0f}, vec4d{0.0f, 0.0f, 0.0f, 1.0f}},
        {vec3d{-0.5f, -0.5f, 0.0f}, vec4d{1.0f, 1.0f, 0.0f, 1.0f}},
        {vec3d{0.5f, -0.5f, 0.0f}, vec4d{1.0f, 0.0f, 1.0f, 1.0f}}
    });
}

SDLInterface::~SDLInterface() {
    if (device_) {
        if (!SDL_WaitForGPUIdle(device_)) ReportError("Wait for GPU during cleanup");
        if (pipeline_) SDL_ReleaseGPUGraphicsPipeline(device_, pipeline_);
        if (vertexBuffer_) SDL_ReleaseGPUBuffer(device_, vertexBuffer_);
        if (windowClaimed_) SDL_ReleaseWindowFromGPUDevice(device_, window_);
        SDL_DestroyGPUDevice(device_);
    }
    if (window_) SDL_DestroyWindow(window_);
    if (sdlInitialised_) SDL_Quit();
}

#pragma once
#include "IRenderAPI.h"
#include <memory>

enum class RenderAPIType {
    DirectX12,
    // Otras APIs pueden añadirse aquí, como OpenGL, Vulkan, etc.
};

class RenderAPIFactory {
public:
    static std::unique_ptr<IRenderAPI> CreateRenderAPI(RenderAPIType apiType);
};

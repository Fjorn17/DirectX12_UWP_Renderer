#include "pch.h"
#include "RenderAPIFactory.h"
#include "DirectX12RendererAPI.h"
#include <stdexcept>

std::unique_ptr<IRenderAPI> RenderAPIFactory::CreateRenderAPI(RenderAPIType apiType) {
    switch (apiType) {
    case RenderAPIType::DirectX12:
        return std::make_unique<DirectX12RendererAPI>();
    default:
        throw std::runtime_error("Unsupported Render API");
    }
}
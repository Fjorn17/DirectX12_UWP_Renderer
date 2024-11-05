/**
 * @file DirectX12RendererAPI.cpp
 * @brief Implementación de la API de renderizado con DirectX 12.
 */

#include "pch.h"
#include "DirectX12RendererAPI.h"
#include <stdexcept>
#include <iostream>

DirectX12RendererAPI::DirectX12RendererAPI() : frameIndex(0), fenceValue(0), fenceEvent(nullptr), rtvDescriptorSize(0) {}

DirectX12RendererAPI::~DirectX12RendererAPI() {
    CleanUp();
    
}

/**
 * @brief Inicializa la API de DirectX 12.
 * @param hwnd Handle de la ventana donde se renderizará.
 */
void DirectX12RendererAPI::Initialize(void* hwnd) {
    // Realizamos el casting de void* a HWND para usarlo en DirectX
    HWND windowHandle = static_cast<HWND>(hwnd);
    CreateDevice();
    CreateCommandQueue();
    CreateSwapChain(windowHandle); // Realiza el casting de void* a HWND
    CreateRenderTargetView();
    CreateCommandObjects();

    fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!fenceEvent) {
        throw std::runtime_error("Failed to create fence event.");
    }
}

/**
 * @brief Renderiza un fotograma utilizando DirectX 12.
 */
void DirectX12RendererAPI::RenderFrame() {
    try {
        std::cerr << "RenderFrame: Start" << std::endl;

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);

        std::cerr << "RenderFrame: Command allocator and list reset" << std::endl;
        commandAllocator->Reset();
        commandList->Reset(commandAllocator.Get(), nullptr);

        std::cerr << "RenderFrame: Transitioning to render target" << std::endl;
        CD3DX12_RESOURCE_BARRIER barrierToRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(
            renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandList->ResourceBarrier(1, &barrierToRenderTarget);

        FLOAT clearColor[] = { 0.1f, 0.1f, 0.2f, 1.0f };
        std::cerr << "RenderFrame: Clearing render target view" << std::endl;
        commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
        commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

        std::cerr << "RenderFrame: Transitioning to present" << std::endl;
        CD3DX12_RESOURCE_BARRIER barrierToPresent = CD3DX12_RESOURCE_BARRIER::Transition(
            renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        commandList->ResourceBarrier(1, &barrierToPresent);

        std::cerr << "RenderFrame: Closing command list" << std::endl;
        commandList->Close();
        ID3D12CommandList* commandLists[] = { commandList.Get() };
        commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

        std::cerr << "RenderFrame: Presenting swap chain" << std::endl;
        HRESULT hr = swapChain->Present(1, 0);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to present the swap chain.");
        }

        std::cerr << "RenderFrame: Waiting for previous frame" << std::endl;
        WaitForPreviousFrame();

        std::cerr << "RenderFrame: End" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in RenderFrame: " << e.what() << std::endl;
        throw;
    }
}


/**
 * @brief Cambia el tamaño del renderizado en DirectX 12.
 * @param width Ancho de la ventana.
 * @param height Alto de la ventana.
 */
void DirectX12RendererAPI::Resize(int width, int height) {
    // Implementar lógica para ajustar el tamaño del swap chain y RTV.
}

/**
 * @brief Limpia los recursos utilizados por la API de DirectX 12.
 */
void DirectX12RendererAPI::CleanUp() {
    if (device) {
        // Liberar el frame actual antes de limpiar
        WaitForPreviousFrame();

        // Cerrar el evento de sincronización del fence
        if (fenceEvent) {
            CloseHandle(fenceEvent);
            fenceEvent = nullptr;
        }

        // Libera los recursos de DirectX en el orden de creación inverso
        for (UINT i = 0; i < FrameCount; i++) {
            renderTargets[i].Reset();
        }
        rtvHeap.Reset();
        swapChain.Reset();
        commandQueue.Reset();
        commandAllocator.Reset();
        commandList.Reset();
        fence.Reset();
        device.Reset();
    }
}

void DirectX12RendererAPI::CreateDevice() {
    if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)))) {
        throw std::runtime_error("Failed to create DirectX 12 Device.");
    }
}

void DirectX12RendererAPI::CreateCommandQueue() {
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    if (FAILED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)))) {
        throw std::runtime_error("Failed to create Command Queue.");
    }
}

void DirectX12RendererAPI::CreateSwapChain(HWND hwnd) {
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = FrameCount;
    swapChainDesc.Width = 800;
    swapChainDesc.Height = 600;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
    if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory)))) {
        throw std::runtime_error("Failed to create DXGI Factory.");
    }

    Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChainTemp;
    if (FAILED(factory->CreateSwapChainForHwnd(
        commandQueue.Get(), hwnd, &swapChainDesc, nullptr, nullptr, &swapChainTemp))) {
        throw std::runtime_error("Failed to create Swap Chain.");
    }

    swapChainTemp.As(&swapChain);
    frameIndex = swapChain->GetCurrentBackBufferIndex();
}

void DirectX12RendererAPI::CreateRenderTargetView() {
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = FrameCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    if (FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)))) {
        throw std::runtime_error("Failed to create RTV Descriptor Heap.");
    }

    rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());

    for (UINT i = 0; i < FrameCount; i++) {
        if (FAILED(swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i])))) {
            throw std::runtime_error("Failed to get buffer for Render Target View.");
        }
        device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, rtvDescriptorSize);
    }
}

void DirectX12RendererAPI::CreateCommandObjects() {
    if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)))) {
        throw std::runtime_error("Failed to create Command Allocator.");
    }

    if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)))) {
        throw std::runtime_error("Failed to create Command List.");
    }

    // Cierra la lista de comandos para que esté en un estado válido
    commandList->Close();

    // Configura el evento de sincronización del fence, que se usa para esperar a que el GPU termine
    fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!fenceEvent) {
        throw std::runtime_error("Failed to create fence event.");
    }

    if (FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)))) {
        throw std::runtime_error("Failed to create Fence.");
    }
    fenceValue = 1;
}


void DirectX12RendererAPI::WaitForPreviousFrame() {
    std::cerr << "WaitForPreviousFrame: Start" << std::endl;

    // Verificar que el fence y el commandQueue estén inicializados
    if (!fence || !commandQueue) {
        std::cerr << "WaitForPreviousFrame: Error - fence or commandQueue not initialized." << std::endl;
        throw std::runtime_error("Fence or Command Queue not initialized.");
    }

    const UINT64 currentFenceValue = fenceValue;
    std::cerr << "WaitForPreviousFrame: Signal fence with value " << currentFenceValue << std::endl;
    HRESULT hr = commandQueue->Signal(fence.Get(), currentFenceValue);
    if (FAILED(hr)) {
        std::cerr << "WaitForPreviousFrame: Error - failed to signal command queue." << std::endl;
        throw std::runtime_error("Failed to signal command queue.");
    }
    fenceValue++;

    if (fence->GetCompletedValue() < currentFenceValue) {
        std::cerr << "WaitForPreviousFrame: Waiting for fence completion..." << std::endl;
        hr = fence->SetEventOnCompletion(currentFenceValue, fenceEvent);
        if (FAILED(hr)) {
            std::cerr << "WaitForPreviousFrame: Error - failed to set event on completion." << std::endl;
            throw std::runtime_error("Failed to set event on completion.");
        }
        WaitForSingleObject(fenceEvent, INFINITE);
        std::cerr << "WaitForPreviousFrame: Fence completed" << std::endl;
    }
    else {
        std::cerr << "WaitForPreviousFrame: Fence already completed, no wait needed" << std::endl;
    }

    frameIndex = swapChain->GetCurrentBackBufferIndex();
    std::cerr << "WaitForPreviousFrame: frameIndex updated to " << frameIndex << std::endl;
    std::cerr << "WaitForPreviousFrame: End" << std::endl;
}
#include "pch.h"
#include "Renderer.h"
#include <stdexcept>
#include <iostream>

using Microsoft::WRL::ComPtr;

/**
 * @class Renderer
 * @brief Clase principal para gestionar el renderizado con DirectX 12.
 */

Renderer::Renderer() : frameIndex(0), fenceValue(0), fenceEvent(nullptr), rtvDescriptorSize(0) {}

Renderer::~Renderer() {
    if (fenceEvent) {
        CloseHandle(fenceEvent);
    }

    commandQueue.Reset();
    swapChain.Reset();
    rtvHeap.Reset();
    for (auto& target : renderTargets) {
        target.Reset();
    }
    commandAllocator.Reset();
    commandList.Reset();
    fence.Reset();
    device.Reset();
}

/**
 * @brief Inicializa el Renderer con el identificador de la ventana.
 * @param hwnd Handle de la ventana donde se renderizará.
 */
void Renderer::Initialize(HWND hwnd) {
    try {
        CreateDevice();
        CreateCommandQueue();
        CreateSwapChain(hwnd);
        CreateRenderTargetView();
        CreateCommandObjects();

        fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (!fenceEvent) {
            throw std::runtime_error("Failed to create fence event.");
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in Renderer::Initialize: " << e.what() << std::endl;
        throw;
    }
}

/**
 * @brief Crea el dispositivo DirectX 12.
 */
void Renderer::CreateDevice() {
    UINT dxgiFactoryFlags = 0;
    ComPtr<IDXGIFactory4> factory;
    if (FAILED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)))) {
        throw std::runtime_error("Failed to create DXGI Factory.");
    }

    if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)))) {
        throw std::runtime_error("Failed to create DirectX 12 Device.");
    }
}

/**
 * @brief Crea la cola de comandos utilizada para enviar instrucciones de renderizado a la GPU.
 */
void Renderer::CreateCommandQueue() {
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    if (FAILED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)))) {
        throw std::runtime_error("Failed to create Command Queue.");
    }
}

/**
 * @brief Crea la cadena de intercambio (swap chain) asociada a la ventana.
 * @param hwnd Handle de la ventana donde se renderizará.
 */
void Renderer::CreateSwapChain(HWND hwnd) {
    ComPtr<IDXGIFactory4> factory;
    UINT dxgiFactoryFlags = 0;
    if (FAILED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)))) {
        throw std::runtime_error("Failed to create DXGI Factory for SwapChain.");
    }

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = FrameCount;
    swapChainDesc.Width = 800;
    swapChainDesc.Height = 600;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> swapChainTemp;
    if (FAILED(factory->CreateSwapChainForHwnd(
        commandQueue.Get(), hwnd, &swapChainDesc, nullptr, nullptr, &swapChainTemp))) {
        throw std::runtime_error("Failed to create Swap Chain.");
    }

    swapChainTemp.As(&swapChain);
    frameIndex = swapChain->GetCurrentBackBufferIndex();
}

/**
 * @brief Crea las vistas de destino de renderizado (RTV).
 */
void Renderer::CreateRenderTargetView() {
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = FrameCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

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

/**
 * @brief Crea los objetos de comando necesarios para el renderizado.
 */
void Renderer::CreateCommandObjects() {
    if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)))) {
        throw std::runtime_error("Failed to create Command Allocator.");
    }

    if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)))) {
        throw std::runtime_error("Failed to create Command List.");
    }

    commandList->Close();

    // Crear el fence para la sincronización de GPU
    if (FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)))) {
        throw std::runtime_error("Failed to create fence.");
    }
    fenceValue = 1; // Inicializa el valor del fence

    // Crear el evento de sincronización del fence
    fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!fenceEvent) {
        throw std::runtime_error("Failed to create fence event.");
    }

    std::cerr << "Command objects and synchronization fence created successfully." << std::endl;
}

/**
 * @brief Renderiza un fotograma.
 */
void Renderer::RenderFrame() {
    try {
        std::cerr << "Start RenderFrame" << std::endl;

        if (!commandList || !commandQueue) {
            throw std::runtime_error("Uninitialized command list or queue.");
        }

        std::cerr << "Command list and queue validated" << std::endl;

        // Configuración del render target handle
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);

        commandAllocator->Reset();
        commandList->Reset(commandAllocator.Get(), nullptr);

        std::cerr << "Command allocator and list reset" << std::endl;

        // Clear y configuración de render target
        FLOAT clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
        commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
        commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

        std::cerr << "Render target configured" << std::endl;

        // Cierra y ejecuta la command list
        commandList->Close();
        ID3D12CommandList* commandLists[] = { commandList.Get() };
        commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

        std::cerr << "Command list executed" << std::endl;

        HRESULT hr = swapChain->Present(1, 0);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to present the swap chain.");
        }

        std::cerr << "Frame presented" << std::endl;

        WaitForPreviousFrame();
        std::cerr << "Wait for previous frame completed" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in RenderFrame: " << e.what() << std::endl;
        throw;
    }
}



/**
 * @brief Espera a que el frame actual sea completado por la GPU.
 */
void Renderer::WaitForPreviousFrame() {
    try {
        std::cerr << "WaitForPreviousFrame: Start" << std::endl;
        if (!fence) {
            throw std::runtime_error("Fence is not initialized.");
        }
        if (!commandQueue) {
            throw std::runtime_error("CommandQueue is not initialized.");
        }
        // Incrementa el valor del fence para el próximo frame
        const UINT64 currentFenceValue = fenceValue;
        HRESULT signalResult = commandQueue->Signal(fence.Get(), currentFenceValue);
        if (FAILED(signalResult)) {
            throw std::runtime_error("Failed to signal command queue.");
        }
        std::cerr << "WaitForPreviousFrame: Signal sent, fenceValue = " << currentFenceValue << std::endl;

        fenceValue++;

        // Espera a que el fence alcance el valor actual
        if (fence->GetCompletedValue() < currentFenceValue) {
            std::cerr << "WaitForPreviousFrame: Waiting for fence completion..." << std::endl;
            HRESULT eventResult = fence->SetEventOnCompletion(currentFenceValue, fenceEvent);
            if (FAILED(eventResult)) {
                throw std::runtime_error("Failed to set event on fence completion.");
            }

            DWORD waitResult = WaitForSingleObject(fenceEvent, INFINITE);
            if (waitResult != WAIT_OBJECT_0) {
                throw std::runtime_error("WaitForSingleObject failed in WaitForPreviousFrame.");
            }
            std::cerr << "WaitForPreviousFrame: Fence completed" << std::endl;
        }
        else {
            std::cerr << "WaitForPreviousFrame: Fence already completed, no wait needed" << std::endl;
        }

        // Actualiza el índice del frame actual
        frameIndex = swapChain->GetCurrentBackBufferIndex();
        std::cerr << "WaitForPreviousFrame: frameIndex updated to " << frameIndex << std::endl;

        std::cerr << "WaitForPreviousFrame: End" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in WaitForPreviousFrame: " << e.what() << std::endl;
        throw;
    }
}


extern "C" {
    MYTHFORGE_API Renderer* CreateRenderer() {
        return new Renderer();
    }

    MYTHFORGE_API void Initialize(Renderer* renderer, HWND hwnd) {
        if (renderer) {
            renderer->Initialize(hwnd);
        }
    }

    MYTHFORGE_API void RenderFrame(Renderer* renderer) {
        if (renderer) {
            renderer->RenderFrame();
        }
    }

    MYTHFORGE_API void DestroyRenderer(Renderer* renderer) {
        delete renderer;
    }
}

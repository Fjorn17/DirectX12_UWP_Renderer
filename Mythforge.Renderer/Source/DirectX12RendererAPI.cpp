/**
 * @file DirectX12RendererAPI.cpp
 * @brief Implementación de la API de renderizado con DirectX 12.
 */

#include "pch.h"
#include "DirectX12RendererAPI.h"
#include "DirectXHelper.h"
#include <DirectXColors.h>
#include <stdexcept>
#include <iostream>
#include "DeviceUtils.h"
/**
 * @brief Inicializa los componentes necesarios para la API de DirectX 12.
 * @param hwnd Handle de la ventana donde se renderizará.
 */
void DirectX12RendererAPI::Initialize(HWND hwnd) {
    window = hwnd;
    
#if defined(_DEBUG)
    {
        ComPtr<ID3D12Debug1> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();
        }
    }
#endif

    ComPtr<IDXGIAdapter4> dxgiAdapter4 = GetAdapter();
    d3dDevice = CreateDevice(dxgiAdapter4);
    commandQueue = CreateCommandQueue(d3dDevice);
    swapChain = CreateSwapChain(window, commandQueue, frameCount);
    backBufferIndex = swapChain->GetCurrentBackBufferIndex();
    rtvDescriptorHeap = CreateDescriptorHeap(d3dDevice, frameCount);
    rtvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    dsvDescriptorHeap = CreateDescriptorHeap(d3dDevice, 1, D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

    NAME_D3D12_OBJECT(d3dDevice);
    NAME_D3D12_OBJECT(commandQueue);
    NAME_D3D12_OBJECT(rtvDescriptorHeap);
    NAME_D3D12_OBJECT(dsvDescriptorHeap);

    UpdateRenderTargetViews(d3dDevice, swapChain, rtvDescriptorHeap, renderTargets, frameCount);

    // Obtén el área del cliente de la ventana usando HWND
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    // Calcula el ancho y alto de la ventana
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;

    // Llama a la función UpdateDepthStencilView usando estos valores
    UpdateDepthStencilView(d3dDevice, dsvDescriptorHeap, depthStencil, width, height);

    for (int i = 0; i < frameCount; i++) {
        commandAllocators[i] = CreateCommandAllocator(d3dDevice);
        commandAllocators[i]->SetName((L"commandAllocator[" + std::to_wstring(i) + L"]").c_str());
    }

    commandList = CreateCommandList(d3dDevice, commandAllocators[backBufferIndex]);
    NAME_D3D12_OBJECT(commandList);

    fence = CreateFence(d3dDevice);
    fenceEvent = CreateEventHandle();

    UpdateViewportPerspective(width, height);
}

void DirectX12RendererAPI::Destroy() {
    fence->Release();
    commandList->Release();
    for (auto commandAllocator : commandAllocators) {
        commandAllocator->Release();
    }
    depthStencil->Release();
    for (auto renderTarget : renderTargets) {
        renderTarget->Release();
    }
    dsvDescriptorHeap->Release();
    rtvDescriptorHeap->Release();
    commandQueue->Release();
    d3dDevice->Release();
}

void DirectX12RendererAPI::UpdateViewportPerspective(UINT width, UINT height) {
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    scissorRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
    screenViewport = { 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };
}

/**
 * @brief Cambia el tamaño del renderizado en DirectX 12.
 * @param width Ancho de la ventana.
 * @param height Alto de la ventana.
 */
void DirectX12RendererAPI::Resize(UINT width, UINT height) {
    Flush(commandQueue, fence, fenceValue, fenceEvent);

    for (UINT i = 0; i < frameCount; ++i) {
        renderTargets[i].Reset();
        frameFenceValues[i] = frameFenceValues[backBufferIndex];
    }

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    DX::ThrowIfFailed(swapChain->GetDesc(&swapChainDesc));
    DX::ThrowIfFailed(swapChain->ResizeBuffers(frameCount, width, height,
        swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));

    backBufferIndex = swapChain->GetCurrentBackBufferIndex();

    UpdateRenderTargetViews(d3dDevice, swapChain, rtvDescriptorHeap, renderTargets, frameCount);

    depthStencil.Reset();
    UpdateDepthStencilView(d3dDevice, dsvDescriptorHeap, depthStencil, width, height);

    UpdateViewportPerspective(width, height);
}

void DirectX12RendererAPI::ResetCommands() {
    auto commandAllocator = commandAllocators[backBufferIndex];
    commandAllocator->Reset();
    commandList->Reset(commandAllocator.Get(), nullptr);
}

void DirectX12RendererAPI::SetRenderTargets()
{
    auto backBuffer = renderTargets[backBufferIndex];

    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandList->ResourceBarrier(1, &barrier);

    commandList->RSSetViewports(1, &screenViewport);
    commandList->RSSetScissorRects(1, &scissorRect);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), backBufferIndex, rtvDescriptorSize);
    commandList->ClearRenderTargetView(rtv, DirectX::Colors::CornflowerBlue, 0, nullptr);

    CD3DX12_CPU_DESCRIPTOR_HANDLE dsv(dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    commandList->OMSetRenderTargets(1, &rtv, false, &dsv);
}

void DirectX12RendererAPI::Present()
{
    auto backBuffer = renderTargets[backBufferIndex];
    
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    commandList->ResourceBarrier(1, &barrier);

    DX::ThrowIfFailed(commandList->Close());
    ID3D12CommandList* const commandLists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

    DX::ThrowIfFailed(swapChain->Present(1, 0));
    frameFenceValues[backBufferIndex] = Signal(commandQueue, fence, fenceValue);

    WaitForFenceValue(fence, frameFenceValues[backBufferIndex], fenceEvent);
}

void DirectX12RendererAPI::CloseCommandsAndFlush()
{
    commandList->Close();
    ID3D12CommandList* const commandLists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
    Flush(commandQueue, fence, fenceValue, fenceEvent);
}



/**
 * @file Renderer.h
 * @brief Define la clase Renderer para gestionar el renderizado utilizando una API de renderizado gen�rica.
 */

#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include <dxgi1_6.h>
#include <wrl.h>
#include <Windows.h>

using namespace Microsoft::WRL;
using namespace Platform;
using namespace Windows::UI::Core;
using namespace DirectX;
class Renderer {
public:
    
    void Initialize(CoreWindow^ coreWindow);
    void Destroy();
    void UpdateViewportPerspective();
    void Resize(UINT width, UINT height);
    void ResetCommands();
    void CloseCommandsAndFlush();
    void SetRenderTargets();
    void Present();



    ComPtr<ID3D12Device2>               d3dDevice; ///< Dispositivo DirectX 12

    static const UINT frameCount = 3; ///< N�mero de fotogramas en la cadena de intercambio
    static const constexpr float fovAngleY = (70.0f * XM_PI / 180.0f);
    ComPtr<ID3D12CommandQueue>          commandQueue; ///< Cola de comandos DirectX 12
    UINT64                              fenceValue; ///< Valor del fence para sincronizaci�n
    ComPtr<ID3D12Fence>                 fence; ///< Fence para sincronizaci�n GPU-CPU
    HANDLE                              fenceEvent; ///< Evento para la sincronizaci�n del fence

    ComPtr<ID3D12GraphicsCommandList2>   commandList; ///< Lista de comandos de gr�ficos

    UINT                                backBufferIndex;

    XMMATRIX                            perspectiveMatrix;
private:

    Agile<CoreWindow> window;


    ComPtr<IDXGISwapChain4>             swapChain; ///< Cadena de intercambio DirectX 12
    ComPtr<ID3D12DescriptorHeap>        rtvDescriptorHeap; ///< Heap de descriptores para vistas de renderizado
    UINT                                rtvDescriptorSize; ///< Tama�o del descriptor RTV
    ComPtr<ID3D12DescriptorHeap>        dsvDescriptorHeap;
    ComPtr<ID3D12CommandAllocator>      commandAllocators[frameCount]; ///< Allocator de comandos

    UINT                                frameIndex; ///< �ndice del fotograma actual

    UINT64                              frameFenceValues[frameCount] = {};

    D3D12_VIEWPORT                      screenViewport;
    D3D12_RECT                          scissorRect;

    ComPtr<ID3D12Resource>              renderTargets[frameCount];
    ComPtr<ID3D12Resource>              depthStencil;
};
/**
 * @file DirectX12RendererAPI.h
 * @brief Clase que implementa la API de renderizado utilizando DirectX 12.
 */

#pragma once
#include "IRenderAPI.h"
#include <d3d12.h>
#include "d3dx12.h"
#include <dxgi1_6.h>
#include <wrl.h>
#include <Windows.h>

using namespace Microsoft::WRL;

 /**
  * @class DirectX12RendererAPI
  * @brief Implementación de IRenderAPI para manejar el renderizado usando DirectX 12.
  */
class DirectX12RendererAPI : public IRenderAPI {
public:
    /**
    * @brief Inicializa los componentes necesarios para la API de DirectX 12.
    * @param hwnd Handle de la ventana donde se renderizará.
    */
    void Initialize(HWND hwnd);

    void Destroy();

    void UpdateViewportPerspective(UINT width, UINT height);

    /**
    * @brief Cambia el tamaño del renderizado en DirectX 12.
    * @param width Ancho de la ventana.
    * @param height Alto de la ventana.
    *
    * Redimensiona los recursos del SwapChain y las vistas de renderizado.
    */
    void Resize(UINT width, UINT height);

    void ResetCommands();
    void CloseCommandsAndFlush();
    void SetRenderTargets();
    void Present();

    

private:
   
    HWND window;

    static const UINT frameCount = 3; ///< Número de fotogramas en la cadena de intercambio

    ComPtr<ID3D12Device2>               d3dDevice; ///< Dispositivo DirectX 12
    ComPtr<ID3D12CommandQueue>          commandQueue; ///< Cola de comandos DirectX 12
    ComPtr<IDXGISwapChain4>             swapChain; ///< Cadena de intercambio DirectX 12
    ComPtr<ID3D12DescriptorHeap>        rtvDescriptorHeap; ///< Heap de descriptores para vistas de renderizado
    UINT                                rtvDescriptorSize; ///< Tamaño del descriptor RTV
    ComPtr<ID3D12DescriptorHeap>        dsvDescriptorHeap;
    ComPtr<ID3D12CommandAllocator>      commandAllocators[frameCount]; ///< Allocator de comandos
    ComPtr<ID3D12GraphicsCommandList>   commandList; ///< Lista de comandos de gráficos
    
    
    UINT                                frameIndex; ///< Índice del fotograma actual

    ComPtr<ID3D12Fence>                 fence; ///< Fence para sincronización GPU-CPU
    HANDLE                              fenceEvent; ///< Evento para la sincronización del fence
    UINT64                              frameFenceValues[frameCount] = {};
    UINT64                              fenceValue; ///< Valor del fence para sincronización
    

    D3D12_VIEWPORT                      screenViewport;
    D3D12_RECT                          scissorRect;

    ComPtr<ID3D12Resource>              renderTargets[frameCount];
    ComPtr<ID3D12Resource>              depthStencil;
    UINT                                backBufferIndex;
};

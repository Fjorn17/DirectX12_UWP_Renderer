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

 /**
  * @class DirectX12RendererAPI
  * @brief Implementación de IRenderAPI para manejar el renderizado usando DirectX 12.
  */
class DirectX12RendererAPI : public IRenderAPI
{
public:
    DirectX12RendererAPI();
    ~DirectX12RendererAPI();

    void Initialize(void* hwnd) override;
    void RenderFrame() override;
    void Resize(int width, int height) override;
    void CleanUp() override;

private:
    void CreateDevice();
    void CreateCommandQueue();
    void CreateSwapChain(HWND hwnd);
    void CreateRenderTargetView();
    void CreateCommandObjects();
    void WaitForPreviousFrame();

    static const UINT FrameCount = 2; ///< Número de fotogramas en la cadena de intercambio

    Microsoft::WRL::ComPtr<ID3D12Device> device; ///< Dispositivo DirectX 12
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue; ///< Cola de comandos DirectX 12
    Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain; ///< Cadena de intercambio DirectX 12
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap; ///< Heap de descriptores para vistas de renderizado
    Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets[FrameCount]; ///< Vistas de renderizado
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator; ///< Allocator de comandos
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList; ///< Lista de comandos de gráficos
    Microsoft::WRL::ComPtr<ID3D12Fence> fence; ///< Fence para sincronización GPU-CPU
    UINT rtvDescriptorSize; ///< Tamaño del descriptor RTV
    UINT frameIndex; ///< Índice del fotograma actual
    HANDLE fenceEvent; ///< Evento para la sincronización del fence
    UINT64 fenceValue; ///< Valor del fence para sincronización
};

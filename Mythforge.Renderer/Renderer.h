/**
 * @file Renderer.h
 * @brief Define la clase Renderer para gestionar el renderizado con DirectX 12.
 */

#pragma once
#include <Windows.h>
#include <d3d12.h>
#include "d3dx12.h"
#include <dxgi1_6.h>
#include <wrl.h>
#include "framework.h"

 /**
  * @class Renderer
  * @brief Clase que proporciona la funcionalidad de renderizado con DirectX 12.
  */
class MYTHFORGE_API Renderer
{
public:
    Renderer();
    ~Renderer();

    /**
     * @brief Inicializa el Renderer con el identificador de la ventana.
     * @param hwnd Handle de la ventana donde se renderizará.
     */
    void Initialize(HWND hwnd);

    /**
     * @brief Renderiza un fotograma.
     */
    void RenderFrame();

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

extern "C" {
    MYTHFORGE_API Renderer* CreateRenderer();
    MYTHFORGE_API void Initialize(Renderer* renderer, HWND hwnd);
    MYTHFORGE_API void RenderFrame(Renderer* renderer);
    MYTHFORGE_API void DestroyRenderer(Renderer* renderer);
}

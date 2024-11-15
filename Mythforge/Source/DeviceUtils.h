#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include <dxgi1_6.h>
#include <wrl.h>
#include <Windows.h>

using namespace Microsoft::WRL;
using namespace Platform;
using namespace Windows::UI::Core;

ComPtr<IDXGIAdapter4> GetAdapter();
ComPtr<ID3D12Device2> CreateDevice(ComPtr<IDXGIAdapter4> adapter);
ComPtr<ID3D12CommandQueue> CreateCommandQueue(ComPtr<ID3D12Device2> device);
ComPtr<IDXGISwapChain4> CreateSwapChain(CoreWindow^ window, ComPtr<ID3D12CommandQueue> commandQueue, UINT bufferCount);
ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ComPtr<ID3D12Device2> device, uint32_t numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE = D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
void UpdateRenderTargetViews(ComPtr<ID3D12Device2> device, ComPtr<IDXGISwapChain4> swapChain, ComPtr<ID3D12DescriptorHeap> descriptorHeap, ComPtr<ID3D12Resource> renderTargets[], UINT bufferCount);
void UpdateDepthStencilView(ComPtr<ID3D12Device2> device, ComPtr<ID3D12DescriptorHeap> descriptorHeap, ComPtr<ID3D12Resource>& depthStencil, UINT width, UINT height);
ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(ComPtr<ID3D12Device2> device);
ComPtr<ID3D12GraphicsCommandList2> CreateCommandList(ComPtr<ID3D12Device2> device, ComPtr<ID3D12CommandAllocator> commandAllocator);
ComPtr<ID3D12Fence> CreateFence(ComPtr<ID3D12Device2> device);
HANDLE CreateEventHandle();
UINT64 Signal(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence, UINT64& fenceValue);
void WaitForFenceValue(ComPtr<ID3D12Fence> fence, UINT64 fenceValue, HANDLE fenceEvent);
void Flush(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence, UINT64& fenceValue, HANDLE fenceEvent);
void UpdateBufferResource(ComPtr<ID3D12Device2> device, ComPtr<ID3D12GraphicsCommandList2> commandList, ComPtr<ID3D12Resource>& pDestinationResource, ComPtr<ID3D12Resource>& pIntermediateResource, size_t numElements, size_t elementSize, const void* bufferData);
void CreateTextureResource(ComPtr<ID3D12Device2> device, ComPtr<ID3D12GraphicsCommandList2> commandList, const LPWSTR path, ComPtr<ID3D12Resource>& texture, ComPtr<ID3D12Resource>& textureUpload, D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc, DXGI_FORMAT textureFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
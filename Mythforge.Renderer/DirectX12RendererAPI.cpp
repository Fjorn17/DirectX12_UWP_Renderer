/**
 * @file DirectX12RendererAPI.cpp
 * @brief Implementaci�n de la API de renderizado con DirectX 12.
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
 * @brief Inicializa los componentes necesarios para la API de DirectX 12.
 * @param hwnd Handle de la ventana donde se renderizar�.
 */
void DirectX12RendererAPI::Initialize(void* hwnd) {
    HWND windowHandle = static_cast<HWND>(hwnd);
    InitializeDeviceComponents();
    InitializeSwapChain(windowHandle);
    InitializeRenderTargetView();
    InitializeCommandObjects();
    InitializeSynchronizationObjects();
}

/**
 * @brief Inicializa los componentes principales de DirectX 12, como el dispositivo y la cola de comandos.
 *
 * Este m�todo encapsula la creaci�n del dispositivo de DirectX 12 y de la cola de comandos
 * necesaria para gestionar las operaciones de renderizado. Es un paso esencial en la configuraci�n
 * del entorno de renderizado.
 */
void DirectX12RendererAPI::InitializeDeviceComponents() {
    CreateDevice();
    CreateCommandQueue();
}

/**
 * @brief Inicializa el SwapChain que se utiliza para la presentaci�n de fotogramas.
 * @param hwnd Handle de la ventana donde se renderizar�.
 *
 * Esta funci�n crea y configura el SwapChain, que es el mecanismo de intercambio de buffers en
 * DirectX 12. Utiliza el handle de ventana proporcionado para asociarse a la ventana de renderizado.
 */
void DirectX12RendererAPI::InitializeSwapChain(HWND hwnd) {
    CreateSwapChain(hwnd);
}

/**
 * @brief Configura la vista de destino de renderizado (Render Target View) para el renderizado.
 *
 * Esta funci�n configura las vistas de destino de renderizado, que son necesarias para renderizar
 * el contenido en los buffers de destino asociados al SwapChain. Se realiza una configuraci�n
 * espec�fica para cada buffer.
 */
void DirectX12RendererAPI::InitializeRenderTargetView() {
    CreateRenderTargetView();
}

/**
 * @brief Inicializa los objetos de comando necesarios para el renderizado.
 *
 * Este m�todo crea y configura el asignador de comandos (command allocator) y la lista de comandos
 * (command list), que se utilizan para gestionar y ejecutar los comandos de renderizado. La lista de
 * comandos se cierra inicialmente para estar en un estado v�lido.
 */
void DirectX12RendererAPI::InitializeCommandObjects() {
    CreateCommandObjects();
}

/**
 * @brief Crea y configura los objetos de sincronizaci�n necesarios para la comunicaci�n CPU-GPU.
 *
 * En este m�todo se configura el evento de sincronizaci�n (fence event), necesario para coordinar
 * el flujo entre la CPU y GPU. Este evento permite esperar hasta que el GPU haya terminado de
 * procesar el frame actual antes de proceder.
 *
 * @throws std::runtime_error si el evento de sincronizaci�n no se puede crear.
 */
void DirectX12RendererAPI::InitializeSynchronizationObjects() {
    fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!fenceEvent) {
        throw std::runtime_error("Failed to create fence event.");
    }
}

/**
 * @brief Renderiza un fotograma utilizando DirectX 12.
 *
 * Este m�todo gestiona todo el proceso de renderizado de un solo fotograma, desde la
 * configuraci�n de las barreras de recursos hasta la presentaci�n en pantalla.
 */
void DirectX12RendererAPI::RenderFrame() {
    
    ResetCommandObjects();
    TransitionToRenderTarget();
    ClearRenderTarget();
    TransitionToPresent();
    ExecuteCommandList();
    PresentFrame();

    WaitForPreviousFrame();
    
}

/**
 * @brief Reinicia el asignador y la lista de comandos para preparar un nuevo fotograma.
 *
 * Esta funci�n garantiza que el asignador y la lista de comandos est�n listos para recibir
 * los comandos de renderizado necesarios para el siguiente fotograma.
 */
void DirectX12RendererAPI::ResetCommandObjects() {
    commandAllocator->Reset();
    commandList->Reset(commandAllocator.Get(), nullptr);
}

/**
 * @brief Cambia el estado del recurso a RENDER_TARGET para permitir operaciones de renderizado.
 *
 * Establece una barrera de recursos que asegura que el recurso est� en estado de destino de renderizado,
 * permitiendo que se puedan ejecutar operaciones de dibujo.
 */
void DirectX12RendererAPI::TransitionToRenderTarget() {
    CD3DX12_RESOURCE_BARRIER barrierToRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(
        renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandList->ResourceBarrier(1, &barrierToRenderTarget);
}

/**
 * @brief Limpia el buffer de destino de renderizado con un color predeterminado.
 *
 * Esta funci�n establece un color base para limpiar la vista de destino de renderizado antes de
 * realizar cualquier operaci�n de dibujo.
 */
void DirectX12RendererAPI::ClearRenderTarget() {
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);
    FLOAT clearColor[] = { 0.1f, 0.1f, 0.2f, 1.0f };
    commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
}

/**
 * @brief Cambia el estado del recurso a PRESENT para que est� listo para mostrarse en pantalla.
 *
 * Establece una barrera de recursos que asegura que el recurso est� en estado de presentaci�n
 * para que el SwapChain pueda mostrarlo.
 */
void DirectX12RendererAPI::TransitionToPresent() {
    CD3DX12_RESOURCE_BARRIER barrierToPresent = CD3DX12_RESOURCE_BARRIER::Transition(
        renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    commandList->ResourceBarrier(1, &barrierToPresent);
}

/**
 * @brief Cierra la lista de comandos y la env�a a la cola de comandos para su ejecuci�n.
 *
 * Esta funci�n garantiza que los comandos de renderizado se env�en al GPU para que se ejecuten
 * de manera eficiente.
 */
void DirectX12RendererAPI::ExecuteCommandList() {
    commandList->Close();
    ID3D12CommandList* commandLists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
}

/**
 * @brief Presenta el fotograma en el SwapChain para mostrarlo en pantalla.
 *
 * Realiza la operaci�n de presentaci�n del SwapChain para el fotograma actual.
 * @throws std::runtime_error si la presentaci�n del SwapChain falla.
 */
void DirectX12RendererAPI::PresentFrame() {
    HRESULT hr = swapChain->Present(1, 0);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to present the swap chain.");
    }
}


/**
 * @brief Cambia el tama�o del renderizado en DirectX 12.
 * @param width Ancho de la ventana.
 * @param height Alto de la ventana.
 */
void DirectX12RendererAPI::Resize(int width, int height) {
    // Implementar l�gica para ajustar el tama�o del swap chain y RTV.
}

/**
 * @brief Limpia los recursos utilizados por la API de DirectX 12.
 */
void DirectX12RendererAPI::CleanUp() {
    if (device) {
        // Espera a que el GPU haya terminado de procesar el frame actual
        WaitForPreviousFrame();

        // Cierra los objetos de sincronizaci�n y libera los recursos de DirectX
        CloseSynchronizationObjects();
        ReleaseDirectXResources();
    }
}

/**
 * @brief Cierra los objetos de sincronizaci�n utilizados para la comunicaci�n CPU-GPU.
 *
 * Libera y cierra los eventos de sincronizaci�n, asegurando que no queden objetos abiertos
 * que puedan provocar fugas de memoria.
 */
void DirectX12RendererAPI::CloseSynchronizationObjects() {
    if (fenceEvent) {
        CloseHandle(fenceEvent);
        fenceEvent = nullptr;
    }
}

/**
 * @brief Libera todos los recursos de DirectX en el orden inverso al de creaci�n.
 *
 * Este m�todo asegura que todos los objetos de DirectX se liberen correctamente,
 * evitando fugas de memoria y recursos en uso.
 */
void DirectX12RendererAPI::ReleaseDirectXResources() {
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

/**
 * @brief Crea el dispositivo DirectX 12 necesario para el renderizado.
 *
 * Este m�todo intenta crear un dispositivo DirectX 12 con el nivel de caracter�stica D3D_FEATURE_LEVEL_11_0.
 * El dispositivo se requiere para todas las operaciones de renderizado, y su inicializaci�n es fundamental.
 *
 * @throws std::runtime_error si la creaci�n del dispositivo falla, indicando que no se puede continuar sin un dispositivo v�lido.
 */
void DirectX12RendererAPI::CreateDevice() {
    if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)))) {
        throw std::runtime_error("Failed to create DirectX 12 Device.");
    }
}

/**
 * @brief Crea la cola de comandos necesaria para enviar comandos al GPU.
 *
 * Este m�todo configura y crea una cola de comandos de tipo D3D12_COMMAND_LIST_TYPE_DIRECT, que se utiliza
 * para enviar comandos de renderizado al GPU. La cola de comandos es fundamental para la ejecuci�n de
 * comandos gr�ficos en DirectX 12.
 *
 * @throws std::runtime_error si la creaci�n de la cola de comandos falla, lo que podr�a indicar que el
 * dispositivo DirectX 12 no est� correctamente inicializado o que el sistema no es compatible.
 */
void DirectX12RendererAPI::CreateCommandQueue() {
    // Configuraci�n de la descripci�n de la cola de comandos
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    // Intento de creaci�n de la cola de comandos
    HRESULT result = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));
    if (FAILED(result)) {
        throw std::runtime_error("Failed to create Command Queue. Ensure the DirectX 12 device is correctly initialized.");
    }
}

/**
 * @brief Crea el SwapChain para la ventana y configura los buffers de renderizado.
 * @param hwnd Handle de la ventana donde se renderizar�.
 *
 * @throws std::runtime_error si la creaci�n del SwapChain falla.
 */
void DirectX12RendererAPI::CreateSwapChain(HWND hwnd) {
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
    ConfigureSwapChainDesc(swapChainDesc);

    Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
    CreateDXGIFactory(factory);

    CreateSwapChainForWindow(hwnd, swapChainDesc);

    frameIndex = swapChain->GetCurrentBackBufferIndex();
}

/**
 * @brief Crea y configura las vistas de destino de renderizado (RTVs) para cada buffer en el SwapChain.
 *
 * Este m�todo crea el heap de descriptores para las RTVs y luego configura una vista de destino de renderizado
 * para cada buffer en el SwapChain.
 * @throws std::runtime_error si falla la creaci�n del heap o de las vistas de destino.
 */
void DirectX12RendererAPI::CreateRenderTargetView() {
    CreateRTVHeap();
    CreateRenderTargetViews();
}

/**
 * @brief Crea los objetos necesarios para el manejo de comandos, incluyendo el asignador, la lista de comandos y la sincronizaci�n.
 *
 * Este m�todo configura el `Command Allocator`, el `Command List` y el objeto `Fence` con su evento de sincronizaci�n.
 * Estos son esenciales para el env�o de comandos al GPU y la sincronizaci�n entre la CPU y el GPU en DirectX 12.
 *
 * - `Command Allocator`: Almacena y organiza los comandos de renderizado.
 * - `Command List`: Lista de comandos para enviar instrucciones al GPU.
 * - `Fence` y `Fence Event`: Objeto y evento necesarios para coordinar la finalizaci�n de comandos del GPU.
 *
 * @throws std::runtime_error si falla la creaci�n del asignador, la lista de comandos o el objeto de sincronizaci�n `Fence`.
 */
void DirectX12RendererAPI::CreateCommandObjects() {
    CreateCommandAllocator();
    CreateCommandList();
    CreateFenceAndEvent();
}

/**
 * @brief Espera a que el GPU complete el procesamiento del frame anterior.
 *
 * Sincroniza la CPU y el GPU para asegurar que el GPU haya terminado el trabajo actual
 * antes de avanzar al siguiente fotograma.
 * @throws std::runtime_error si el fence o la cola de comandos no est�n inicializados.
 */
void DirectX12RendererAPI::WaitForPreviousFrame() {

    
    const UINT64 currentFenceValue = fenceValue;
    SignalFence(currentFenceValue);
    fenceValue++;

    WaitForFenceCompletion(currentFenceValue);

    frameIndex = swapChain->GetCurrentBackBufferIndex();
}

/**
 * @brief Se�aliza el fence con el valor actual para indicar que los comandos previos est�n listos.
 * @param fenceValue Valor del fence a establecer.
 *
 * Llama al m�todo Signal de la cola de comandos para indicar que los comandos hasta este punto han sido enviados al GPU.
 * @throws std::runtime_error si falla la se�alizaci�n del fence.
 */
void DirectX12RendererAPI::SignalFence(UINT64 fenceValue) {
    HRESULT hr = commandQueue->Signal(fence.Get(), fenceValue);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to signal command queue.");
    }
}

/**
 * @brief Espera hasta que el fence alcance el valor especificado.
 * @param fenceValue Valor del fence que se debe alcanzar.
 *
 * Configura un evento que se dispara cuando el fence alcanza el valor especificado, y luego espera a que el evento ocurra.
 * @throws std::runtime_error si falla la configuraci�n del evento en el fence.
 */
void DirectX12RendererAPI::WaitForFenceCompletion(UINT64 fenceValue) {
    if (fence->GetCompletedValue() < fenceValue) {
        HRESULT hr = fence->SetEventOnCompletion(fenceValue, fenceEvent);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to set event on completion.");
        }
        WaitForSingleObject(fenceEvent, INFINITE);
    }
}

/**
 * @brief Configura la descripci�n del SwapChain con los par�metros necesarios.
 * @param desc Referencia a la descripci�n del SwapChain a configurar.
 *
 * Define par�metros como el n�mero de buffers, el formato de color y el modo de intercambio.
 */
void DirectX12RendererAPI::ConfigureSwapChainDesc(DXGI_SWAP_CHAIN_DESC1& desc) {
    desc = {};
    desc.BufferCount = FrameCount;
    desc.Width = 800;
    desc.Height = 600;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.SampleDesc.Count = 1;
}

/**
 * @brief Crea la f�brica DXGI necesaria para la creaci�n del SwapChain.
 * @param factory Referencia al puntero inteligente donde se almacenar� la f�brica DXGI.
 *
 * @throws std::runtime_error si la creaci�n de la f�brica DXGI falla.
 */
void DirectX12RendererAPI::CreateDXGIFactory(Microsoft::WRL::ComPtr<IDXGIFactory4>& factory) {
    HRESULT result = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
    if (FAILED(result)) {
        throw std::runtime_error("Failed to create DXGI Factory.");
    }
}

/**
 * @brief Crea el SwapChain para la ventana especificada y lo convierte en el SwapChain final.
 * @param hwnd Handle de la ventana donde se renderizar�.
 * @param desc Descripci�n del SwapChain que contiene los par�metros configurados.
 *
 * @throws std::runtime_error si la creaci�n del SwapChain falla.
 */
void DirectX12RendererAPI::CreateSwapChainForWindow(HWND hwnd, const DXGI_SWAP_CHAIN_DESC1& desc) {
    Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChainTemp;
    Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
    CreateDXGIFactory(factory);

    HRESULT result = factory->CreateSwapChainForHwnd(
        commandQueue.Get(), hwnd, &desc, nullptr, nullptr, &swapChainTemp);
    if (FAILED(result)) {
        throw std::runtime_error("Failed to create Swap Chain.");
    }

    swapChainTemp.As(&swapChain);
}

/**
 * @brief Crea el heap de descriptores para las vistas de destino de renderizado (RTVs).
 *
 * Configura y crea el descriptor heap de tipo RTV, necesario para manejar las vistas de destino de renderizado.
 * @throws std::runtime_error si falla la creaci�n del heap de descriptores.
 */
void DirectX12RendererAPI::CreateRTVHeap() {
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = FrameCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    HRESULT result = device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));
    if (FAILED(result)) {
        throw std::runtime_error("Failed to create RTV Descriptor Heap.");
    }

    rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

/**
 * @brief Configura las vistas de destino de renderizado (RTVs) para cada buffer en el SwapChain.
 *
 * Recorre cada buffer en el SwapChain, creando una vista de destino de renderizado y configur�ndola
 * en el heap de RTV.
 * @throws std::runtime_error si falla la obtenci�n de un buffer o la configuraci�n de la vista de destino.
 */
void DirectX12RendererAPI::CreateRenderTargetViews() {
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());

    for (UINT i = 0; i < FrameCount; i++) {
        HRESULT result = swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
        if (FAILED(result)) {
            throw std::runtime_error("Failed to get buffer for Render Target View.");
        }

        device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, rtvDescriptorSize);
    }
}

/**
 * @brief Crea el asignador de comandos necesario para almacenar comandos de renderizado.
 *
 * Este m�todo crea un `Command Allocator` de tipo `D3D12_COMMAND_LIST_TYPE_DIRECT`,
 * que organiza los comandos antes de ser enviados al GPU.
 * @throws std::runtime_error si falla la creaci�n del asignador de comandos.
 */
void DirectX12RendererAPI::CreateCommandAllocator() {
    HRESULT result = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
    if (FAILED(result)) {
        throw std::runtime_error("Failed to create Command Allocator.");
    }
}


/**
 * @brief Crea la lista de comandos que se utilizar� para enviar comandos al GPU.
 *
 * Configura una `Command List` de tipo `D3D12_COMMAND_LIST_TYPE_DIRECT` enlazada al `Command Allocator`.
 * La lista se cierra inmediatamente despu�s de su creaci�n para que est� en un estado inicial v�lido.
 * @throws std::runtime_error si falla la creaci�n de la lista de comandos.
 */
void DirectX12RendererAPI::CreateCommandList() {
    HRESULT result = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
    if (FAILED(result)) {
        throw std::runtime_error("Failed to create Command List.");
    }

    // Cierra la lista de comandos para que est� en un estado v�lido inicial
    commandList->Close();
}

/**
 * @brief Crea el fence y el evento de sincronizaci�n necesarios para la sincronizaci�n CPU-GPU.
 *
 * El fence se usa para coordinar el renderizado, asegurando que el GPU haya completado el trabajo
 * de un fotograma antes de que la CPU avance al siguiente. El evento se configura para esperar la
 * se�al del fence.
 *
 * @throws std::runtime_error si falla la creaci�n del evento o el fence.
 */
void DirectX12RendererAPI::CreateFenceAndEvent() {
    // Crear el evento de sincronizaci�n del fence
    fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!fenceEvent) {
        throw std::runtime_error("Failed to create fence event.");
    }

    // Crear el objeto fence para la sincronizaci�n con el GPU
    HRESULT result = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    if (FAILED(result)) {
        throw std::runtime_error("Failed to create Fence.");
    }

    // Valor inicial del fence
    fenceValue = 1;
}


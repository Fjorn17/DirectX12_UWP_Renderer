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
  * @brief Implementaci�n de IRenderAPI para manejar el renderizado usando DirectX 12.
  */
class DirectX12RendererAPI : public IRenderAPI {
public:
    /**
    * @brief Constructor de DirectX12RendererAPI.
    *
    * Inicializa los valores por defecto de los atributos de la clase.
    */
    DirectX12RendererAPI();

    /**
    * @brief Destructor de DirectX12RendererAPI.
    *
    * Libera los recursos y llama a CleanUp() para asegurarse de que
    * todos los recursos de DirectX est�n adecuadamente liberados.
    */
    ~DirectX12RendererAPI();

    /**
    * @brief Inicializa los componentes necesarios para la API de DirectX 12.
    * @param hwnd Handle de la ventana donde se renderizar�.
    */
    void Initialize(void* hwnd);

    /**
    * @brief Renderiza un fotograma utilizando DirectX 12.
    *
    * Ejecuta la l�gica de renderizado, incluyendo la configuraci�n de barreras de recursos,
    * la limpieza de vistas de destino de renderizado, y la presentaci�n del `SwapChain`.
    */
    void RenderFrame();

    /**
    * @brief Cambia el tama�o del renderizado en DirectX 12.
    * @param width Ancho de la ventana.
    * @param height Alto de la ventana.
    *
    * Redimensiona los recursos del SwapChain y las vistas de renderizado.
    */
    void Resize(int width, int height);

    /**
    * @brief Limpia los recursos utilizados por la API de DirectX 12.
    *
    * Libera todos los recursos de DirectX y cierra el evento de sincronizaci�n
    * asegurando que el GPU haya completado el trabajo actual.
    */
    void CleanUp();

private:
    /**
    * @brief Inicializa los componentes de DirectX 12 como el dispositivo y la cola de comandos.
    */
    void InitializeDeviceComponents();

    /**
    * @brief Inicializa el SwapChain que se utiliza para la presentaci�n de fotogramas.
    * @param hwnd Handle de la ventana donde se renderizar�.
    */
    void InitializeSwapChain(HWND hwnd);

    /**
    * @brief Configura la vista de destino de renderizado (Render Target View) para el renderizado.
    */
    void InitializeRenderTargetView();

    /**
    * @brief Inicializa los objetos de comando necesarios para el renderizado.
    */
    void InitializeCommandObjects();

    /**
    * @brief Crea y configura los objetos de sincronizaci�n necesarios para la comunicaci�n CPU-GPU.
    *
    * Este m�todo configura el evento de sincronizaci�n (fence event), necesario para coordinar
    * el flujo entre la CPU y GPU.
    */
    void InitializeSynchronizationObjects();

    /**
    * @brief Crea el dispositivo DirectX 12.
    *
    * Intenta crear un dispositivo DirectX 12 en el nivel de caracter�stica D3D_FEATURE_LEVEL_11_0.
    * @throws std::runtime_error si no se puede crear el dispositivo.
    */
    void CreateDevice();

    /**
    * @brief Crea la cola de comandos necesaria para enviar comandos al GPU.
    *
    * Configura una cola de comandos del tipo D3D12_COMMAND_LIST_TYPE_DIRECT.
    * @throws std::runtime_error si no se puede crear la cola de comandos.
    */
    void CreateCommandQueue();

    /**
    * @brief Crea el SwapChain para intercambiar los buffers de renderizado.
    * @param hwnd Handle de la ventana donde se renderizar�.
    * @throws std::runtime_error si no se puede crear el SwapChain.
    */
    void CreateSwapChain(HWND hwnd);

    /**
    * @brief Crea y configura las vistas de destino de renderizado.
    *
    * Configura las vistas de destino de renderizado (Render Target Views) necesarias para
    * el renderizado en cada buffer del SwapChain.
    * @throws std::runtime_error si no se puede crear la vista de destino de renderizado.
    */
    void CreateRenderTargetView();

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
    void CreateCommandObjects();

    /**
     * @brief Crea el fence y el evento de sincronizaci�n necesarios para la sincronizaci�n CPU-GPU.
     *
     * El fence se usa para coordinar el renderizado, asegurando que el GPU haya completado el trabajo
     * de un fotograma antes de que la CPU avance al siguiente. El evento se configura para esperar la
     * se�al del fence.
     *
     * @throws std::runtime_error si falla la creaci�n del evento o el fence.
     */
    void CreateFenceAndEvent();

    /**
        * @brief Espera a que el GPU complete el procesamiento del frame anterior.
        *
        * Sincroniza la CPU y el GPU para asegurar que el GPU haya terminado el trabajo actual
        * antes de avanzar al siguiente fotograma.
        * @throws std::runtime_error si el fence o la cola de comandos no est�n inicializados.
        */
    void WaitForPreviousFrame();

    /**
    * @brief Reinicia el asignador y la lista de comandos para preparar un nuevo fotograma.
    *
    * Esta funci�n garantiza que el asignador y la lista de comandos est�n listos para recibir
    * los comandos de renderizado necesarios para el siguiente fotograma.
    */
    void ResetCommandObjects();

    /**
     * @brief Cambia el estado del recurso a RENDER_TARGET para permitir operaciones de renderizado.
     *
     * Establece una barrera de recursos que asegura que el recurso est� en estado de destino de renderizado,
     * permitiendo que se puedan ejecutar operaciones de dibujo.
     */
    void TransitionToRenderTarget();

    /**
     * @brief Limpia el buffer de destino de renderizado con un color predeterminado.
     *
     * Esta funci�n establece un color base para limpiar la vista de destino de renderizado antes de
     * realizar cualquier operaci�n de dibujo.
     */
    void ClearRenderTarget();

    /**
     * @brief Cambia el estado del recurso a PRESENT para que est� listo para mostrarse en pantalla.
     *
     * Establece una barrera de recursos que asegura que el recurso est� en estado de presentaci�n
     * para que el SwapChain pueda mostrarlo.
     */
    void TransitionToPresent();

    /**
     * @brief Cierra la lista de comandos y la env�a a la cola de comandos para su ejecuci�n.
     *
     * Esta funci�n garantiza que los comandos de renderizado se env�en al GPU para que se ejecuten
     * de manera eficiente.
     */
    void ExecuteCommandList();

    /**
     * @brief Presenta el fotograma en el SwapChain para mostrarlo en pantalla.
     *
     * Realiza la operaci�n de presentaci�n del SwapChain para el fotograma actual.
     * @throws std::runtime_error si la presentaci�n del SwapChain falla.
     */
    void PresentFrame();

    /**
    * @brief Cierra los objetos de sincronizaci�n utilizados para la comunicaci�n CPU-GPU.
    *
    * Libera y cierra los eventos de sincronizaci�n, asegurando que no queden objetos abiertos
    * que puedan provocar fugas de memoria.
    */
    void CloseSynchronizationObjects();

    /**
     * @brief Libera todos los recursos de DirectX en el orden inverso al de creaci�n.
     *
     * Este m�todo asegura que todos los objetos de DirectX se liberen correctamente,
     * evitando fugas de memoria y recursos en uso.
     */
    void ReleaseDirectXResources();

    /**
     * @brief Se�aliza el fence con el valor actual para indicar que los comandos previos est�n listos.
     * @param fenceValue Valor del fence a establecer.
     *
     * Llama al m�todo Signal de la cola de comandos para indicar que los comandos hasta este punto han sido enviados al GPU.
     * @throws std::runtime_error si falla la se�alizaci�n del fence.
     */
    void SignalFence(UINT64 fenceValue);

    /**
     * @brief Espera hasta que el fence alcance el valor especificado.
     * @param fenceValue Valor del fence que se debe alcanzar.
     *
     * Configura un evento que se dispara cuando el fence alcanza el valor especificado, y luego espera a que el evento ocurra.
     * @throws std::runtime_error si falla la configuraci�n del evento en el fence.
     */
    void WaitForFenceCompletion(UINT64 fenceValue);

    /**
     * @brief Configura la descripci�n del SwapChain con los par�metros necesarios.
     * @param desc Referencia a la descripci�n del SwapChain a configurar.
     *
     * Define par�metros como el n�mero de buffers, el formato de color y el modo de intercambio.
     */
    void ConfigureSwapChainDesc(DXGI_SWAP_CHAIN_DESC1& desc);

    /**
     * @brief Crea la f�brica DXGI necesaria para la creaci�n del SwapChain.
     * @param factory Referencia al puntero inteligente donde se almacenar� la f�brica DXGI.
     *
     * @throws std::runtime_error si la creaci�n de la f�brica DXGI falla.
     */
    void CreateDXGIFactory(Microsoft::WRL::ComPtr<IDXGIFactory4>& factory);

    /**
     * @brief Crea el SwapChain para la ventana especificada y lo convierte en el SwapChain final.
     * @param hwnd Handle de la ventana donde se renderizar�.
     * @param desc Descripci�n del SwapChain que contiene los par�metros configurados.
     *
     * @throws std::runtime_error si la creaci�n del SwapChain falla.
     */
    void CreateSwapChainForWindow(HWND hwnd, const DXGI_SWAP_CHAIN_DESC1& desc);

    /**
     * @brief Crea el heap de descriptores para las vistas de destino de renderizado (RTVs).
     *
     * Configura y crea el descriptor heap de tipo RTV, necesario para manejar las vistas de destino de renderizado.
     * @throws std::runtime_error si falla la creaci�n del heap de descriptores.
     */
    void CreateRTVHeap();

    /**
     * @brief Configura las vistas de destino de renderizado (RTVs) para cada buffer en el SwapChain.
     *
     * Recorre cada buffer en el SwapChain, creando una vista de destino de renderizado y configur�ndola
     * en el heap de RTV.
     * @throws std::runtime_error si falla la obtenci�n de un buffer o la configuraci�n de la vista de destino.
     */
    void CreateRenderTargetViews();

    /**
     * @brief Crea el asignador de comandos necesario para almacenar comandos de renderizado.
     *
     * Este m�todo crea un `Command Allocator` de tipo `D3D12_COMMAND_LIST_TYPE_DIRECT`,
     * que organiza los comandos antes de ser enviados al GPU.
     * @throws std::runtime_error si falla la creaci�n del asignador de comandos.
     */
    void CreateCommandAllocator();

    /**
     * @brief Crea la lista de comandos que se utilizar� para enviar comandos al GPU.
     *
     * Configura una `Command List` de tipo `D3D12_COMMAND_LIST_TYPE_DIRECT` enlazada al `Command Allocator`.
     * La lista se cierra inmediatamente despu�s de su creaci�n para que est� en un estado inicial v�lido.
     * @throws std::runtime_error si falla la creaci�n de la lista de comandos.
     */
    void CreateCommandList();

    static const UINT FrameCount = 2; ///< N�mero de fotogramas en la cadena de intercambio

    Microsoft::WRL::ComPtr<ID3D12Device> device; ///< Dispositivo DirectX 12
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue; ///< Cola de comandos DirectX 12
    Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain; ///< Cadena de intercambio DirectX 12
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap; ///< Heap de descriptores para vistas de renderizado
    Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets[FrameCount]; ///< Vistas de renderizado
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator; ///< Allocator de comandos
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList; ///< Lista de comandos de gr�ficos
    Microsoft::WRL::ComPtr<ID3D12Fence> fence; ///< Fence para sincronizaci�n GPU-CPU
    UINT rtvDescriptorSize; ///< Tama�o del descriptor RTV
    UINT frameIndex; ///< �ndice del fotograma actual
    HANDLE fenceEvent; ///< Evento para la sincronizaci�n del fence
    UINT64 fenceValue; ///< Valor del fence para sincronizaci�n
};

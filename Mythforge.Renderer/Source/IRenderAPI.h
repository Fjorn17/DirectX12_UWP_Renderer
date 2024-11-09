#pragma once

class IRenderAPI {
public:
    virtual ~IRenderAPI() = default;

    // Inicializa la API de renderizado con un handle de ventana.
    virtual void Initialize(HWND hwnd) = 0;

    // Libera todos los recursos de la API de renderizado.
    virtual void Destroy() = 0;

    // Actualiza la perspectiva del viewport.
    virtual void UpdateViewportPerspective(UINT width, UINT height) = 0;

    // Cambia el tama�o del �rea de renderizado.
    virtual void Resize(UINT width, UINT height) = 0;

    // Prepara la API para nuevos comandos.
    virtual void ResetCommands() = 0;

    // Cierra los comandos actuales y asegura su ejecuci�n.
    virtual void CloseCommandsAndFlush() = 0;

    // Configura los render targets para la siguiente operaci�n de renderizado.
    virtual void SetRenderTargets() = 0;

    // Presenta el frame actual en pantalla.
    virtual void Present() = 0;
};

/**
 * @file Renderer.h
 * @brief Define la clase Renderer para gestionar el renderizado utilizando una API de renderizado genérica.
 */

#pragma once
#include "IRenderAPI.h"
#include <memory>

 /**
  * @class Renderer
  * @brief Clase que proporciona la funcionalidad de renderizado utilizando una API de renderizado.
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

    /**
     * @brief Cambia el tamaño del renderizado según las dimensiones de la ventana.
     * @param width Ancho de la ventana.
     * @param height Alto de la ventana.
     */
    void Resize(int width, int height);

private:
    std::unique_ptr<IRenderAPI> renderAPI; ///< Instancia de la API de renderizado utilizada.
};

extern "C" {
    MYTHFORGE_API Renderer* CreateRenderer();
    MYTHFORGE_API void DestroyRenderer(Renderer* renderer);
    MYTHFORGE_API void InitializeRenderer(Renderer* renderer, HWND hwnd);
    MYTHFORGE_API void RenderFrame(Renderer* renderer);
    MYTHFORGE_API void ResizeRenderer(Renderer* renderer, int width, int height);
}

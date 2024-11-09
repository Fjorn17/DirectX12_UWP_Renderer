/**
 * @file Renderer.cpp
 * @brief Implementación de la clase Renderer utilizando una API de renderizado a través de IRenderAPI.
 */
#include "pch.h"
#include "Renderer.h"
#include "RenderAPIFactory.h"
#include <stdexcept>
#include <iostream>

 /**
  * @brief Constructor de la clase Renderer.
  *
  * Crea una instancia de la API de renderizado a través de RenderAPIFactory.
  */
Renderer::Renderer() {
    renderAPI = RenderAPIFactory::CreateRenderAPI(RenderAPIType::DirectX12); // Selecciona DirectX12 como ejemplo
    if (!renderAPI) {
        throw std::runtime_error("No se pudo crear la instancia de RenderAPI.");
    }
}

/**
 * @brief Destructor de la clase Renderer.
 *
 * Libera los recursos de la API de renderizado.
 */
Renderer::~Renderer() {
    if (renderAPI) {
        renderAPI->Destroy(); // Libera los recursos de la API de renderizado
        renderAPI.reset();
    }
}

/**
 * @brief Inicializa el Renderer utilizando la API de renderizado seleccionada.
 * @param hwnd Handle de la ventana donde se renderizará.
 */
void Renderer::Initialize(HWND hwnd) {
    if (renderAPI) {
        renderAPI->Initialize(hwnd);
    }
}

/**
 * @brief Renderiza un fotograma utilizando la API de renderizado seleccionada.
 */
void Renderer::RenderFrame() {
    std::cerr << "ERROR antes de ResetCommands" << std::endl;
    if (renderAPI) {
        renderAPI->ResetCommands();
        std::cerr << "ERROR después de ResetCommands" << std::endl;

        std::cerr << "ERROR antes de SetRenderTargets" << std::endl;
        renderAPI->SetRenderTargets();
        std::cerr << "ERROR después de SetRenderTargets" << std::endl;

        // Otros comandos de renderizado, si los hubiera

        std::cerr << "ERROR antes de Present" << std::endl;
        renderAPI->Present();
        std::cerr << "ERROR después de Present" << std::endl;
    }
}

/**
 * @brief Cambia el tamaño del renderizado según las dimensiones de la ventana.
 * @param width Ancho de la ventana.
 * @param height Alto de la ventana.
 */
void Renderer::Resize(int width, int height) {
    if (renderAPI) {
        renderAPI->Resize(width, height);
    }
}

/**
 * @brief Función C para crear una instancia de Renderer.
 * @return Un puntero a la instancia de Renderer.
 */
extern "C" MYTHFORGE_API Renderer* CreateRenderer() {
    return new Renderer();
}

/**
 * @brief Función C para destruir una instancia de Renderer.
 * @param renderer Puntero a la instancia de Renderer a destruir.
 */
extern "C" MYTHFORGE_API void DestroyRenderer(Renderer* renderer) {
    delete renderer;
}

/**
 * @brief Función C para inicializar el Renderer con un handle de ventana.
 * @param renderer Puntero a la instancia de Renderer.
 * @param hwnd Handle de la ventana.
 */
extern "C" MYTHFORGE_API void InitializeRenderer(Renderer* renderer, HWND hwnd) {
    if (renderer) {
        renderer->Initialize(hwnd);
    }
}

/**
 * @brief Función C para renderizar un fotograma.
 * @param renderer Puntero a la instancia de Renderer.
 */
extern "C" MYTHFORGE_API void RenderFrame(Renderer* renderer) {
    if (renderer) {
        renderer->RenderFrame();
    }
}

/**
 * @brief Función C para cambiar el tamaño del renderizado.
 * @param renderer Puntero a la instancia de Renderer.
 * @param width Ancho de la ventana.
 * @param height Alto de la ventana.
 */
extern "C" MYTHFORGE_API void ResizeRenderer(Renderer* renderer, int width, int height) {
    if (renderer) {
        renderer->Resize(width, height);
    }
}

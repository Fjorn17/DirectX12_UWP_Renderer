/**
 * @file Renderer.cpp
 * @brief Implementaci�n de la clase Renderer utilizando una API de renderizado a trav�s de IRenderAPI.
 */
#include "pch.h"
#include "Renderer.h"
#include "RenderAPIFactory.h"
#include <stdexcept>
#include <iostream>

 /**
  * @brief Constructor de la clase Renderer.
  *
  * Crea una instancia de la API de renderizado a trav�s de RenderAPIFactory.
  */
Renderer::Renderer() {
    renderAPI = RenderAPIFactory::CreateRenderAPI(RenderAPIType::DirectX12); // Selecciona DirectX12 como ejemplo
}

/**
 * @brief Destructor de la clase Renderer.
 *
 * Libera los recursos de la API de renderizado.
 */
Renderer::~Renderer() {
    if (renderAPI) {
        renderAPI->CleanUp();
    }
}

/**
 * @brief Inicializa el Renderer utilizando la API de renderizado seleccionada.
 * @param hwnd Handle de la ventana donde se renderizar�.
 */
void Renderer::Initialize(HWND hwnd) {
    renderAPI->Initialize(hwnd);
}

/**
 * @brief Renderiza un fotograma utilizando la API de renderizado seleccionada.
 */
void Renderer::RenderFrame() {
    renderAPI->RenderFrame();
}

/**
 * @brief Cambia el tama�o del renderizado seg�n las dimensiones de la ventana.
 * @param width Ancho de la ventana.
 * @param height Alto de la ventana.
 */
void Renderer::Resize(int width, int height) {
    renderAPI->Resize(width, height);
}

/**
 * @brief Funci�n C para crear una instancia de Renderer.
 * @return Un puntero a la instancia de Renderer.
 */
extern "C" MYTHFORGE_API Renderer* CreateRenderer() {
    return new Renderer();
}

/**
 * @brief Funci�n C para destruir una instancia de Renderer.
 * @param renderer Puntero a la instancia de Renderer a destruir.
 */
extern "C" MYTHFORGE_API void DestroyRenderer(Renderer* renderer) {
    delete renderer;
}

/**
 * @brief Funci�n C para inicializar el Renderer con un handle de ventana.
 * @param renderer Puntero a la instancia de Renderer.
 * @param hwnd Handle de la ventana.
 */
extern "C" MYTHFORGE_API void InitializeRenderer(Renderer* renderer, HWND hwnd) {
    if (renderer) {
        renderer->Initialize(hwnd);
    }
}

/**
 * @brief Funci�n C para renderizar un fotograma.
 * @param renderer Puntero a la instancia de Renderer.
 */
extern "C" MYTHFORGE_API void RenderFrame(Renderer* renderer) {
    if (renderer) {
        renderer->RenderFrame();
    }
}

/**
 * @brief Funci�n C para cambiar el tama�o del renderizado.
 * @param renderer Puntero a la instancia de Renderer.
 * @param width Ancho de la ventana.
 * @param height Alto de la ventana.
 */
extern "C" MYTHFORGE_API void ResizeRenderer(Renderer* renderer, int width, int height) {
    if (renderer) {
        renderer->Resize(width, height);
    }
}

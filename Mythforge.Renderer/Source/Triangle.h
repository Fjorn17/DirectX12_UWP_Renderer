/**
 * @file Triangle.h
 * @brief Declaración del struct Triangle que representa un triángulo en una malla.
 */

#pragma once
#include "Vertex.h"

 /**
  * @struct Triangle
  * @brief Estructura que representa un triángulo formado por tres vértices.
  */
struct Triangle
{
    Vertex v0; ///< Primer vértice del triángulo.
    Vertex v1; ///< Segundo vértice del triángulo.
    Vertex v2; ///< Tercer vértice del triángulo.

    /**
     * @brief Constructor por defecto del triángulo.
     */
    Triangle() = default;

    /**
     * @brief Constructor que inicializa los vértices del triángulo.
     * @param vertex0 Primer vértice del triángulo.
     * @param vertex1 Segundo vértice del triángulo.
     * @param vertex2 Tercer vértice del triángulo.
     */
    Triangle(const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2)
        : v0(vertex0), v1(vertex1), v2(vertex2) {}
};
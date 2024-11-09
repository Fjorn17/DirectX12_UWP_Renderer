/**
 * @file Triangle.h
 * @brief Declaraci�n del struct Triangle que representa un tri�ngulo en una malla.
 */

#pragma once
#include "Vertex.h"

 /**
  * @struct Triangle
  * @brief Estructura que representa un tri�ngulo formado por tres v�rtices.
  */
struct Triangle
{
    Vertex v0; ///< Primer v�rtice del tri�ngulo.
    Vertex v1; ///< Segundo v�rtice del tri�ngulo.
    Vertex v2; ///< Tercer v�rtice del tri�ngulo.

    /**
     * @brief Constructor por defecto del tri�ngulo.
     */
    Triangle() = default;

    /**
     * @brief Constructor que inicializa los v�rtices del tri�ngulo.
     * @param vertex0 Primer v�rtice del tri�ngulo.
     * @param vertex1 Segundo v�rtice del tri�ngulo.
     * @param vertex2 Tercer v�rtice del tri�ngulo.
     */
    Triangle(const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2)
        : v0(vertex0), v1(vertex1), v2(vertex2) {}
};
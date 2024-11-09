/**
 * @file Vertex.cpp
 * @brief Implementación de la clase Vertex que representa un vértice en una malla.
 */

#include "pch.h"
#include "Vertex.h"

 /**
  * @brief Constructor de la clase Vertex.
  * @param position Posición del vértice como Vector3.
  * @param normal Normal del vértice como Vector3.
  * @param uv Coordenadas de textura (UV) del vértice como Vector2.
  * @param tangent Vector tangente del vértice como Vector3 (opcional).
  */
Vertex::Vertex(const Vector3& position, const Vector3& normal, const Vector2& uv, const Vector3& tangent)
    : position(position), normal(normal), uv(uv), tangent(tangent) {}

/**
 * @brief Obtiene la posición del vértice.
 * @return La posición del vértice como Vector3.
 */
Vector3 Vertex::GetPosition() const {
    return position;
}

/**
 * @brief Obtiene la normal del vértice.
 * @return La normal del vértice como Vector3.
 */
Vector3 Vertex::GetNormal() const {
    return normal;
}

/**
 * @brief Obtiene las coordenadas de textura (UV) del vértice.
 * @return Las coordenadas UV del vértice como Vector2.
 */
Vector2 Vertex::GetUV() const {
    return uv;
}

/**
 * @brief Obtiene el vector tangente del vértice.
 * @return El vector tangente como Vector3.
 */
Vector3 Vertex::GetTangent() const {
    return tangent;
}

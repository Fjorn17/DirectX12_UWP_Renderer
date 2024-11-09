/**
 * @file Vertex.cpp
 * @brief Implementaci�n de la clase Vertex que representa un v�rtice en una malla.
 */

#include "pch.h"
#include "Vertex.h"

 /**
  * @brief Constructor de la clase Vertex.
  * @param position Posici�n del v�rtice como Vector3.
  * @param normal Normal del v�rtice como Vector3.
  * @param uv Coordenadas de textura (UV) del v�rtice como Vector2.
  * @param tangent Vector tangente del v�rtice como Vector3 (opcional).
  */
Vertex::Vertex(const Vector3& position, const Vector3& normal, const Vector2& uv, const Vector3& tangent)
    : position(position), normal(normal), uv(uv), tangent(tangent) {}

/**
 * @brief Obtiene la posici�n del v�rtice.
 * @return La posici�n del v�rtice como Vector3.
 */
Vector3 Vertex::GetPosition() const {
    return position;
}

/**
 * @brief Obtiene la normal del v�rtice.
 * @return La normal del v�rtice como Vector3.
 */
Vector3 Vertex::GetNormal() const {
    return normal;
}

/**
 * @brief Obtiene las coordenadas de textura (UV) del v�rtice.
 * @return Las coordenadas UV del v�rtice como Vector2.
 */
Vector2 Vertex::GetUV() const {
    return uv;
}

/**
 * @brief Obtiene el vector tangente del v�rtice.
 * @return El vector tangente como Vector3.
 */
Vector3 Vertex::GetTangent() const {
    return tangent;
}

/*!
 * @file Vertex.h
 * @brief Declaración de la clase Vertex que representa un vértice en una malla.
 */

#pragma once
#include "Vector.h"

 /**
  * @class Vertex
  * @brief Clase que representa un vértice en una malla.
  */
class Vertex
{
public:
    Vertex() = default;
    Vertex(const Vector3& position, const Vector3& normal, const Vector2& uv, const Vector3& tangent = Vector3());

    /**
     * @brief Obtiene la posición del vértice.
     * @return La posición del vértice como Vector3.
     */
    Vector3 GetPosition() const;

    /**
     * @brief Obtiene la normal del vértice.
     * @return La normal del vértice como Vector3.
     */
    Vector3 GetNormal() const;

    /**
     * @brief Obtiene las coordenadas de textura (UV) del vértice.
     * @return Las coordenadas UV del vértice como Vector2.
     */
    Vector2 GetUV() const;

    /**
     * @brief Obtiene el vector tangente del vértice.
     * @return El vector tangente como Vector3.
     */
    Vector3 GetTangent() const;

private:
    Vector3 position; ///< Posición del vértice.
    Vector3 normal;   ///< Normal del vértice para el cálculo de iluminación.
    Vector2 uv;       ///< Coordenadas de textura (UV) para mapeo de texturas.
    Vector3 tangent;  ///< Vector tangente para el cálculo de mapas de normales.
};


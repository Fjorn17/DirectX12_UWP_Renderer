/*!
 * @file Vertex.h
 * @brief Declaraci�n de la clase Vertex que representa un v�rtice en una malla.
 */

#pragma once
#include "Vector.h"

 /**
  * @class Vertex
  * @brief Clase que representa un v�rtice en una malla.
  */
class Vertex
{
public:
    Vertex() = default;
    Vertex(const Vector3& position, const Vector3& normal, const Vector2& uv, const Vector3& tangent = Vector3());

    /**
     * @brief Obtiene la posici�n del v�rtice.
     * @return La posici�n del v�rtice como Vector3.
     */
    Vector3 GetPosition() const;

    /**
     * @brief Obtiene la normal del v�rtice.
     * @return La normal del v�rtice como Vector3.
     */
    Vector3 GetNormal() const;

    /**
     * @brief Obtiene las coordenadas de textura (UV) del v�rtice.
     * @return Las coordenadas UV del v�rtice como Vector2.
     */
    Vector2 GetUV() const;

    /**
     * @brief Obtiene el vector tangente del v�rtice.
     * @return El vector tangente como Vector3.
     */
    Vector3 GetTangent() const;

private:
    Vector3 position; ///< Posici�n del v�rtice.
    Vector3 normal;   ///< Normal del v�rtice para el c�lculo de iluminaci�n.
    Vector2 uv;       ///< Coordenadas de textura (UV) para mapeo de texturas.
    Vector3 tangent;  ///< Vector tangente para el c�lculo de mapas de normales.
};


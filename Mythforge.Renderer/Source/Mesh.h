#pragma once
#include "framework.h"
#include "Vertex.h"
#include <vector>
/**
 * @class Mesh
 * @brief Clase que representa una malla sencilla para renderizado.
 */
class Mesh
{
public:
    Mesh() = default;
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    /**
     * @brief Obtiene los vértices de la malla.
     * @return Una referencia constante al vector de vértices.
     */
    const std::vector<Vertex>& GetVertices() const;

    /**
     * @brief Obtiene los índices de la malla.
     * @return Una referencia constante al vector de índices.
     */
    const std::vector<unsigned int>& GetIndices() const;

private:
    std::vector<Vertex> vertices; ///< Vértices de la malla.
    std::vector<unsigned int> indices; ///< Índices de la malla para definir triángulos.
};


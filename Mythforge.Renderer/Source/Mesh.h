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
     * @brief Obtiene los v�rtices de la malla.
     * @return Una referencia constante al vector de v�rtices.
     */
    const std::vector<Vertex>& GetVertices() const;

    /**
     * @brief Obtiene los �ndices de la malla.
     * @return Una referencia constante al vector de �ndices.
     */
    const std::vector<unsigned int>& GetIndices() const;

private:
    std::vector<Vertex> vertices; ///< V�rtices de la malla.
    std::vector<unsigned int> indices; ///< �ndices de la malla para definir tri�ngulos.
};


/**
 * @file Mesh.cpp
 * @brief Implementación de la clase Mesh.
 */

#include "pch.h"
#include "Mesh.h"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
    : vertices(vertices), indices(indices)
{
}

const std::vector<Vertex>& Mesh::GetVertices() const
{
    return vertices;
}

const std::vector<unsigned int>& Mesh::GetIndices() const
{
    return indices;
}

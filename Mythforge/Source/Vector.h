/*!
 * @file Vector.h
 * @brief Definiciones de los vectores 2D, 3D y 4D para operaciones matemáticas en gráficos.
 */

#pragma once
#include <cmath> // Para std::fabs

constexpr float EPSILON = 1e-5f; ///< Tolerancia para comparaciones de punto flotante

/**
 * @struct Vector2
 * @brief Representa un vector en 2 dimensiones.
 */
struct Vector2 {
    float x; ///< Componente X del vector.
    float y; ///< Componente Y del vector.

    Vector2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
    Vector2(const Vector2& other) : x(other.x), y(other.y) {}
    explicit Vector2(const struct Vector3& v3);
    explicit Vector2(const struct Vector4& v4);

    /**
     * @brief Compara dos vectores 2D para verificar si son iguales con tolerancia EPSILON.
     * @param other El otro Vector2 para comparar.
     * @return true si ambos vectores son aproximadamente iguales, de lo contrario false.
     */
    bool operator==(const Vector2& other) const {
        return std::fabs(x - other.x) < EPSILON && std::fabs(y - other.y) < EPSILON;
    }

    bool operator!=(const Vector2& other) const { return !(*this == other); }
};

/**
 * @struct Vector3
 * @brief Representa un vector en 3 dimensiones.
 */
struct Vector3 {
    float x; ///< Componente X del vector.
    float y; ///< Componente Y del vector.
    float z; ///< Componente Z del vector.

    Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}
    Vector3(const Vector3& other) : x(other.x), y(other.y), z(other.z) {}
    explicit Vector3(const Vector2& v2, float z = 0.0f);
    explicit Vector3(const struct Vector4& v4);

    /**
     * @brief Compara dos vectores 3D para verificar si son iguales con tolerancia EPSILON.
     * @param other El otro Vector3 para comparar.
     * @return true si ambos vectores son aproximadamente iguales, de lo contrario false.
     */
    bool operator==(const Vector3& other) const {
        return std::fabs(x - other.x) < EPSILON &&
            std::fabs(y - other.y) < EPSILON &&
            std::fabs(z - other.z) < EPSILON;
    }

    bool operator!=(const Vector3& other) const { return !(*this == other); }
};

/**
 * @struct Vector4
 * @brief Representa un vector en 4 dimensiones.
 */
struct Vector4 {
    float x; ///< Componente X del vector.
    float y; ///< Componente Y del vector.
    float z; ///< Componente Z del vector.
    float w; ///< Componente W del vector.

    Vector4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f) : x(x), y(y), z(z), w(w) {}
    Vector4(const Vector4& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}
    explicit Vector4(const Vector2& v2, float z = 0.0f, float w = 1.0f);
    explicit Vector4(const Vector3& v3, float w = 1.0f);

    /**
     * @brief Compara dos vectores 4D para verificar si son iguales con tolerancia EPSILON.
     * @param other El otro Vector4 para comparar.
     * @return true si ambos vectores son aproximadamente iguales, de lo contrario false.
     */
    bool operator==(const Vector4& other) const {
        return std::fabs(x - other.x) < EPSILON &&
            std::fabs(y - other.y) < EPSILON &&
            std::fabs(z - other.z) < EPSILON &&
            std::fabs(w - other.w) < EPSILON;
    }

    bool operator!=(const Vector4& other) const { return !(*this == other); }
};

// Definiciones de los constructores de conversión en línea

inline Vector2::Vector2(const Vector3& v3) : x(v3.x), y(v3.y) {}
inline Vector2::Vector2(const Vector4& v4) : x(v4.x), y(v4.y) {}
inline Vector3::Vector3(const Vector2& v2, float z) : x(v2.x), y(v2.y), z(z) {}
inline Vector3::Vector3(const Vector4& v4) : x(v4.x), y(v4.y), z(v4.z) {}
inline Vector4::Vector4(const Vector2& v2, float z, float w) : x(v2.x), y(v2.y), z(z), w(w) {}
inline Vector4::Vector4(const Vector3& v3, float w) : x(v3.x), y(v3.y), z(v3.z), w(w) {}
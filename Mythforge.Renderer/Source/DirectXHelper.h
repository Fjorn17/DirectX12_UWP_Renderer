#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <d3d12.h>
#include <wrl.h>
#include <ppltasks.h> // Para create_task

namespace DX
{
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            // Configura un punto de interrupción en esta línea para detectar errores de la API Win32.
            throw std::runtime_error("Error de API de DirectX");
        }
    }

    // Función que lee desde un archivo binario de forma sincrónica (para aplicaciones de escritorio)
    inline std::vector<byte> ReadData(const std::wstring& filename)
    {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file)
        {
            throw std::runtime_error("No se pudo abrir el archivo.");
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<byte> buffer(size);
        if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
        {
            throw std::runtime_error("No se pudo leer el archivo.");
        }

        return buffer;
    }

    // Convierte una longitud expresada en píxeles independientes del dispositivo (PID) en una longitud expresada en píxeles físicos.
    inline float ConvertDipsToPixels(float dips, float dpi)
    {
        static const float dipsPerInch = 96.0f;
        return floorf(dips * dpi / dipsPerInch + 0.5f); // Redondear al entero más próximo.
    }

    // Asignar un nombre al objeto para facilitar la depuración.
#if defined(_DEBUG)
    inline void SetName(ID3D12Object* pObject, LPCWSTR name)
    {
        pObject->SetName(name);
    }
#else
    inline void SetName(ID3D12Object*, LPCWSTR)
    {
    }
#endif
}

// Nombrar función del asistente para ComPtr<T>.
// Asigna el nombre de la variable como nombre del objeto.
#define NAME_D3D12_OBJECT(x) DX::SetName(x.Get(), L#x)

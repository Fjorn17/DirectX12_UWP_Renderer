// framework.h
#pragma once

#define WIN32_LEAN_AND_MEAN             // Excluir material rara vez utilizado de encabezados de Windows

#ifdef MYTHFORGERENDERER_EXPORTS
#define MYTHFORGE_API __declspec(dllexport)
#else
#define MYTHFORGE_API __declspec(dllimport)
#endif

// Otros encabezados comunes
#include <windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>


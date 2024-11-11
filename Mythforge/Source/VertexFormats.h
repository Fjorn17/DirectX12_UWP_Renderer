#pragma once
#include <DirectXMath.h>

using namespace DirectX;

struct VertexPosColor
{
	XMFLOAT3 Position;
	XMFLOAT3 Color;
};

struct VertexPosTexCoord
{
	XMFLOAT3 Position;
	XMFLOAT2 TextCoord;
};
cbuffer ColorBuffer : register(b1)
{
    float4 objectColor;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

float4 PSMain(PixelInputType input) : SV_Target
{
    // Aplicar un color uniforme al objeto
    return objectColor;
}
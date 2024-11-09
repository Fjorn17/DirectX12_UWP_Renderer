cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VertexInputType
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

PixelInputType VSMain(VertexInputType input)
{
    PixelInputType output;
    
    // Transformar la posición del vértice al espacio de la pantalla
    float4 worldPosition = mul(float4(input.position, 1.0f), worldMatrix);
    float4 viewPosition = mul(worldPosition, viewMatrix);
    output.position = mul(viewPosition, projectionMatrix);
    
    // Pasar la normal y las coordenadas UV al pixel shader
    output.normal = input.normal;
    output.uv = input.uv;

    return output;
}

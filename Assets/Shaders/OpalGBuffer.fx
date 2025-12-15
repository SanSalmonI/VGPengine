cbuffer TransformBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix wvp;
}

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 texCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float3 normal : TEXCOORD0; // World Space Normal
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = mul(float4(input.position, 1.0f), wvp);
    
    // Transform Normal to World then View
    float3 worldNormal = mul(input.normal, (float3x3)world);
    output.normal = normalize(worldNormal); // Output World Normal
    
    return output;
}

float4 PS_Mask(VS_OUTPUT input) : SV_Target
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}

float4 PS_Normal(VS_OUTPUT input) : SV_Target
{
    float3 n = normalize(input.normal);
    // Encode [-1, 1] to [0, 1]
    return float4(n * 0.5f + 0.5f, 1.0f); // Alpha = Thickness (Default 1.0)
}

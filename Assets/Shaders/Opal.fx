// Opal.fx

cbuffer TransformBuffer : register(b0)
{
	matrix World;
	matrix WVP;
	float3 ViewPosition;
    float Time;
}

cbuffer LightBuffer : register(b1)
{
	float3 Direction;
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
}

cbuffer MaterialBuffer : register(b2)
{
	float4 MaterialEmissive;
	float4 MaterialAmbient;
	float4 MaterialDiffuse;
	float4 MaterialSpecular;
	float MaterialPower;
    float3 Scaling; // Padding
}

cbuffer SettingsBuffer : register(b3)
{
    float OpalPower;
    float IridescenceIntensity;
    float FlickerSpeed;
}

Texture2D DiffuseMap : register(t0);
Texture2D SpecularMap : register(t1);
Texture2D NormalMap : register(t2);

SamplerState Sampler : register(s0);

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
	float3 normal : NORMAL;
    float3 worldPosition : POSITION;
	float2 texCoord : TEXCOORD;
    float3 viewDir : TEXCOORD1;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;
	output.position = mul(float4(input.position, 1.0f), WVP);
	output.worldPosition = mul(float4(input.position, 1.0f), World).xyz;
	output.normal = mul(input.normal, (float3x3)World);
	output.texCoord = input.texCoord;
    output.viewDir = normalize(ViewPosition - output.worldPosition);
	return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float3 viewDir = normalize(input.viewDir);
    float3 normal = normalize(input.normal);

    // Fresnel effect
    float fresnel = pow(1.0 - max(0.0, dot(normal, viewDir)), 3.0);
    
    // Iridescence based on normal and view angle
    float3 iridescence = float3(0,0,0);
    float t = Time * FlickerSpeed;
    
    // Simple spectral shift approximation
    float3 shift = float3(
        sin(dot(normal, viewDir) * 10.0 + t + 0.0),
        sin(dot(normal, viewDir) * 10.0 + t + 2.0),
        sin(dot(normal, viewDir) * 10.0 + t + 4.0)
    );
    
    iridescence = shift * 0.5 + 0.5; // Map to 0-1

    float4 diffuseColor = DiffuseMap.Sample(Sampler, input.texCoord);
    
    // Combine
    float3 finalColor = diffuseColor.rgb * (Ambient.rgb + Diffuse.rgb * max(0, dot(normal, -Direction)));
    
    // Add Opal overlay
    finalColor += iridescence * fresnel * IridescenceIntensity * OpalPower;
    
    return float4(finalColor, diffuseColor.a);
}

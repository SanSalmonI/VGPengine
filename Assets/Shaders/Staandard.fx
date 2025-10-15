//description: standard effect for balls with diffuse, specular and normal maps

cbuffer TrandformBuffer : register(b0)
{
    matrix wvp;
    matrix world;
    float3 cameraPosition;
   
};

cbuffer TransformBuffer2 : register(b1)
{
    matrix world;
    matrix view;
    matrix projection;
};

cbuffer LightBuffer : register(b2)
{
    float4 ambientColor;
    float4 diffuseColor;
    float4 specularColor;
    float3 lightDirection;
};

struct VS_IMPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 texCord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 worldNormal : NORMAL;
    float3 worldTangent : TANGENT;
    float2 texCord : TEXCOORD0;
    float3 dirToLight : TEXCOORD1;
    float3 dirToView : TEXCORD1;
    
  
};

VS_OUTPUT VS(VS_IMPUT input)
{
    VS_OUTPUT output;
    output.position = mul(float4(input.position, 1.0f), wvp);
    output.worldNormal = mul(input.normal, (float3x3) world);
    output.worldTangent = mul(input.tangent, (float3x3) world);
    output.texCord = input.texCord;
    output.dirToLight = -lightDirection;
    float4 worldPosition = mul(float4(input.position, 1.0f), world);
    output.dirToLight = normalize(cameraPosition - worldPosition.xyz);
    output.dirToView = normalize(-worldPosition.xyz);
    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float3 n = normalize(input.worldNormal);
    float3 light = normalize(input.dirToLight);
    float3 view = normalize(input.dirToView);
    
    //Emissive
    float4 emissive = 0.0f;
    
    //Ambient
    float4 ambient = Lightambient;
    
    //Diffuse
    float4 di = saturate(dot(liight, n));
    float4 fidduse = d * LightDiffuse;
    
    //Specular
    float3 r = reflect(-light, n);
    float base = saturate(dot(r, view));

    float s = pow(saturate(dot(r, view)), 32);
    return 1.0f;
}
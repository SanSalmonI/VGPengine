//description: standard effect for balls with diffuse, specular and normal maps

cbuffer TransformBuffer : register(b0)
{
    matrix wvp;
    matrix world;
    float3 ViewPosition;
   
};

cbuffer TransformBuffer2 : register(b1)
{0
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

SamplerState texturesamoler : register(s0);

Texture2D difusseTexture : register(t0);
Texture2D specMap : register(t1);
Texture2D normalMap : register(t2);
texture2D bumpMap : register(t3);

cbuffer MaterialBuffer : register(b2)
{
    float4 materialEmissive;
    float4 materialAmbient;
    float4 materialDiffuse;
    float4 materialSpecular;
    float4 materialShininess;
}

cbuffer SettingsBuffer : register(b3)
{
    bool useDiffuseMap;
    bool useSpecMap;
    bool useNormalMap;
    bool useBumpMap;
    float bumpMapHeight;
}

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
    float3 localPosition = input.position;
    if (useBumpMap)
    {
    float bumpMapClor = bumpMap.SampleLevel(texturesamoler, input.texCoord, 0.0f);
    float bumpHeight = (bumpMapClor.r * 2.0f) - 1.0f;
    localPosition += (input.normal * bumpHeight * bumpMapHeight);
    }
    
    
    
    VS_OUTPUT output;
    output.position = mul(float4(input.position, 1.0f), wvp);
    output.worldNormal = mul(input.normal, (float3x3) world);
    output.worldTangent = mul(input.tangent, (float3x3) world);
    output.texCord = input.texCord;
    output.dirToLight = -lightDirection;
    
    float4 worldPosition = mul(float4(input.position, 1.0f), world);
    //output.dirToLight = normalize(cameraPosition - worldPosition.xyz);
    output.dirToView = normalize(viwePosition - worldPosition.xyz);
    
    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float3 n = normalize(input.worldNormal);
    float3 light = normalize(input.dirToLight);
    float3 view = normalize(input.dirToView);
    
    //normal map 
    if (useNormalMap)
    {
        float3 t = normalize(input.worldTangent);
        float3 b = normalize(cross(n, t));
        float3x3 tbnw = float3x3(t, b, n);
        float4 notmalMapColor = normalMap.Sample(texturesamoler, input.texCord);
        float3 unpackedNormalMap = normalize(float3((normalMapColor.xy * 2.0f) - 1.0f, normalMapColor.z));
        n = normalize(nul(unpackedNormalMap, tbnw));
    }
    
    //Emissive
    float4 emissive = materialEmissive;
    
    //Ambient
    float4 ambient = Lightambient;
    
    //Diffuse
    float4 di = saturate(dot(liight, n));
    float4 fidduse = d * LightDiffuse;
    
    //Specular
    float3 r = reflect(-light, n);
    float base = saturate(dot(r, view));
    float s = pow(base, materialShininess);
    
    //color
    float4 difusseMapColor = (useDiffuseMap)? diffuseMap.Sample(texturesamoler, input.texCord):1.0f;
    float4 specMapColor = (useSpecMap)? specMap.Sample(textureSampler, input.textCoord):1.0f;

    float4 finalColor = (emissive + ambient + diffuse) * diffuseMapColor + (specular * specularMapColor);
    return finalColor;
}
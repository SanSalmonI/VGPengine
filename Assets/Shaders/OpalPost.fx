cbuffer OpalParams : register(b0)
{
    float opalStrength; 
    float iridescenceScale; 
    float iridescencePower; 
    float scratchScale; 
    float rotationStrength; 
    // float inclusionContrast; 
    // float sparkleStrength;
    // float sparkleThreshold;
    float scratchIntensity;
    // float depthAttenuation;
    int debugMode;
    float voronoiScale;
    float3 baseColor;
    float milkyPower;
    float3 fireTint;
    float milkyIntensity;
    float refractionStrength;
    float voronoiDisplacement;
    float voronoiBorder;
    float shapeDistortion;
    float noiseScale;
    float featureScale;
    float facetStrength;
    float flashSharpness;
    float featureScaleMin;
    float3 padding2;
    matrix viewMatrix; // Transpose(View)
};

Texture2D SceneColor : register(t0);
Texture2D OpalMask : register(t1);
Texture2D OpalNormalThickness : register(t2);
Texture2D NoiseTexture : register(t3); 
Texture2D FireTexture : register(t4); 
Texture2D SparkleTexture : register(t5);
Texture2D Scratches : register(t6);


SamplerState TextureSampler : register(s0);

struct VS_INPUT
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = float4(input.position, 1.0f);
    output.texCoord = input.texCoord;
    return output;
}


// Utility Functions

float2 hash2(float2 p)
{
    p = float2(dot(p, float2(127.1, 311.7)), dot(p, float2(269.5, 183.3)));
    return frac(sin(p) * 43758.5453);
}

float3 hash3(float2 p)
{
    float3 q = float3( dot(p,float2(127.1,311.7)), 
                       dot(p,float2(269.5,183.3)), 
                       dot(p,float2(419.2,371.9)) );
    return frac(sin(q)*43758.5453);
}

float2 Rotate2D(float2 v, float a)
{
    float s = sin(a), c = cos(a);
    return float2(c*v.x - s*v.y, s*v.x + c*v.y);
}

struct VoronoiResult
{
    float2 id;       // Cell ID (Seed for rotation)
    float2 centerUV; // UV Relative to Cell Center (For internal structure)
    float dist;      // Distance to center
    float edgeDist;  // Distance to nearest edge
};

VoronoiResult Voronoi(float2 value, float jitter, float sizeVarMax, float sizeVarMin)
{
    float2 baseCell = floor(value);
    float2 f = frac(value); // 'value' relative to tile, used if needed, but tutorial uses vectors relative to input 'value'

    // Pass 1: Find Closest Cell (Weighted)
    float minDistToCell = 800.0; // High init
    float2 closestCell = float2(0,0); // Grid coordinate
    float2 toClosestCell = float2(0,0); // Vector from 'value' to cell center
    float weightClosest = 0.0;

    for(int x1=-1; x1<=1; x1++)
    {
        for(int y1=-1; y1<=1; y1++)
        {
            float2 cell = baseCell + float2(x1, y1);
            float2 cellRand = hash2(cell);
            
            // Calculate Weight (Organic Size)
            float randomVal = hash2(cell + float2(43.0, 17.0)).x;
            float weight = lerp(sizeVarMin, sizeVarMax, randomVal);
            
            // Animated offset
            float2 offset = (sin(mad(cellRand, 6.2831, 1.0)) * 0.5 + 0.5) * jitter;
            float2 cellPosition = cell + offset;
            
            float2 toCell = cellPosition - value; // Vector from pixel to center
            
            // Metric: Distance^2 - Weight (Power Diagram)
            float distToCellSq = dot(toCell, toCell);
            float weightedDist = distToCellSq - weight;
            
            if(weightedDist < minDistToCell)
            {
                minDistToCell = weightedDist;
                closestCell = cell;
                toClosestCell = toCell;
                weightClosest = weight;
            }
        }
        // https://www.ronja-tutorials.com/post/028-voronoi-noise/
    }

    // Pass 2: Distance to Edge
    float minEdgeDistance = 800.0; // High init
    
    for(int x2=-1; x2<=1; x2++)
    {
        for(int y2=-1; y2<=1; y2++)
        {
            float2 cell = baseCell + float2(x2, y2);
            float2 cellRand = hash2(cell);
            float randomVal = hash2(cell + float2(43.0, 17.0)).x;
            float weight = lerp(sizeVarMin, sizeVarMax, randomVal);
            
            float2 offset = (sin(mad(cellRand, 6.2831, 1.0)) * 0.5 + 0.5) * jitter;
            float2 cellPosition = cell + offset;
            float2 toCell = cellPosition - value;
            
            // Skip if this IS the closest cell
            float2 diffToClosestCell = abs(closestCell - cell);
            bool isClosestCell = diffToClosestCell.x + diffToClosestCell.y < 0.1;

            if(!isClosestCell)
            {
                // Calculate edge distance using weighted bisector logic
                
                float2 diffVec = toCell - toClosestCell; // Vector from Closest -> Neighbor
                float len = length(diffVec);
                float2 cellDifference = diffVec / len; // Normalize
                
                float2 toCenter = (toClosestCell + toCell) * 0.5;
                float bisectorDist = dot(toCenter, cellDifference); // Pure geometric bisector
                
                // Add weight bias
                float weightDist = (weightClosest - weight) / (2.0 * len);
                
                float edgeDistance = bisectorDist + weightDist;
                
                minEdgeDistance = min(minEdgeDistance, edgeDistance);
            }
        }
    }
    
    VoronoiResult res;
    res.dist = length(toClosestCell);
    
    res.edgeDist = minEdgeDistance;
    res.centerUV = toClosestCell;
    res.id = closestCell;
    return res;
}

float3 RainbowPhase(float phase)
{
    // Cosine based palette (Opal-like)
    return 0.5f + 0.5f * cos(6.28318f * (phase + float3(0.0f, 0.33f, 0.67f)));
}

float3 DecodeNormal(float3 encoded)
{
    return normalize(encoded * 2.0f - 1.0f);
}

float2 GetWorldNormalUV(float3 N_world)
{
    float u = atan2(N_world.z, N_world.x) / 6.283185f + 0.5f;
    float v = asin(clamp(N_world.y, -1.0f, 1.0f)) / 3.141592f + 0.5f;
    return float2(u, v);
}

// Opal Material Shader

float4 PS_OpalFire(VS_OUTPUT input) : SV_Target
{
    float mask = OpalMask.Sample(TextureSampler, input.texCoord).r;
    if (mask <= 0.001f) return 0;

    // Geometry & UVs
    float4 nt = OpalNormalThickness.Sample(TextureSampler, input.texCoord);
    float3 N_world = DecodeNormal(nt.rgb); 
    float2 modelUV = GetWorldNormalUV(N_world);
    
    // Debug 2: Normals
    if (debugMode == 2) return float4(N_world * 0.5f + 0.5f, 1.0f);

    // Voronoi Micro-Domains
    // Warp UVs with noise for natural shape variation
    float2 noiseDistort = NoiseTexture.Sample(TextureSampler, modelUV * noiseScale).rg;
    float2 distortedUV = modelUV + (noiseDistort - 0.5f) * shapeDistortion;
    
    VoronoiResult voro = Voronoi(distortedUV * (voronoiScale * 2.0f), voronoiDisplacement, featureScale, featureScaleMin); 
    
    // Debug 3: Voronoi ID
    if (debugMode == 3) return float4(hash2(voro.id).xxx, 1.0f);
    
    // Mode A: Cell ID -> Random Rotation
    float randomAngle = hash2(voro.id).x * 6.28318f * rotationStrength;

    // Faceted Normals
    // Perturb normal based on cell ID for crystal facet look
    // Use random unit sphere vector for 360-degree scatter
    float3 rnd = hash3(voro.id);
    float3 randDir = normalize(rnd * 2.0f - 1.0f); // Random Unit Sphere
    
    // Blend from Surface Normal to Random Normal
    float3 N_facet = normalize(lerp(N_world, randDir, facetStrength));
    float3 N_use = N_facet;

    // Diffraction Grating Setup
    float3 Up = float3(0, 1, 0);
    float3 Tangent = abs(N_use.y) > 0.99f ? float3(1, 0, 0) : normalize(cross(N_use, Up));
    float3 Bitangent = normalize(cross(N_use, Tangent));

    // Rotate Grating Direction in Tangent Plane
    float s = sin(randomAngle);
    float c = cos(randomAngle);
    float3 GratingDir_World = normalize(Tangent * c + Bitangent * s);

    // View Dependence
    float3 N_view = normalize(mul(N_use, (float3x3)viewMatrix));
    float3 T_view = normalize(mul(GratingDir_World, (float3x3)viewMatrix));
    
    // View Vector (Camera looks down -Z in View Space)
    float3 viewDirection = float3(0.0f, 0.0f, -1.0f); 

    // Phase & Flashing
    // Anisotropic Phase: dot(Tangent, View)
    float gratingPhase = dot(T_view, viewDirection); 
    
    // Flash intensity based on facet angle relative to view
    float facetFlash = pow(saturate(dot(N_view, viewDirection)), flashSharpness);
    
    // Combine terms
    float scratch = Scratches.Sample(TextureSampler, voro.centerUV * 2.0f).r;
    float phase = gratingPhase * iridescenceScale * 0.5f;
    phase += (scratch - 0.5f) * scratchIntensity;
    phase *= iridescencePower;

    // Color
    float3 color = RainbowPhase(phase);
    color *= fireTint;
    
    // Apply Flash/Facet intensity
    color *= lerp(1.0f, facetFlash, saturate(facetStrength * 2.0f));

    
    // Mask gaps between cells
    float cellMask = smoothstep(voronoiBorder, voronoiBorder + 0.05f, voro.edgeDist);
    color *= cellMask;

    color *= opalStrength;

    // Debug: Voronoi ID check
    // if (debugMode == 4) return float4(hash2(voro.id).xxx, 1.0f);

    return float4(color, 1.0f);
}

// Pass B: Sparkles (Disabled)

float4 PS_SparkleGlints(VS_OUTPUT input) : SV_Target
{
    return float4(0,0,0,0);
}

// Pass C: Composite

float4 PS_Composite(VS_OUTPUT input) : SV_Target
{
    float4 mainColor = SceneColor.Sample(TextureSampler, input.texCoord);
    float mask = OpalMask.Sample(TextureSampler, input.texCoord).r;
    float4 fire = FireTexture.Sample(TextureSampler, input.texCoord);
    
    // Debug: Global (Pre-Mask) or Post-Mask
    if (debugMode != 0)
    {
        if (debugMode == 1) return float4(fire.rgb, 1.0f); // Fire
        if (debugMode == 2) return float4(fire.rgb, 1.0f); // Normals (via Fire RT)
        if (debugMode == 3) return float4(fire.rgb, 1.0f); // Voronoi (via Fire RT)
    }
    
    if (mask <= 0.001f) return mainColor;

    // Calculate Normal and View for Milky Effect
    float4 nt = OpalNormalThickness.Sample(TextureSampler, input.texCoord);
    float3 N_world = DecodeNormal(nt.rgb);
    float3 N_view = normalize(mul(N_world, (float3x3)viewMatrix));
    float3 viewDirection = float3(0.0f, 0.0f, -1.0f);
    
    // "Facing Ratio": How much the surface points at the camera.
    // 1.0 = Center of sphere (Thickest part looking through)
    // 0.0 = Edge of sphere (Thinnest part)
    float facingRatio = saturate(dot(N_view, viewDirection));

    // Refraction / Distortion
    // 1. Macro: Orb shape refraction
    float2 refractOffset = N_view.xy * refractionStrength;
    
    // 2. Micro: Surface scratches
    float scratchVal = Scratches.Sample(TextureSampler, input.texCoord * scratchScale).r;
    refractOffset += (scratchVal - 0.5f) * (refractionStrength * 0.5f);
    
    // Sample Background with distortion
    float2 distortedUV = input.texCoord + (refractOffset * mask);
    float4 bgDistorted = SceneColor.Sample(TextureSampler, distortedUV);

    // Milky Body Effect
    // We use the facing ratio as a proxy for depth/thickness.
    float milk = pow(facingRatio, milkyPower) * milkyIntensity;

    if (debugMode == 4) return float4(milk.xxx, 1.0f); // Debug Milky
    if (debugMode == 5) return float4(refractOffset.xy * 5.0f + 0.5f, 0.0f, 1.0f); // Debug Refraction
    
    float3 bodyColor = lerp(bgDistorted.rgb, baseColor, milk * mask);
    float3 finalRGB = bodyColor + fire.rgb;
    
    return float4(lerp(mainColor.rgb, finalRGB, mask), 1.0f);
}

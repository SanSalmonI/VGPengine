#pragma once

#include <IExeEngine/Inc/IExeEngine.h>

class GameState : public IExeEngine::AppState
{
public:
	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void Render() override;
	void DebugUI() override;

private:
	void UpdateCamera(float deltaTime);

	IExeEngine::Graphics::Camera mCamera;
    IExeEngine::Graphics::DirectionalLight mDirectionalLight;

    // Objects
    IExeEngine::Graphics::RenderObject mOpalObject; // Sphere
    IExeEngine::Graphics::RenderGroup mCharacter; // Character
    IExeEngine::Graphics::RenderObject mGround;
    IExeEngine::Graphics::RenderObject mScreenQuad;

    // Effects
    IExeEngine::Graphics::StandardEffect mStandardEffect;
    // IExeEngine::Graphics::ShadowEffect mShadowEffect; // Keeping simpler for now

    // Render Targets
    IExeEngine::Graphics::RenderTarget mSceneRT;
    IExeEngine::Graphics::RenderTarget mOpalMaskRT;
    IExeEngine::Graphics::RenderTarget mOpalNormalRT;
    IExeEngine::Graphics::RenderTarget mFireRT;
    IExeEngine::Graphics::RenderTarget mSparkleRT;

    // Opal Shaders
    IExeEngine::Graphics::VertexShader mScreenQuadVS; // For full screen passes
    IExeEngine::Graphics::PixelShader mOpalFirePS;
    IExeEngine::Graphics::PixelShader mOpalSparklePS;
    IExeEngine::Graphics::PixelShader mOpalCompositePS;

    IExeEngine::Graphics::VertexShader mOpalMeshVS; // For GBuffer passes
    IExeEngine::Graphics::PixelShader mOpalMaskPS;
    IExeEngine::Graphics::PixelShader mOpalNormalPS;

    // Resources
    IExeEngine::Graphics::Texture mNoiseTexture;
    IExeEngine::Graphics::Sampler mSampler; // Linear Wrap
    IExeEngine::Graphics::Texture mScratchTexture;

    // Constants
    struct OpalParams
    {
        float opalStrength = 2.0f;
        float iridescenceScale = 5.0f;
        float iridescencePower = 1.0f;
        float scratchScale = 20.0f;
        float rotationStrength = 1.0f; 
        // float inclusionScaleHigh = 40.0f; 
        // float inclusionContrast = 1.0f; 
        // float sparkleStrength = 1.0f; 
        // float sparkleThreshold = 0.9f; 
        float scratchIntensity = 0.0f;
        // float depthAttenuation = 1.0f; 
        int debugMode = 0;
        float voronoiScale = 5.0f; 
        IExeEngine::Math::Vector3 baseColor = { 0.39f, 0.78f, 1.0f }; 
        float milkyPower = 0.1f;
        IExeEngine::Math::Vector3 fireTint = { 0.99f, 1.0f, 0.8f }; 
        float milkyIntensity = 1.0f;
        float refractionStrength = 0.1f;
        float voronoiDisplacement = 0.97f; // 0=Grid, 1=Irregular
        float voronoiBorder = 0.0f; // 0=Full, 1=Small Dots
        float shapeDistortion = 0.23f;
        float noiseScale = 2.0f; // Low freq for broad variation
        float featureScale = 1.0f; // Size Variance
        float facetStrength = 0.85f; // 0=Smooth, 1=Faceted
        float flashSharpness = 5.0f; // 2=Broad, 64+=Sharp Sparkle
        float featureScaleMin = 0.0f;
        float padding[3];
        IExeEngine::Math::Matrix4 viewMatrix;
    };
    using OpalConstantBuffer = IExeEngine::Graphics::TypedConstantBuffer<OpalParams>;
    OpalConstantBuffer mOpalParamsBuffer;
    OpalParams mOpalParams;

    struct TransformData
    {
        IExeEngine::Math::Matrix4 world;
        IExeEngine::Math::Matrix4 view; // Added View
        IExeEngine::Math::Matrix4 wvp;
    };
    using TransformBuffer = IExeEngine::Graphics::TypedConstantBuffer<TransformData>;
    TransformBuffer mTransformBuffer;
};
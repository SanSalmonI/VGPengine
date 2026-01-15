#pragma once

#include "ConstantBuffer.h"
#include "DirectionalLight.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "Material.h"
#include "Sampler.h"

namespace IExeEngine::Graphics
{
    class Camera;
    class RenderObject;
    class Texture;

    class OpalEffect
    {
    public:
        void Initialize(const std::filesystem::path& filepath);
        void Terminate();

        void Begin();
        void End();

        void Render(const RenderObject& renderObject);

        void SetCamera(const Camera& camera);
        void SetDirectionalLight(const DirectionalLight& directionalLight);
        void SetTime(float time);

        void DebugUI();

    private:
        struct TransformData
        {
            Math::Matrix4 world;
            Math::Matrix4 wvp;
            Math::Vector3 viewPosition;
            float time;
        };

        struct SettingsData
        {
            float opalPower = 1.0f;
            float iridescenceIntensity = 1.0f;
            float flickerSpeed = 5.0f;
            float padding;
        };

        using TransformBuffer = TypedConstantBuffer<TransformData>;
        using LightBuffer = TypedConstantBuffer<DirectionalLight>;
        using MaterialBuffer = TypedConstantBuffer<Material>;
        using SettingsBuffer = TypedConstantBuffer<SettingsData>;

        TransformBuffer mTransformBuffer;
        LightBuffer mLightBuffer;
        MaterialBuffer mMaterialBuffer;
        SettingsBuffer mSettingsBuffer;

        VertexShader mVertexShader;
        PixelShader mPixelShader;
        Sampler mSampler;

        const Camera* mCamera = nullptr;
        const DirectionalLight* mDirectionalLight = nullptr;
        float mTime = 0.0f;
        SettingsData mSettings;
    };
}

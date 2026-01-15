#include "Precompiled.h"
#include "OpalEffect.h"
#include "Camera.h"
#include "RenderObject.h"
#include "VertexTypes.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;

void OpalEffect::Initialize(const std::filesystem::path& filepath)
{
    mTransformBuffer.Initialize();
    mLightBuffer.Initialize();
    mMaterialBuffer.Initialize();
    mSettingsBuffer.Initialize();
    mVertexShader.Initialize(filepath, VertexPX::Format);
    mPixelShader.Initialize(filepath);
    mSampler.Initialize(Sampler::Filter::Linear, Sampler::AddressMode::Wrap);
}

void OpalEffect::Terminate()
{
    mSampler.Terminate();
    mPixelShader.Terminate();
    mVertexShader.Terminate();
    mSettingsBuffer.Terminate();
    mMaterialBuffer.Terminate();
    mLightBuffer.Terminate();
    mTransformBuffer.Terminate();
}

void OpalEffect::Begin()
{
    mVertexShader.Bind();
    mPixelShader.Bind();
    mTransformBuffer.BindVS(0);
    mLightBuffer.BindVS(1);
    mLightBuffer.BindPS(1);
    mMaterialBuffer.BindPS(2);
    mSettingsBuffer.BindPS(3);
    mSampler.BindPS(0);
}

void OpalEffect::End()
{
}

void OpalEffect::Render(const RenderObject& renderObject)
{
    const auto& matWorld = renderObject.transform.GetMatrix4();
    const auto& matView = mCamera->GetViewMatrix();
    const auto& matProj = mCamera->GetProjectionMatrix();

    TransformData data;
    data.world = Transpose(matWorld);
    data.wvp = Transpose(matWorld * matView * matProj);
    data.viewPosition = mCamera->GetPosition();
    data.time = mTime;

    mTransformBuffer.Update(data);

    if (mDirectionalLight != nullptr)
    {
        mLightBuffer.Update(*mDirectionalLight);
    }

    mMaterialBuffer.Update(renderObject.material);
    mSettingsBuffer.Update(mSettings);

    TextureManager* tm = TextureManager::Get();
    tm->BindPS(renderObject.diffuseMapId, 0);
    tm->BindPS(renderObject.specMapId, 1);
    tm->BindPS(renderObject.normalMapId, 2);

    renderObject.meshBuffer.Render();
}

void OpalEffect::SetCamera(const Camera& camera)
{
    mCamera = &camera;
}

void OpalEffect::SetDirectionalLight(const DirectionalLight& directionalLight)
{
    mDirectionalLight = &directionalLight;
}

void OpalEffect::SetTime(float time)
{
    mTime = time;
}

void OpalEffect::DebugUI()
{
     if (ImGui::CollapsingHeader("Opal Effect", ImGuiTreeNodeFlags_DefaultOpen))
     {
         ImGui::DragFloat("Opal Power", &mSettings.opalPower, 0.1f, 0.0f, 10.0f);
         ImGui::DragFloat("Iridescence Intensity", &mSettings.iridescenceIntensity, 0.1f, 0.0f, 10.0f);
         ImGui::DragFloat("Flicker Speed", &mSettings.flickerSpeed, 0.1f, 0.0f, 50.0f);
     }
}

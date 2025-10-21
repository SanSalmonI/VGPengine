#include "Precompiled.h"
#include "StandardEffect.h"
#include "Camera.h"
#include "RenderObject.h"
#include "VertexTypes.h"

using namespace IExeEngine;

using namespace IExeEngine::Graphics;

void StandardEffect::Initialize(const std::filesystem::path& path)
{
	mTransformBuffer.Initialize();
	mVertexShader.Initialize<Vertex>(path);
	mPixelShader.Initialize(path);
	mLightBuffer.Initialize();

	mVertexShader.Initialize<Vertex>(path);
	mPixelShader.Initialize(path);
	mSampler.Initialize(Sampler::Filter::Linear, Sampler::AddressMode::Wrap);

}

void StandardEffect::Terminate()
{
	mSampler.Terminate();
	mPixelShader.Terminate();
	mVertexShader.Terminate();
	mSettingsBuffer.Terminate();

	mTransformBuffer.Terminate();

	mLightBuffer.Terminate();
}

void StandardEffect::Begin()
{
	mVertexShader.Bind();
	mPixelShader.Bind();
	mTransformBuffer.BindVS(0);
	m
	m
}

void StandardEffect::End() 
{

}

void StandardEffect::Render(const RenderObject& renderObject)
{
	const Math::Matrix4 matWorld = renderObject.transform.GetMatrix4();
	const Math::Matrix4 matView = mCamera->GetViewMatrix();
	const Math::Matrix4 matProj = mCamera->GetProjectionMatrix();
	const Math::Matrix4 matFinal = matWorld * matView * matProj;

	TransformData data;
	data.wvp = Math::Transpose(matFinal);
	data.world = Math::Transpose(matWorld);
	data.ViewPosition = mCamera->GetPosition();
	mTransformBuffer.Update(data);

	SettingsData settings;
	settings.useDiffuesMap = (renderObject.DiffuseMapId > 0 && mSettingsData.useDiffuesMap > 0) ? 1 : 0;
	settings.useSpecMap = (renderObject.specMapId > 0 && mSettingsData.useSpecMap > 0) ? 1 : 0;
	settings.useNormalMap = (renderOnject.normalMapId > 0 && mSettingsData.useNormalMap > 0) ? 1 : 0;
	settings.bumpWeight = mSettingsData.bumpWeight;
	mSettingsBuffer.Update(settings);

	TextureManager* tm = TextureManager::Get();
	tm->BindPs(renderObject.diffuseMapId, 0);
	tm->BindPs(renderObject.specMapId, 1);
	tm->BindPs(renderObject.normalMapId, 2);
	tm->BindPs(renderObject.
	tm->BindPs(renderObject.


	mLightBuffer.Update(*mDirectionalLight);
	mMaterialBuffer.Update()

	renderObject.meshBuffer.Render();
}

void StandardEffect::setCamera(const Camera& camera)
{
	mCamera = &camera;
}

void StandardEffect::SetDirectionalLight(const DirectionalLight& directionalLight)
{
	mDirectionalLight = &directionalLight;
}

void StandardEffect::DebugUI()
{
	if (ImGui::CollapsingHeader("StandardEffecr", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool useDiffuseMap = mSettingsData.useDiffuesMap > 0;
		if (ImGui::Checkbox("UseDiffuseMap", &useDiffuseMap))
		{
			mSettingsData.useDiffuesMap = (useDiffuseMap) ? 1 : 0;
		}

		bool useSpecMap = mSettingsData.useSpecMap > 0;
		if (ImGui::Checkbox("UseSpeceMap", &useSpecMap))
		{
			mSettingsData.useSpecMap = (useSpecMap) ? 1 : 0;
		}

		bool useNormalMap = mSettingsData.useNormalMap > 0;
		if (ImGui::Checkbox("UseNormalMap", &useNormalMap))
		{
			mSettingsData.useNormalMap = (useNormalMap) ? 1 : 0;
		}

		bool useBumpMap = mSettingsData.useBumpMap > 0;
		if (ImGui::Checkbox("UseBumpMap", &useBumpMap))
		{
			mSettingsData.useBumpMap = (useBumpMap) ? 1 : 0;
		}

		ImGui::DragFloat("BumpWeight", &mSettingsData.bumpWeight, 0.1f, 0.0f, 100.0f);
	}
}
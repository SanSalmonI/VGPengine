#pragma once
#include "ConstantBuffer.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "DirectionalLight.h"
#include "Sampler.h"

namespace IExeEngine::Graphics
{
	class Camera;
	class RenderObject;

	class StandardEffect
	{
	public:
		void Initialize(const std::filesystem::path& path);
		void Terminate();

		void Begin();
		void End();

		void Render(const RenderObject& renderObject);

		void setCamera(const Camera& camera);

		void SetDirectionalLight(const DirectionalLight& directionalLight);

		void DebugUI();

	private:

		struct TransformData
		{
			Math::Matrix4 wvp; // World View Projection
			Math::Matrix4 world; // World matrix
			Math::Vector3 ViewPosition; // Camera position in world space
			float padding = 0.0f; // Padding to make the structure 16 byte aligned
		};

		struct SettingsData
		{
			int useDiffuesMap = 1;
			int useSpecMap = 1;
			int useNormalMap = 1;
			int useBumpMap = 1;
			float bumpWeight = 0.1f;
			float padding[3] = { 0.0f };
		};

		using TransformBuffer = TypedConstantBuffer<TransformData>;
		TransformBuffer mTransformBuffer;

		using LightBuffer = TypedConstantBuffer<DirectionalLight>;
		LightBuffer mLightBuffer;


		VertexShader mVertexShader;
		PixelShader mPixelShader;
		Sampler mSampler;

		const Camera* mCamera = nullptr;
		const DirectionalLight* mDirectionalLight = nullptr;
		ConstantBuffer mConstantBuffer;
		SettingsData mSettingsData;

	};
}
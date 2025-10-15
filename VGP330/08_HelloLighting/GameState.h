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

	IExeEngine::Graphics::StandardEffect mStandardEffect;

	IExeEngine::Graphics::Transform mTransform;

	IExeEngine::Graphics::RenderObject mRenderObject;

	IExeEngine::Graphics::DirectionalLight mDirectionalLight;

};
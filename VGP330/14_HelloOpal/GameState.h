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

	IExeEngine::Graphics::RenderGroup mCharacter;
	IExeEngine::Graphics::RenderObject mGround;
    
	// We will replace this with OpalEffect later, for now just include header
    IExeEngine::Graphics::OpalEffect mOpalEffect; 
    // Implementing usage in next steps
};

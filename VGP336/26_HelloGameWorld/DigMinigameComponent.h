#pragma once

#include "CustomTypeIds.h"
#include <IExeEngine/Inc/IExeEngine.h>

class DigMinigameComponent : public IExeEngine::UIComponent
{
public:
	SET_TYPE_ID(CustomComponentId::DigMinigame);

	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void Render() override;
	void DebugUI() override;
	void Deserialize(const rapidjson::Value& value) override;

private:
	enum class Mode
	{
		Compass,
		Dig
	};

	Mode mMode = Mode::Compass;
	float mPrevMouseWheel = 0.0f;

	IExeEngine::Math::Vector3 mTargetPosition = { 5.0f, 0.0f, 5.0f };
	float mSuccessRadius = 2.0f;
	bool mTreasureFound = false;
	float mWaitTimer = 0.0f;

	IExeEngine::Graphics::UISprite mCompassBase;
	IExeEngine::Graphics::UISprite mCompassNeedle;

	// Particle systems for confetti explosion
	IExeEngine::Graphics::ParticleSystemEffect mParticleSystemEffect;
	IExeEngine::Physics::ParticleSystem mParticleSystem;
};

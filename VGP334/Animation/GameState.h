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

	void OnCollisionEvent(const IExeEngine::Core::Event& e);
	void OnFireworkEvent(const IExeEngine::Core::Event& e);

	// Camera and Lighting
	IExeEngine::Graphics::Camera mCamera;
	IExeEngine::Graphics::DirectionalLight mDirectionalLight;

	// Rendering
	IExeEngine::Graphics::StandardEffect mStandardEffect;

	IExeEngine::Graphics::RenderObject mFloor;

	// Esso Model
	IExeEngine::Graphics::RenderObject mEssoModel;
	IExeEngine::Graphics::Animation mEssoMovement;
	float mEssoMoveTime = 0.0f;

	// Character 1 - Animated character model
	IExeEngine::Graphics::RenderGroup mCharacter01;
	IExeEngine::Graphics::Animator mChar01Animator;
	IExeEngine::Graphics::Animation mChar01Movement;
	float mChar01MoveTime = 0.0f;

	// Character 2 - Animated character model
	IExeEngine::Graphics::RenderGroup mCharacter02;
	IExeEngine::Graphics::Animator mChar02Animator;
	IExeEngine::Graphics::Animation mChar02Movement;
	float mChar02MoveTime = 0.0f;

	// Character 3 - Animated character model
	IExeEngine::Graphics::RenderGroup mCharacter03;
	IExeEngine::Graphics::Animator mChar03Animator;
	IExeEngine::Graphics::Animation mChar03Movement;
	float mChar03MoveTime = 0.0f;

	// Particle Systems
	IExeEngine::Graphics::ParticleSystemEffect mParticleEffect;
	IExeEngine::Physics::ParticleSystem mChar01Bullets;
	IExeEngine::Physics::ParticleSystem mChar02Bullets;
	IExeEngine::Physics::ParticleSystem mExplosionParticles;
	IExeEngine::Physics::ParticleSystem mChar03Bullets;

	// Audio
	IExeEngine::Audio::SoundId mBgMusicId = 0;
	IExeEngine::Audio::SoundId mWarningSoundId = 0;
	IExeEngine::Audio::SoundId mExplosionSoundId = 0;
	IExeEngine::Audio::SoundId mFireworkSoundId = 0;
	IExeEngine::Audio::SoundId mCoughSoundId = 0;


	// Events
	IExeEngine::Core::EventListenerId mCollisionListenerId = 0;
	IExeEngine::Core::EventListenerId mFireworkListenerId = 0;

	// Scene Timer
	float mSceneTime = 0.0f;
	float mTotalSceneTime = 50.0f;
	bool mBgMusicStarted = false;
};
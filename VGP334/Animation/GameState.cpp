#include "GameState.h"
#include "GameEvents.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;
using namespace IExeEngine::Input;
using namespace IExeEngine::Physics;
using namespace IExeEngine::Audio;

namespace
{
	const float SCENE_DURATION = 45.0f;

	// Animation clip indices (order they are added via AddAnimation)
	// Same order for ALL three characters
	// 0 = Hip_Hop_Dancing
	// 1 = Dancing
	// 2 = Charge
	// 3 = Kneeling_Pointing
	// 4 = Dancing_Twerk
	// 5 = Stand_still
	// 6 = Sad_Idle           (Char01 only)
	// 6 = Slow_Jog_Backwards (Char02 and Char03 only)
	enum AnimClip
	{
		Hip_Hop_Dancing = 0,
		Dancing,
		Charge,
		Kneeling_Pointi,
		Dancing_Twerk,
		Stand_still,
		Sad_Idle,
		Slow_Jog_Backwards
	};
}

void GameState::Initialize()
{
	// Camera - positioned to see all three characters
	mCamera.SetPosition({ 0.0f, 3.0f, -12.0f });
	mCamera.SetLookAt({ 0.0f, 1.0f, 0.0f });

	// Lighting
	mDirectionalLight.direction = Math::Normalize({ 1.0f, -1.0f, 1.0f });
	mDirectionalLight.ambient = { 0.3f, 0.3f, 0.4f, 1.0f };
	mDirectionalLight.diffuse = { 0.9f, 0.9f, 0.8f, 1.0f };
	mDirectionalLight.specular = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Standard Effect
	std::filesystem::path shaderFile = L"../../Assets/Shaders/Standard.fx";
	mStandardEffect.Initialize(shaderFile);
	mStandardEffect.SetCamera(mCamera);
	mStandardEffect.SetDirectionalLight(mDirectionalLight);

	ModelManager* mm = ModelManager::Get();
	TextureManager* tm = TextureManager::Get();

	// ============================================================
	// ESSO SIGN - billboard plane with Esso.jpg texture
	// Visible only at 15-16s when characters point
	// ============================================================
	{
		Mesh essoPlane = MeshBuilder::CreatePlane(1, 1, 3.0f);
		mEssoModel.meshBuffer.Initialize(essoPlane);
		mEssoModel.diffuseMapId = tm->LoadTexture("Images/Esso.jpg");
		// Stand upright: rotate -90 on X so plane faces camera
		mEssoModel.transform.rotation = Math::Quaternion::CreateFromAxisAngle(Math::Vector3::XAxis, -Math::Constants::HalfPi);
	}

	mEssoMovement = AnimationBuilder()
		.AddPositionKey({ 0.0f, 2.0f, 2.0f }, 0.0f)
		.AddScaleKey(Math::Vector3(0.0f, 0.0f, 0.0f), 0.0f)
		.AddScaleKey(Math::Vector3(0.0f, 0.0f, 0.0f), 14.99f)
		.AddScaleKey(Math::Vector3(1.0f, 1.0f, 1.0f), 15.0f)
		.AddScaleKey(Math::Vector3(1.0f, 1.0f, 1.0f), 15.99f)
		.AddScaleKey(Math::Vector3(0.0f, 0.0f, 0.0f), 16.0f)
		.AddScaleKey(Math::Vector3(0.0f, 0.0f, 0.0f), 45.0f)
		.Build();

	// ============================================================
	// FLOOR - concrete textured plane
	// ============================================================
	{
		Mesh floorMesh = MeshBuilder::CreatePlane(20, 20, 1.0f);
		mFloor.meshBuffer.Initialize(floorMesh);
		mFloor.diffuseMapId = tm->LoadTexture(L"../../Assets/Textures/textures/misc/concrete.jpg");
	}

	// ============================================================
	// CHARACTER 01 - (Center)
	// ============================================================
	mCharacter01.Initialize("Character01/Character01.model");
	mCharacter01.transform.position = { 0.0f, 0.0f, 0.0f };
	mCharacter01.transform.rotation = Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, 0.0f);
	mCharacter01.animator = &mChar01Animator;

	mm->AddAnimation(mCharacter01.modelId, L"../../Assets/Models/Character01/Hip_Hop_Dancing.animset");     // 0
	mm->AddAnimation(mCharacter01.modelId, L"../../Assets/Models/Character01/Dancing.animset");             // 1
	mm->AddAnimation(mCharacter01.modelId, L"../../Assets/Models/Character01/Charge.animset");              // 2
	mm->AddAnimation(mCharacter01.modelId, L"../../Assets/Models/Character01/Kneeling_Pointing.animset");   // 3
	mm->AddAnimation(mCharacter01.modelId, L"../../Assets/Models/Character01/Dancing_Twerk.animset");       // 4
	mm->AddAnimation(mCharacter01.modelId, L"../../Assets/Models/Character01/Stand_still.animset");         // 5
	mm->AddAnimation(mCharacter01.modelId, L"../../Assets/Models/Character01/Sad_Idle.animset");            // 6
	mm->AddAnimation(mCharacter01.modelId, L"../../Assets/Models/Character01/Slow_Jog_Backwards.animset"); // 7
	mChar01Animator.Initialize(mCharacter01.modelId);
	mChar01Animator.PlayAnimation(AnimClip::Sad_Idle, true);

	// ============================================================
	// CHARACTER 02 - Right side (faces LEFT toward Char01)
	// ============================================================
	mCharacter02.Initialize("Character01/Character01.model");
	mCharacter02.transform.position = { 5.0f, 0.0f, 0.0f };
	mCharacter02.transform.rotation = Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, Math::Constants::HalfPi);
	mCharacter02.animator = &mChar02Animator;

	mm->AddAnimation(mCharacter02.modelId, L"../../Assets/Models/Character01/Hip_Hop_Dancing.animset");     // 0
	mm->AddAnimation(mCharacter02.modelId, L"../../Assets/Models/Character01/Dancing.animset");             // 1
	mm->AddAnimation(mCharacter02.modelId, L"../../Assets/Models/Character01/Charge.animset");              // 2
	mm->AddAnimation(mCharacter02.modelId, L"../../Assets/Models/Character01/Kneeling_Pointing.animset");   // 3
	mm->AddAnimation(mCharacter02.modelId, L"../../Assets/Models/Character01/Dancing_Twerk.animset");       // 4
	mm->AddAnimation(mCharacter02.modelId, L"../../Assets/Models/Character01/Stand_still.animset");         // 5
	mm->AddAnimation(mCharacter02.modelId, L"../../Assets/Models/Character01/Sad_Idle.animset");            // 6
	mm->AddAnimation(mCharacter02.modelId, L"../../Assets/Models/Character01/Slow_Jog_Backwards.animset"); // 7
	mChar02Animator.Initialize(mCharacter02.modelId);
	mChar02Animator.PlayAnimation(AnimClip::Slow_Jog_Backwards, true);

	// ============================================================
	// CHARACTER 03 - Left side (faces Right toward Char01)
	// ============================================================
	mCharacter03.Initialize("Character01/Character01.model");
	mCharacter03.transform.position = { -5.0f, 0.0f, 0.0f };
	mCharacter03.transform.rotation = Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, -Math::Constants::HalfPi);
	mCharacter03.animator = &mChar03Animator;

	mm->AddAnimation(mCharacter03.modelId, L"../../Assets/Models/Character01/Hip_Hop_Dancing.animset");     // 0
	mm->AddAnimation(mCharacter03.modelId, L"../../Assets/Models/Character01/Dancing.animset");             // 1
	mm->AddAnimation(mCharacter03.modelId, L"../../Assets/Models/Character01/Charge.animset");              // 2
	mm->AddAnimation(mCharacter03.modelId, L"../../Assets/Models/Character01/Kneeling_Pointing.animset");   // 3
	mm->AddAnimation(mCharacter03.modelId, L"../../Assets/Models/Character01/Dancing_Twerk.animset");       // 4
	mm->AddAnimation(mCharacter03.modelId, L"../../Assets/Models/Character01/Stand_still.animset");         // 5
	mm->AddAnimation(mCharacter03.modelId, L"../../Assets/Models/Character01/Sad_Idle.animset");            // 6
	mm->AddAnimation(mCharacter03.modelId, L"../../Assets/Models/Character01/Slow_Jog_Backwards.animset"); // 7
	mChar03Animator.Initialize(mCharacter03.modelId);
	mChar03Animator.PlayAnimation(AnimClip::Slow_Jog_Backwards, true);

	// ============================================================
	// PARTICLE EFFECTS
	// ============================================================
	mParticleEffect.Initialize();
	mParticleEffect.SetCamera(mCamera);

	// ---- CHAR01 particles: Sombreros shooting upward ----
	ParticleSystemInfo char01BulletInfo;
	char01BulletInfo.textureId = tm->LoadTexture("Images/Sombrero.png");
	char01BulletInfo.maxParticles = 300;
	char01BulletInfo.particlesPerEmit = { 1, 2 };
	char01BulletInfo.delay = 0.0f;
	char01BulletInfo.lifeTime = FLT_MAX;
	char01BulletInfo.timeBetweenEmit = { 0.4f, 0.8f };
	char01BulletInfo.spawnAngle = { -15.0f, 15.0f };
	char01BulletInfo.spawnSpeed = { 6.0f, 10.0f };
	char01BulletInfo.particleLifeTime = { 1.0f, 2.0f };
	char01BulletInfo.spawnDirection = Math::Vector3::YAxis;
	char01BulletInfo.spawnPosition = Math::Vector3::Zero;
	char01BulletInfo.startScale = { Math::Vector3::One * 1.2f, Math::Vector3::One * 1.5f };
	char01BulletInfo.endScale = { Math::Vector3::One * 0.5f, Math::Vector3::One * 0.8f };
	char01BulletInfo.startColour = { Colors::White, Colors::White };
	char01BulletInfo.endColour = { Colors::White, Colors::White };
	mChar01Bullets.Initialize(char01BulletInfo);

	// ---- CHAR02 particles: Trumpets thrown toward center (-X) ----
	ParticleSystemInfo char02BulletInfo;
	char02BulletInfo.textureId = tm->LoadTexture("Images/trumpet.png");
	char02BulletInfo.maxParticles = 300;
	char02BulletInfo.particlesPerEmit = { 1, 2 };
	char02BulletInfo.delay = 0.0f;
	char02BulletInfo.lifeTime = FLT_MAX;
	char02BulletInfo.timeBetweenEmit = { 0.4f, 0.8f };
	char02BulletInfo.spawnAngle = { -10.0f, 10.0f };
	char02BulletInfo.spawnSpeed = { 8.0f, 12.0f };
	char02BulletInfo.particleLifeTime = { 1.0f, 2.0f };
	char02BulletInfo.spawnDirection = { -1.0f, 0.0f, 0.0f };
	char02BulletInfo.spawnPosition = Math::Vector3::Zero;
	char02BulletInfo.startScale = { Math::Vector3::One * 1.2f, Math::Vector3::One * 1.5f };
	char02BulletInfo.endScale = { Math::Vector3::One * 0.8f, Math::Vector3::One * 1.0f };
	char02BulletInfo.startColour = { Colors::White, Colors::White };
	char02BulletInfo.endColour = { Colors::White, Colors::White };
	mChar02Bullets.Initialize(char02BulletInfo);

	// ---- CHAR03 particles: Trumpets thrown toward center (+X) ----
	ParticleSystemInfo char03BulletInfo;
	char03BulletInfo.textureId = tm->LoadTexture("Images/trumpet.png");
	char03BulletInfo.maxParticles = 300;
	char03BulletInfo.particlesPerEmit = { 1, 2 };
	char03BulletInfo.delay = 0.0f;
	char03BulletInfo.lifeTime = FLT_MAX;
	char03BulletInfo.timeBetweenEmit = { 0.4f, 0.8f };
	char03BulletInfo.spawnAngle = { -10.0f, 10.0f };
	char03BulletInfo.spawnSpeed = { 8.0f, 12.0f };
	char03BulletInfo.particleLifeTime = { 1.0f, 2.0f };
	char03BulletInfo.spawnDirection = { 1.0f, 0.0f, 0.0f };
	char03BulletInfo.spawnPosition = Math::Vector3::Zero;
	char03BulletInfo.startScale = { Math::Vector3::One * 1.2f, Math::Vector3::One * 1.5f };
	char03BulletInfo.endScale = { Math::Vector3::One * 0.8f, Math::Vector3::One * 1.0f };
	char03BulletInfo.startColour = { Colors::White, Colors::White };
	char03BulletInfo.endColour = { Colors::White, Colors::White };
	mChar03Bullets.Initialize(char03BulletInfo);

	// Explosion particles
	ParticleSystemInfo explosionInfo;
	explosionInfo.textureId = tm->LoadTexture("Images/Sombrero.png");
	explosionInfo.maxParticles = 2000;
	explosionInfo.particlesPerEmit = { 30, 60 };
	explosionInfo.delay = 0.0f;
	explosionInfo.lifeTime = 0.0f;
	explosionInfo.timeBetweenEmit = { 0.0f, 0.0f };
	explosionInfo.spawnAngle = { -180.0f, 180.0f };
	explosionInfo.spawnSpeed = { 5.0f, 15.0f };
	explosionInfo.particleLifeTime = { 0.5f, 2.5f };
	explosionInfo.spawnDirection = Math::Vector3::YAxis;
	explosionInfo.spawnPosition = Math::Vector3::Zero;
	explosionInfo.startScale = { Math::Vector3::One * 0.8f, Math::Vector3::One * 1.5f };
	explosionInfo.endScale = { Math::Vector3::Zero, Math::Vector3::Zero };
	explosionInfo.startColour = { Colors::Yellow, Colors::Red };
	explosionInfo.endColour = { Colors::OrangeRed, Colors::White };
	mExplosionParticles.Initialize(explosionInfo);

	// ============================================================
	// AUDIO
	// ============================================================
	SoundEffectManager* sm = SoundEffectManager::Get();
	mBgMusicId = sm->Load("UnDosTres.wav");
	mCoughSoundId = sm->Load("Cough.wav");

	// ============================================================
	// EVENTS
	// ============================================================
	EventManager* em = EventManager::Get();
	mCollisionListenerId = em->AddListener(CollisionEvent::StaticGetTypeId(),
		std::bind(&GameState::OnCollisionEvent, this, std::placeholders::_1));
	mFireworkListenerId = em->AddListener(FireworkEvent::StaticGetTypeId(),
		std::bind(&GameState::OnFireworkEvent, this, std::placeholders::_1));

	// ============================================================
	// CHOREOGRAPHY LAMBDAS
	// ============================================================
	auto playWarningSound = [&]()
		{ SoundEffectManager::Get()->Play(mWarningSoundId); };
	auto playExplosionSound = [&]()
		{ SoundEffectManager::Get()->Play(mExplosionSoundId); };

	// --- Char01 ---
	auto char01SadIdleWithCough = [&]()
		{
			SoundEffectManager::Get()->Play(mCoughSoundId);
			mChar01Animator.PlayAnimation(AnimClip::Sad_Idle, true);
		};
	auto startMusic = [&]()
		{ SoundEffectManager::Get()->Play(mBgMusicId); };
	auto char01Hip_Hop_Dancing = [&]()
		{ mChar01Animator.PlayAnimation(AnimClip::Hip_Hop_Dancing, true); };
	auto char01Dancing = [&]()
		{ mChar01Animator.PlayAnimation(AnimClip::Dancing, true); };
	auto char01Charge = [&]()
		{ mChar01Animator.PlayAnimation(AnimClip::Charge, true); };
	auto char01Kneeling_Pointi = [&]()
		{ mChar01Animator.PlayAnimation(AnimClip::Kneeling_Pointi, true); };
	auto char01Dancing_Twerk = [&]()
		{ mChar01Animator.PlayAnimation(AnimClip::Dancing_Twerk, true); };
	auto char01Stand_Still = [&]()
		{ mChar01Animator.PlayAnimation(AnimClip::Stand_still, false); };

	// --- Char02 ---
	auto char02Hip_Hop_Dancing = [&]()
		{ mChar02Animator.PlayAnimation(AnimClip::Hip_Hop_Dancing, true); };
	auto char02Dancing = [&]()
		{ mChar02Animator.PlayAnimation(AnimClip::Dancing, true); };
	auto char02Charge = [&]()
		{ mChar02Animator.PlayAnimation(AnimClip::Charge, true); };
	auto char02Dancing_Twerk = [&]()
		{ mChar02Animator.PlayAnimation(AnimClip::Dancing_Twerk, true); };
	auto char02Stand_Still = [&]()
		{ mChar02Animator.PlayAnimation(AnimClip::Stand_still, false); };
	auto char02Back_point = [&]()
		{ mChar02Animator.PlayAnimation(AnimClip::Kneeling_Pointi, true); };
	auto char02JogBackwards  = [&]()
		{ mChar02Animator.PlayAnimation(AnimClip::Slow_Jog_Backwards, true); };

	// --- Char03 ---
	auto char03Hip_Hop_Dancing = [&]()
		{ mChar03Animator.PlayAnimation(AnimClip::Hip_Hop_Dancing, true); };
	auto char03Dancing = [&]()
		{ mChar03Animator.PlayAnimation(AnimClip::Dancing, true); };
	auto char03Charge = [&]()
		{ mChar03Animator.PlayAnimation(AnimClip::Charge, true); };
	auto char03Dancing_Twerk = [&]()
		{ mChar03Animator.PlayAnimation(AnimClip::Dancing_Twerk, true); };
	auto char03Stand_Still = [&]()
		{ mChar03Animator.PlayAnimation(AnimClip::Stand_still, false); };
	auto char03Back_point = [&]()
		{ mChar03Animator.PlayAnimation(AnimClip::Kneeling_Pointi, true); };
	auto char03SlowJogBackwards = [&]() 
		{ mChar03Animator.PlayAnimation(AnimClip::Slow_Jog_Backwards, true ); };

	// --- Outro ---
	auto outroAllSadIdle = [&]()
		{
			SoundEffectManager::Get()->Play(mCoughSoundId);
			mChar01Animator.PlayAnimation(AnimClip::Sad_Idle, true);
		};

	// --- Collision / explosion events ---
	auto triggerClash = [&]()
		{
			mExplosionParticles.SetPosition({ 0.0f, 1.0f, 0.0f });
			mExplosionParticles.SpawnParticles();
			SoundEffectManager::Get()->Play(mExplosionSoundId);
			CollisionEvent evt;
			EventManager::Broadcast(evt);
		};
	auto triggerSecondExplosion = [&]()
		{
			mExplosionParticles.SetPosition({ -1.0f, 1.5f, 0.0f });
			mExplosionParticles.SpawnParticles();
			SoundEffectManager::Get()->Play(mExplosionSoundId);
		};
	auto triggerFirework = [&]()
		{
			mExplosionParticles.SetPosition({ 0.0f, 4.0f, 0.0f });
			mExplosionParticles.SpawnParticles();
			SoundEffectManager::Get()->Play(mFireworkSoundId);
			FireworkEvent evt;
			EventManager::Broadcast(evt);
		};

	// ============================================================
	// CHARACTER 01 MOVEMENT - 45 SECONDS
	// Choreography:
	//   0.0-4.0s:   Sad idle + Cough sound
	//   4.0-9.5s:   Hip hop dancing
	//   9.5-10.5s:  Stand still
	//   10.5-15.0s: Hip hop dancing
	//   15.0-16.0s: Pointing at Esso (shifted right, 15 deg rotation)
	//   16.0-19.5s: Twerking
	//   19.5-24.0s: Dancing
	//   24.0-31.5s: Hip hop dancing
	//   31.5-41.0s: Twerking
	//   41.0-45.0s: Sad idle + cough (outro)
	// ============================================================
	mChar01Movement = AnimationBuilder()
		.AddPositionKey({ 0.0f, 0.0f, 0.0f }, 0.0f)
		.AddPositionKey({ 0.0f, 0.0f, 0.0f }, 14.5f)
		.AddPositionKey({ 2.0f, 0.0f, 0.0f }, 15.0f)
		.AddPositionKey({ 2.0f, 0.0f, 0.0f }, 15.99f)
		.AddPositionKey({ 0.0f, 0.0f, 0.0f }, 16.5f)
		.AddPositionKey({ 0.0f, 0.0f, 0.0f }, 45.0f)

		.AddRotationKey(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, 0.0f), 0.0f)
		.AddRotationKey(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, 0.0f), 14.5f)
		.AddRotationKey(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, Math::Constants::Pi / 12.0f), 15.0f)
		.AddRotationKey(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, Math::Constants::Pi / 12.0f), 15.99f)
		.AddRotationKey(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, 0.0f), 16.5f)
		.AddRotationKey(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, 0.0f), 45.0f)

		.AddScaleKey(Math::Vector3(1.0f, 1.0f, 1.0f), 0.0f)
		.AddScaleKey(Math::Vector3(1.0f, 1.0f, 1.0f), 45.0f)

		.AddEventKey(char01SadIdleWithCough, 0.01f)
		.AddEventKey(startMusic, 4.0f)
		.AddEventKey(char01Hip_Hop_Dancing, 4.0f)
		.AddEventKey(char01Stand_Still, 9.5f)
		.AddEventKey(char01Hip_Hop_Dancing, 10.5f)
		.AddEventKey(char01Kneeling_Pointi, 15.0f)
		.AddEventKey(char01Dancing_Twerk, 16.0f)
		.AddEventKey(char01Dancing, 19.5f)
		.AddEventKey(char01Hip_Hop_Dancing, 24.0f)
		.AddEventKey(char01Dancing_Twerk, 31.5f)
		.AddEventKey(outroAllSadIdle, 41.0f)
		.Build();

	// ============================================================
	// CHARACTER 02 MOVEMENT - 45 SECONDS
	// Choreography:
	//   0.0-19.0s:  Out of screen (skips Esso moment entirely)
	//   19.5-21.5s: Comes in - hip hop
	//   21.5-24.0s: Twerking
	//   24.0-31.5s: Dancing
	//   31.5-41.0s: Twerking
	//   41.0-45.0s: Jog backwards off screen
	// ============================================================
	mChar02Movement = AnimationBuilder()
		.AddPositionKey({ 15.0f, 0.0f, 0.0f }, 0.0f)
		.AddPositionKey({ 15.0f, 0.0f, 0.0f }, 19.0f)
		.AddPositionKey({ 5.0f, 0.0f, 0.0f }, 19.5f)
		.AddPositionKey({ 5.0f, 0.0f, 0.0f }, 41.0f)
		.AddPositionKey({ 15.0f, 0.0f, 0.0f }, 45.0f)

		.AddRotationKey(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, Math::Constants::HalfPi), 0.0f)
		.AddRotationKey(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, Math::Constants::HalfPi), 45.0f)

		.AddScaleKey(Math::Vector3(1.0f, 1.0f, 1.0f), 0.0f)
		.AddScaleKey(Math::Vector3(1.0f, 1.0f, 1.0f), 45.0f)

		.AddEventKey(char02Stand_Still, 0.0f)
		.AddEventKey(char02Back_point, 15.99f)
		.AddEventKey(char02Hip_Hop_Dancing, 19.5f)
		.AddEventKey(char02Dancing_Twerk, 21.5f)
		.AddEventKey(char02Dancing, 24.0f)
		.AddEventKey(char02Dancing_Twerk, 31.5f)
		.AddEventKey(char02JogBackwards, 41.0f)
		.Build();

	// ============================================================
	// CHARACTER 03 MOVEMENT - 45 SECONDS
	// Choreography:
	//   0.0-15.0s:  Out of screen
	//   15.0-15.5s: Slides in, spins back to camera, points at Esso
	//   15.5-16.0s: Quickly exits
	//   19.5-21.5s: Comes in - hip hop
	//   21.5-24.0s: Twerking
	//   24.0-31.5s: Dancing
	//   31.5-41.0s: Twerking
	//   41.0-45.0s: Jog backwards off screen
	// ============================================================
	mChar03Movement = AnimationBuilder()
		.AddPositionKey({ -15.0f, 0.0f, 0.0f }, 0.0f)
		.AddPositionKey({ -15.0f, 0.0f, 0.0f }, 14.8f)
		.AddPositionKey({ -2.0f, 0.0f, 0.0f }, 15.0f)
		.AddPositionKey({ -2.0f, 0.0f, 0.0f }, 15.99f)
		.AddPositionKey({ -15.0f, 0.0f, 0.0f }, 16.0f)
		.AddPositionKey({ -15.0f, 0.0f, 0.0f }, 19.0f)
		.AddPositionKey({ -5.0f, 0.0f, 0.0f }, 19.5f)
		.AddPositionKey({ -5.0f, 0.0f, 0.0f }, 41.0f)
		.AddPositionKey({ -15.0f, 0.0f, 0.0f }, 45.0f)

		.AddRotationKey(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, -Math::Constants::HalfPi), 0.0f)
		.AddRotationKey(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, -Math::Constants::HalfPi), 14.8f)
		.AddRotationKey(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, Math::Constants::Pi), 15.0f)
		.AddRotationKey(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, Math::Constants::Pi), 15.5f)
		.AddRotationKey(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, -Math::Constants::HalfPi), 16.5f)
		.AddRotationKey(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, -Math::Constants::HalfPi), 45.0f)

		.AddScaleKey(Math::Vector3(1.0f, 1.0f, 1.0f), 0.0f)
		.AddScaleKey(Math::Vector3(1.0f, 1.0f, 1.0f), 45.0f)

		.AddEventKey(char03Stand_Still, 0.0f)
		.AddEventKey(char03Back_point, 15.00f)
		.AddEventKey(char03Stand_Still, 16.0f)
		.AddEventKey(char03Hip_Hop_Dancing, 19.5f)
		.AddEventKey(char03Dancing_Twerk, 21.5f)
		.AddEventKey(char03Dancing, 24.0f)
		.AddEventKey(char03Dancing_Twerk, 31.5f)
		.AddEventKey(char03SlowJogBackwards, 41.0f)         // Slow jog backwards off screen
		.Build();

	// Initialize scene time
	mSceneTime = 0.0f;
	mTotalSceneTime = SCENE_DURATION;
	mBgMusicStarted = false;
}

void GameState::Terminate()
{
	SoundEffectManager* sm = SoundEffectManager::Get();
	sm->Stop(mBgMusicId);
	sm->Stop(mCoughSoundId);
	sm->Stop(mWarningSoundId);
	sm->Stop(mExplosionSoundId);
	sm->Stop(mFireworkSoundId);

	EventManager* em = EventManager::Get();
	em->RemoveListener(CollisionEvent::StaticGetTypeId(), mCollisionListenerId);
	em->RemoveListener(FireworkEvent::StaticGetTypeId(), mFireworkListenerId);

	mExplosionParticles.Terminate();
	mChar03Bullets.Terminate();
	mChar02Bullets.Terminate();
	mChar01Bullets.Terminate();
	mParticleEffect.Terminate();

	mEssoModel.meshBuffer.Terminate();
	mEssoModel.Terminate();
	mFloor.meshBuffer.Terminate();
	mFloor.Terminate();
	mCharacter03.Terminate();
	mCharacter02.Terminate();
	mCharacter01.Terminate();
	mStandardEffect.Terminate();
}

void GameState::Update(float deltaTime)
{
	UpdateCamera(deltaTime);

	// Start background music is triggered by event key at 4s (after cough intro)

	// Disco light - cycles through 3 colors while music plays (4s - 41s)
	float displayTime = fmod(mSceneTime, mTotalSceneTime);
	if (displayTime >= 4.0f && displayTime < 41.0f)
	{
		float t = fmod(mSceneTime * (1.0f / 1.5f), 3.0f);

		Color colorA = { 1.0f, 0.1f, 0.8f, 1.0f }; // magenta/pink
		Color colorB = { 0.1f, 0.9f, 1.0f, 1.0f }; // cyan
		Color colorC = { 1.0f, 0.8f, 0.1f, 1.0f }; // gold/yellow

		Color current;
		if (t < 1.0f)
		{
			current.r = colorA.r + (colorB.r - colorA.r) * t;
			current.g = colorA.g + (colorB.g - colorA.g) * t;
			current.b = colorA.b + (colorB.b - colorA.b) * t;
		}
		else if (t < 2.0f)
		{
			float s = t - 1.0f;
			current.r = colorB.r + (colorC.r - colorB.r) * s;
			current.g = colorB.g + (colorC.g - colorB.g) * s;
			current.b = colorB.b + (colorC.b - colorB.b) * s;
		}
		else
		{
			float s = t - 2.0f;
			current.r = colorC.r + (colorA.r - colorC.r) * s;
			current.g = colorC.g + (colorA.g - colorC.g) * s;
			current.b = colorC.b + (colorA.b - colorC.b) * s;
		}
		current.a = 1.0f;
		mDirectionalLight.diffuse = current;
	}
	else
	{
		mDirectionalLight.diffuse = { 0.9f, 0.9f, 0.8f, 1.0f };
	}

	// Update skeleton animators
	mChar01Animator.Update(deltaTime);
	mChar02Animator.Update(deltaTime);
	mChar03Animator.Update(deltaTime);

	// Track previous time for event playback
	float prevChar01Time = mChar01MoveTime;
	float prevChar02Time = mChar02MoveTime;
	float prevChar03Time = mChar03MoveTime;

	// Advance scene time
	mSceneTime += deltaTime;
	mEssoMoveTime += deltaTime;
	mChar01MoveTime += deltaTime;
	mChar02MoveTime += deltaTime;
	mChar03MoveTime += deltaTime;

	// Play events and loop
	if (mChar01Movement.GetDuration() > 0)
	{
		mChar01Movement.PlayEvents(prevChar01Time, mChar01MoveTime);
		while (mChar01MoveTime >= mChar01Movement.GetDuration())
		{
			mChar01MoveTime -= mChar01Movement.GetDuration();
			mChar01Animator.PlayAnimation(AnimClip::Hip_Hop_Dancing, true);
		}
	}

	if (mChar02Movement.GetDuration() > 0)
	{
		mChar02Movement.PlayEvents(prevChar02Time, mChar02MoveTime);
		while (mChar02MoveTime >= mChar02Movement.GetDuration())
		{
			mChar02MoveTime -= mChar02Movement.GetDuration();
			mChar02Animator.PlayAnimation(AnimClip::Hip_Hop_Dancing, true);
		}
	}

	if (mChar03Movement.GetDuration() > 0)
	{
		mChar03Movement.PlayEvents(prevChar03Time, mChar03MoveTime);
		while (mChar03MoveTime >= mChar03Movement.GetDuration())
		{
			mChar03MoveTime -= mChar03Movement.GetDuration();
			mChar03Animator.PlayAnimation(AnimClip::Hip_Hop_Dancing, true);
		}
	}

	// Update particles
	mChar01Bullets.Update(deltaTime);
	mChar02Bullets.Update(deltaTime);
	mChar03Bullets.Update(deltaTime);
	mExplosionParticles.Update(deltaTime);
}

void GameState::Render()
{
	// Apply Esso movement (scale drives visibility)
	Transform essoTransform = mEssoMovement.GetTransform(mEssoMoveTime);
	mEssoModel.transform.position = essoTransform.position;
	mEssoModel.transform.scale = essoTransform.scale;

	Transform char01Transform = mChar01Movement.GetTransform(mChar01MoveTime);
	mCharacter01.transform.position = char01Transform.position;
	mCharacter01.transform.rotation = char01Transform.rotation;
	mCharacter01.transform.scale = char01Transform.scale;

	Transform char02Transform = mChar02Movement.GetTransform(mChar02MoveTime);
	mCharacter02.transform.position = char02Transform.position;
	mCharacter02.transform.rotation = char02Transform.rotation;
	mCharacter02.transform.scale = char02Transform.scale;

	Transform char03Transform = mChar03Movement.GetTransform(mChar03MoveTime);
	mCharacter03.transform.position = char03Transform.position;
	mCharacter03.transform.rotation = char03Transform.rotation;
	mCharacter03.transform.scale = char03Transform.scale;

	// Move bullet emitters to character chest height
	Math::Vector3 char01BulletPos = mCharacter01.transform.position + Math::Vector3{ 0.0f, 1.5f, 0.0f };
	Math::Vector3 char02BulletPos = mCharacter02.transform.position + Math::Vector3{ 0.0f, 1.5f, 0.0f };
	Math::Vector3 char03BulletPos = mCharacter03.transform.position + Math::Vector3{ 0.0f, 1.5f, 0.0f };
	mChar01Bullets.SetPosition(char01BulletPos);
	mChar02Bullets.SetPosition(char02BulletPos);
	mChar03Bullets.SetPosition(char03BulletPos);

	// Draw textured floor
	mStandardEffect.Begin();
	mStandardEffect.Render(mFloor);
	mStandardEffect.Render(mEssoModel);
	mStandardEffect.Render(mCharacter01);
	mStandardEffect.Render(mCharacter02);
	mStandardEffect.Render(mCharacter03);
	mStandardEffect.End();

	// Render all particle systems
	mParticleEffect.Begin();
	mChar01Bullets.Render(mParticleEffect);
	mChar02Bullets.Render(mParticleEffect);
	mChar03Bullets.Render(mParticleEffect);
	mExplosionParticles.Render(mParticleEffect);
	mParticleEffect.End();
}

void GameState::DebugUI()
{
	ImGui::Begin("Fighter Showdown - Animation Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "SCENE TIMER");
	ImGui::Separator();

	float displayTime = fmod(mSceneTime, mTotalSceneTime);
	int minutes = static_cast<int>(mSceneTime) / 60;
	int seconds = static_cast<int>(mSceneTime) % 60;

	ImGui::Text("Elapsed: %02d:%02d (%.1fs total)", minutes, seconds, mSceneTime);
	ImGui::Text("Scene Duration: %.0fs", mTotalSceneTime);
	ImGui::Text("Current Loop Time: %.1fs / %.0fs", displayTime, mTotalSceneTime);

	float progress = displayTime / mTotalSceneTime;
	ImGui::ProgressBar(progress, ImVec2(300, 20));

	ImGui::Separator();
	ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.5f, 1.0f), "SCENE PHASE");
	if (displayTime < 4.0f)
		ImGui::Text("Phase 1: Char01 Sad Idle + Cough (0-4s)");
	else if (displayTime < 9.5f)
		ImGui::Text("Phase 2: Char01 Hip Hop Dancing (4-9.5s)");
	else if (displayTime < 10.5f)
		ImGui::Text("Phase 3: Char01 Stand Still (9.5-10.5s)");
	else if (displayTime < 15.0f)
		ImGui::Text("Phase 4: Char01 Hip Hop (10.5-15s)");
	else if (displayTime < 16.0f)
		ImGui::Text("Phase 5: ALL - Pointing at Esso! (15-16s)");
	else if (displayTime < 19.5f)
		ImGui::Text("Phase 6: Char01 Twerking / Char02+03 Off Screen");
	else if (displayTime < 21.5f)
		ImGui::Text("Phase 7: Char01 Dancing / Char02+03 Hip Hop Entry");
	else if (displayTime < 24.0f)
		ImGui::Text("Phase 8: Char01 Dancing / Char02+03 Twerking");
	else if (displayTime < 31.5f)
		ImGui::Text("Phase 9: ALL Dancing (24-31.5s)");
	else if (displayTime < 41.0f)
		ImGui::Text("Phase 10: ALL Twerking (31.5-41s)");
	else
		ImGui::Text("Phase 11: Outro - Sad Idle + Cough, Char02+03 Jog Backwards");

	ImGui::Separator();
	ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "CHARACTER POSITIONS");
	ImGui::Text("Char01: (%.1f, %.1f, %.1f)", mCharacter01.transform.position.x, mCharacter01.transform.position.y, mCharacter01.transform.position.z);
	ImGui::Text("Char02: (%.1f, %.1f, %.1f)", mCharacter02.transform.position.x, mCharacter02.transform.position.y, mCharacter02.transform.position.z);
	ImGui::Text("Char03: (%.1f, %.1f, %.1f)", mCharacter03.transform.position.x, mCharacter03.transform.position.y, mCharacter03.transform.position.z);

	ImGui::Separator();
	if (ImGui::CollapsingHeader("Light Settings"))
	{
		if (ImGui::DragFloat3("Direction##Light", &mDirectionalLight.direction.x, 0.01f))
			mDirectionalLight.direction = Math::Normalize(mDirectionalLight.direction);
		ImGui::ColorEdit4("Ambient##Light", &mDirectionalLight.ambient.r);
		ImGui::ColorEdit4("Diffuse##Light", &mDirectionalLight.diffuse.r);
		ImGui::ColorEdit4("Specular##Light", &mDirectionalLight.specular.r);
	}

	if (ImGui::CollapsingHeader("Particle Debug"))
		mChar01Bullets.DebugUI();

	if (ImGui::CollapsingHeader("Physics Debug"))
		PhysicsWorld::Get()->DebugUI();

	mStandardEffect.DebugUI();
	ImGui::End();
}

void GameState::UpdateCamera(float deltaTime)
{
	InputSystem* input = InputSystem::Get();
	const float moveSpeed = input->IsKeyDown(KeyCode::LSHIFT) ? 10.0f : 4.0f;
	const float turnSpeed = 0.5f;

	if (input->IsKeyDown(KeyCode::W)) mCamera.Walk(moveSpeed * deltaTime);
	else if (input->IsKeyDown(KeyCode::S)) mCamera.Walk(-moveSpeed * deltaTime);

	if (input->IsKeyDown(KeyCode::D)) mCamera.Strafe(moveSpeed * deltaTime);
	else if (input->IsKeyDown(KeyCode::A)) mCamera.Strafe(-moveSpeed * deltaTime);

	if (input->IsKeyDown(KeyCode::E)) mCamera.Rise(moveSpeed * deltaTime);
	else if (input->IsKeyDown(KeyCode::Q)) mCamera.Rise(-moveSpeed * deltaTime);

	if (input->IsMouseDown(MouseButton::RBUTTON))
	{
		mCamera.Yaw(input->GetMouseMoveX() * turnSpeed * deltaTime);
		mCamera.Pitch(input->GetMouseMoveY() * turnSpeed * deltaTime);
	}
}

void GameState::OnCollisionEvent(const IExeEngine::Core::Event& e)
{
	LOG("COLLISION EVENT: The fighters have clashed!");
}

void GameState::OnFireworkEvent(const IExeEngine::Core::Event& e)
{
	LOG("FIREWORK EVENT: Victory celebration!");
}	
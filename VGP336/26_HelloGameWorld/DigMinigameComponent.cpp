#include "DigMinigameComponent.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;
using namespace IExeEngine::Input;
using namespace IExeEngine::Math;

void DigMinigameComponent::Initialize()
{
	mCompassBase.Initialize("Images/compass_base.png");
	mCompassNeedle.Initialize("Images/compass_needle.png");

	mCompassBase.SetPivot(Pivot::Centre);
	mCompassNeedle.SetPivot(Pivot::Centre);

	mCompassBase.SetScale({ 0.3f, 0.3f });
	mCompassNeedle.SetScale({ 0.3f, 0.3f });

	UIRenderService* uiRenderService = GetOwner().GetWorld().GetService<UIRenderService>();
	ASSERT(uiRenderService != nullptr, "DigMinigameComponent: Needs a UI render service!");
	uiRenderService->Register(this);

	mPrevMouseWheel = InputSystem::Get()->GetMouseMoveZ();

	// Initialize Confetti Particle System
	Physics::ParticleSystemInfo info;
	info.textureId = TextureManager::Get()->LoadTexture(L"Images/white.png");
	info.maxParticles = 500;
	info.particlesPerEmit = { 6, 12 };
	info.delay = 0.0f;
	info.lifeTime = 0.1f; // Short system lifetime for manual burst triggering
	info.timeBetweenEmit = { 0.01f, 0.03f };
	info.spawnAngle = { 0.0f, 360.0f }; // Burst in all directions
	info.spawnSpeed = { 6.0f, 12.0f };  // Shoot outwards quickly
	info.particleLifeTime = { 1.0f, 2.5f };
	info.spawnDirection = Vector3::YAxis; // Base upward push
	info.spawnPosition = mTargetPosition;
	info.startScale = { Vector3{0.05f, 0.05f, 0.05f}, Vector3{0.12f, 0.12f, 0.12f} };
	info.endScale = { Vector3{0.005f, 0.005f, 0.005f}, Vector3{0.03f, 0.03f, 0.03f} };
	info.startColour = { Colors::Red, Colors::Violet };
	info.endColour = { Colors::Yellow, Colors::Blue };

	mParticleSystem.Initialize(info);
	mParticleSystemEffect.Initialize();
}

void DigMinigameComponent::Terminate()
{
	mParticleSystemEffect.Terminate();
	mParticleSystem.Terminate();

	UIRenderService* uiRenderService = GetOwner().GetWorld().GetService<UIRenderService>();
	if (uiRenderService != nullptr)
	{
		uiRenderService->Unregister(this);
	}
	mCompassBase.Terminate();
	mCompassNeedle.Terminate();
}

void DigMinigameComponent::Update(float deltaTime)
{
	auto input = InputSystem::Get();
	
	// Update particles
	mParticleSystem.Update(deltaTime);

	// Decrement loop wait timer if active
	if (mTreasureFound)
	{
		mWaitTimer -= deltaTime;
		if (mWaitTimer <= 0.0f)
		{
			mTreasureFound = false;

			// Assign another target spot in the map (100x100 space, coordinates -45 to 45)
			float newX = -45.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / 90.0f);
			float newZ = -45.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / 90.0f);
			mTargetPosition = { newX, 0.0f, newZ };
		}
	}

	// Mode switching with scroll wheel (only if not currently waiting after finding treasure)
	if (!mTreasureFound)
	{
		float currentZ = input->GetMouseMoveZ();
		if (currentZ != mPrevMouseWheel)
		{
			// Toggle mode
			mMode = (mMode == Mode::Compass) ? Mode::Dig : Mode::Compass;
			mPrevMouseWheel = currentZ;
		}
	}

	// Compute player position and angle to target
	auto transform = GetOwner().GetComponent<TransformComponent>();
	if (transform != nullptr)
	{
		Vector3 playerPos = transform->position;
		
		float dx = mTargetPosition.x - playerPos.x;
		float dy = mTargetPosition.z - playerPos.z; // Translate Y/Z axis in 3D world
		float distance = std::sqrt(dx * dx + dy * dy);
		
		if (mMode == Mode::Compass)
		{
			// Point needle at the target
			float targetAngle = std::atan2(dx, dy);

			// Player forward direction
			Matrix4 matrix = transform->GetMatrix4();
			Vector3 forward = GetLook(matrix);
			float playerAngle = std::atan2(forward.x, forward.z);

			// Relative angle
			float relativeAngle = targetAngle - playerAngle;
			
			mCompassNeedle.SetRotation(relativeAngle);
		}
		else if (mMode == Mode::Dig && !mTreasureFound)
		{
			// Check for digging trigger (Left click)
			if (input->IsMousePressed(MouseButton::LBUTTON))
			{
				auto soundBank = GetOwner().GetComponent<SoundBankComponent>();
				if (soundBank != nullptr)
				{
					if (distance <= mSuccessRadius)
					{
						// Success! Play reward sound
						soundBank->Play("ShovelSuccess");
						mTreasureFound = true;
						mWaitTimer = 3.0f; // Wait 3 seconds to reset

						// Trigger the particle explosion at target coordinates
						mParticleSystem.SetPosition(mTargetPosition);
						for (int i = 0; i < 25; ++i)
						{
							mParticleSystem.SpawnParticles();
						}
					}
					else
					{
						// Fail! Play shovel sound
						soundBank->Play("ShovelFail");
					}
				}
			}
		}
	}
}

void DigMinigameComponent::Render()
{
	auto gs = GraphicsSystem::Get();
	float screenW = static_cast<float>(gs->GetBackBufferWidth());
	float screenH = static_cast<float>(gs->GetBackBufferHeight());

	// Draw 3D particles if active
	if (mParticleSystem.IsActive())
	{
		CameraService* cameraService = GetOwner().GetWorld().GetService<CameraService>();
		if (cameraService != nullptr)
		{
			const Camera& camera = cameraService->GetMain();
			mParticleSystemEffect.SetCamera(camera);
		}

		// Suspend 2D UI render pass
		UISpriteRenderer::Get()->EndRender();

		mParticleSystemEffect.Begin();
		mParticleSystem.Render(mParticleSystemEffect);
		mParticleSystemEffect.End();

		// Resume 2D UI render pass
		UISpriteRenderer::Get()->BeginRender();
	}

	Vector2 compassPos = { screenW - 150.0f, screenH - 150.0f };
	mCompassBase.SetPosition(compassPos);
	mCompassNeedle.SetPosition(compassPos);

	if (mMode == Mode::Compass)
	{
		UISpriteRenderer::Get()->Render(mCompassBase);
		UISpriteRenderer::Get()->Render(mCompassNeedle);
	}

	// Draw Minimal HUD text
	std::wstring modeText = L"HUD Option: ";
	if (mMode == Mode::Compass)
	{
		modeText += L"Compass";
	}
	else
	{
		modeText += L"Shovel";
	}

	UIFont::Get()->DrawString(
		modeText.c_str(), 
		{ 30.0f, 30.0f }, 
		Colors::Gold, 
		20.0f
	);

	if (mTreasureFound)
	{
		std::wstring successMsg = L"TREASURE FOUND! Next target in " + std::to_wstring(static_cast<int>(std::ceil(mWaitTimer))) + L"s...";
		UIFont::Get()->DrawString(
			successMsg.c_str(), 
			{ screenW / 2.0f - 220.0f, screenH / 2.0f - 20.0f }, 
			Colors::Yellow, 
			28.0f
		);
	}
}

void DigMinigameComponent::DebugUI()
{
	ImGui::Text("Minigame Debug Status:");
	ImGui::Text("Current Mode: %s", mMode == Mode::Compass ? "Compass" : "Dig");
	ImGui::DragFloat3("Target Position", &mTargetPosition.x, 0.1f);
	ImGui::DragFloat("Success Radius", &mSuccessRadius, 0.1f);
	ImGui::Text("Treasure Found: %s", mTreasureFound ? "YES" : "NO");
	if (mTreasureFound)
	{
		ImGui::Text("Wait Reset Timer: %.2f", mWaitTimer);
	}

	auto transform = GetOwner().GetComponent<TransformComponent>();
	if (transform != nullptr)
	{
		Vector3 playerPos = transform->position;
		float dx = mTargetPosition.x - playerPos.x;
		float dy = mTargetPosition.z - playerPos.z;
		float distance = std::sqrt(dx * dx + dy * dy);
		ImGui::Text("Player Position: (%.2f, %.2f, %.2f)", playerPos.x, playerPos.y, playerPos.z);
		ImGui::Text("Distance to Target: %.2f (Success Radius: %.2f)", distance, mSuccessRadius);
	}
	
	if (ImGui::Button("Reset Treasure"))
	{
		mTreasureFound = false;
		mWaitTimer = 0.0f;
	}

	mParticleSystem.DebugUI();
}

void DigMinigameComponent::Deserialize(const rapidjson::Value& value)
{
	if (value.HasMember("TargetPosition"))
	{
		SaveUtil::ReadVector3("TargetPosition", mTargetPosition, value);
	}
	if (value.HasMember("SuccessRadius"))
	{
		SaveUtil::ReadFloat("SuccessRadius", mSuccessRadius, value);
	}
}

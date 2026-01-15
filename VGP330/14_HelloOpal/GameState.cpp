#include "GameState.h"
#include <IExeEngine/Inc/IExeEngine.h>
#include <Graphics/Inc/OpalEffect.h>

using namespace IExeEngine;
using namespace IExeEngine::Graphics;
using namespace IExeEngine::Input;

void GameState::Initialize()
{
	mCamera.SetPosition({ 0.0f, 1.5f, -2.0f });
	mCamera.SetLookAt({ 0.0f, 1.0f, 0.0f });

	mDirectionalLight.direction = Math::Normalize({ 1.0f, -1.0f, 1.0f });
	mDirectionalLight.ambient = { 0.4f, 0.4f, 0.4f, 1.0f };
	mDirectionalLight.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	mDirectionalLight.specular = { 0.9f, 0.9f, 0.9f, 1.0f };

	Mesh groundMesh = MeshBuilder::CreatePlane(25, 25, 1.0f);
	mGround.meshBuffer.Initialize(groundMesh);
	mGround.diffuseMapId = TextureManager::Get()->LoadTexture("HexGround/HexGround_Diff.jpg");
	mGround.transform.position = { 0.0f, 0.0f, 0.0f };

	mCharacter.Initialize("Character01/Character01.model");
	mCharacter.transform.position = { 0.0f, 0.0f, 0.0f };
    mCharacter.transform.scale = { 10.0f, 10.0f, 10.0f };

    mOpalEffect.Initialize(L"../../Assets/Shaders/Opal.fx");
    mOpalEffect.SetCamera(mCamera);
    mOpalEffect.SetDirectionalLight(mDirectionalLight);
}

void GameState::Terminate()
{
	mCharacter.Terminate();
	mGround.Terminate();
    mOpalEffect.Terminate();
}

void GameState::Update(float deltaTime)
{
	UpdateCamera(deltaTime);
    mOpalEffect.SetTime(TimeUtil::GetTime());
}

void GameState::Render()
{
    // Render Ground
	// ... 

    // Render Character with OpalEffect
     mOpalEffect.Begin();
     mOpalEffect.Render(mCharacter);
     mOpalEffect.End();
}

void GameState::DebugUI()
{
	ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	
    // Light DebugUI
	if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::DragFloat3("Direction#Light", &mDirectionalLight.direction.x, 0.01f))
		{
			mDirectionalLight.direction = Math::Normalize(mDirectionalLight.direction);
		}
		ImGui::ColorEdit4("Ambient#Light", &mDirectionalLight.ambient.r);
		ImGui::ColorEdit4("Diffuse#Light", &mDirectionalLight.diffuse.r);
		ImGui::ColorEdit4("Specular#Light", &mDirectionalLight.specular.r);
	}

    // Opal Effect DebugUI
    mOpalEffect.DebugUI();

	ImGui::End();
}

void GameState::UpdateCamera(float deltaTime)
{
	InputSystem* input = InputSystem::Get();
	const float moveSpeed = input->IsKeyDown(KeyCode::LSHIFT) ? 10.0f : 4.0f;
	const float turnSpeed = 0.5f;

	if (input->IsKeyDown(KeyCode::W)) { mCamera.Walk(moveSpeed * deltaTime); }
	else if (input->IsKeyDown(KeyCode::S)) { mCamera.Walk(-moveSpeed * deltaTime); }
	else if (input->IsKeyDown(KeyCode::D)) { mCamera.Strafe(moveSpeed * deltaTime); }
	else if (input->IsKeyDown(KeyCode::A)) { mCamera.Strafe(-moveSpeed * deltaTime); }
	else if (input->IsKeyDown(KeyCode::E)) { mCamera.Rise(moveSpeed * deltaTime); }
	else if (input->IsKeyDown(KeyCode::Q)) { mCamera.Rise(-moveSpeed * deltaTime); }

	if (input->IsMouseDown(MouseButton::RBUTTON))
	{
		mCamera.Yaw(input->GetMouseMoveX() * turnSpeed * deltaTime);
		mCamera.Pitch(input->GetMouseMoveY() * turnSpeed * deltaTime);
	}
}

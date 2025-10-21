#include "GameState.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;
using namespace IExeEngine::Input;


void GameState::Initialize()
{
	mCamera.SetPosition({ 0.0f, 1.0f, -3.0f });
	mCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });

	mDirectionalLight.direction = Math::Vector3{ 1.0f, -1.0f, 1.0f };
	mDirectionalLight.ambient = { 0.2f, 0.2f, 0.2f, 1.0f };
	mDirectionalLight.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	mDirectionalLight.specular = { 1.0f, 1.0f, 1.0f, 1.0f };
	Mesh mesh = MeshBuilder::CreateSphere(30,30,1.0f);
	mRenderObject.SpecMapId = tm->LoadTexture("earth_spec.jpg");
	mRenderObject.diffusemapID = tm->LoadTexture("earth_spec.jpg");
	mRenderObject.meshBuffer.Initialize(mesh);
	mRenderObject.normalMapId = tm->LoadTexture("earth_normal.jpg");
	mRenderObject.bumpMapId = tm->LoadTexture("earth_bump.jpg");

	std::filesystem::path shaderFile = "Assets/Shaders/Standard.fx";
	mStandardEffect.Initialize(shaderFile);
	mStandardEffect.setCamera(mCamera);
	mStandardEffect.SetDirectionalLight(mDirectionalLight);


}

void GameState::Terminate()
{
	mStandardEffect.Terminate();
	mRenderObject.meshBuffer.Terminate();
}

void GameState::Update(float deltaTime)
{
	UpdateCamera(deltaTime);
}

void GameState::Render()
{
	SimpleDraw::AddGroundPlane(20.0f, Colors::Wheat);
	SimpleDraw::Render(mCamera);

	mStandardEffect.Begin();
	mStandardEffect.Render(mRenderObject);
	mStandardEffect.End();
}

void GameState::DebugUI()
{
	ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::DragFloat3("DirectionalLight", &mDirectionalLight.direction.x, 0.01f))
		{

		}
	}

	ImGui::End();
}

void GameState::UpdateCamera(float deltaTime)
{
	// Camera Controls:
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
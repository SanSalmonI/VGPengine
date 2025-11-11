#include "GameState.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;
using namespace IExeEngine::Input;

enum class SphereRenderTarget
{
	Planet,
	Rock,
	Ground
};

const char* gSphereNames[] = {
	"Planet", "Rock", "Ground"
};

SphereRenderTarget gCurrentSphere = SphereRenderTarget::Planet;

void GameState::Initialize()
{
	mCamera.SetPosition({ 0.0f, 1.0f, -3.0f });
	mCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });

	mDirectionalLight.direction = Math::Normalize({ 1.0f, -1.0f, 1.0f });
	mDirectionalLight.ambient = { 0.4f, 0.4f, 0.4f, 1.0f };
	mDirectionalLight.diffuse = { 0.7f, 0.7f, 0.7f, 1.0f };
	mDirectionalLight.specular = { 0.9f, 0.9f, 0.9f, 1.0f };

	mCharacter.Initialize("Character01/Character01.model");

	Mesh groundMesh = MeshBuilder::CreatePlane(10.0f, 10.0f, 1);
	mGround.meshBuffer.Initialize(groundMesh);
	mGround.diffuseMapId = TextureManager::Get()->LoadTexture(L"space.jpg");

	MeshPX screenQuadMesh = MeshBuilder::CreateScreenQuadPX();
	mScreenQuad.meshBuffer.Initialize(screenQuadMesh);

	std::filesystem::path shaderFile = L"../../Assets/Shaders/Standard.fx";
	mStandardEffect.Initialize(shaderFile);
	mStandardEffect.SetCamera(mCamera);
	mStandardEffect.SetDirectionalLight(mDirectionalLight);

	shaderFile = L"../../Assets/Shaders/PostProcessing.fx";
	mPostProcessingEffect.Initialize(shaderFile);
	mPostProcessingEffect.SetTexture(&mRenderTarget);
	mPostProcessingEffect.SetTexture(&mCombineTexture, 1);

	GraphicsSystem* gs = GraphicsSystem::Get();
	const uint32_t screenWidth = gs->GetBackBufferWidth();
	const uint32_t screenHeight = gs->GetBackBufferHeight();
	mRenderTarget.Initialize(screenWidth, screenHeight, RenderTarget::Format::RGBA_U8);

	mCombineTexture.Initialize(L"../../Assets/Textures/water/water_texture.jpg");

}

void GameState::Terminate()
{
	mRenderTarget.Terminate();
	mScreenQuad.Terminate();
	mCombineTexture.Terminate();
	mPostProcessingEffect.Terminate();
	mGround.Terminate();
	mCharacter.Terminate();
	mStandardEffect.Terminate();
}

void GameState::Update(float deltaTime)
{
	UpdateCamera(deltaTime);
}

void GameState::Render()
{
	/*SimpleDraw::AddGroundPlane(20.0f, Colors::White);
	SimpleDraw::Render(mCamera);*/

	mRenderTarget.BeginRender();

	mStandardEffect.Begin();
	mStandardEffect.Render(mCharacter);
	mStandardEffect.Render(mGround);
	mStandardEffect.End();
	mRenderTarget.EndRender();

	mPostProcessingEffect.Begin();
	mPostProcessingEffect.Render(mScreenQuad);
	mPostProcessingEffect.End();
	
}

void GameState::DebugUI()
{

	ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
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

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (uint32_t i = 0; i < mCharacter.renderObjects.size(); ++i)
		{
			Material& material = mCharacter.renderObjects[i].material;
			std::string renderObjectId = "RenderObject " + std::to_string(i);
			ImGui::PushID(renderObjectId.c_str());
			if (ImGui::CollapsingHeader(renderObjectId.c_str()))
			{
				ImGui::LabelText("label", "Material:");
				ImGui::ColorEdit4("Emissive#Material", &material.emissive.r);
				ImGui::ColorEdit4("Ambient#Material", &material.ambient.r);
				ImGui::ColorEdit4("Diffuse#Material", &material.diffuse.r);
				ImGui::ColorEdit4("Specular#Material", &material.specular.r);
				ImGui::DragFloat("Shininess#Material", &material.shininess, 0.1f, 0.1f, 10000.0f);
			}
			ImGui::PopID();
		}
	}

	ImGui::Separator();
	ImGui::Text("Render Targer");
	ImGui::Image(mRenderTarget.GetRawData(),
		{128,128},
		{0,0},
		{1,1},
		{1,1,1,1},
		{ 1,1,1,1 });
	mStandardEffect.DebugUI();
	mPostProcessingEffect.DebugUI();
	ImGui::End();
}

void GameState::UpdateCamera(float deltaTime)
{
	Input::InputSystem* input = Input::InputSystem::Get();
	const float moveSpeed = input->IsKeyDown(Input::KeyCode::LSHIFT) ? 10.0f : 1.0f;
	const float turnSpeed = 1.0f;

	if (input->IsKeyDown(Input::KeyCode::W)) mCamera.Walk(moveSpeed * deltaTime);
	else if (input->IsKeyDown(Input::KeyCode::S)) mCamera.Walk(-moveSpeed * deltaTime);

	if (input->IsKeyDown(Input::KeyCode::A)) mCamera.Strafe(-moveSpeed * deltaTime);
	else if (input->IsKeyDown(Input::KeyCode::D)) mCamera.Strafe(moveSpeed * deltaTime);

	if (input->IsKeyDown(Input::KeyCode::Q)) mCamera.Rise(-moveSpeed * deltaTime);
	else if (input->IsKeyDown(Input::KeyCode::E)) mCamera.Rise(moveSpeed * deltaTime);

	if (input->IsMouseDown(MouseButton::RBUTTON))
	{
		mCamera.Yaw(input->GetMouseMoveX() * turnSpeed * deltaTime);
		mCamera.Pitch(input->GetMouseMoveY() * turnSpeed * deltaTime);
	}

}
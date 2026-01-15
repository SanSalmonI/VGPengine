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

	// sphere 1
	Mesh planet = MeshBuilder::CreateSphere(30, 30, 1.0f);
	mRenderObject.meshBuffer.Initialize(planet);
	TextureManager* tm = TextureManager::Get();
	mRenderObject.diffuseMapId = tm->LoadTexture(L"earth.jpg");


	std::filesystem::path shaderFile = L"../../Assets/Shaders/Standard.fx";
	mStandardEffect.Initialize(shaderFile);
	mStandardEffect.SetCamera(mCamera);
	mStandardEffect.SetDirectionalLight(mDirectionalLight);

	mAnimationTime = 0.0f;
	mAnimation = AnimationBuilder()
		.AddPositionKey({ 0.0f, 0.0f, 0.0f }, 0.0f)
		.AddPositionKey({ 0.0f, 2.0f, 0.0f }, 3.0f)
		.AddPositionKey({ 0.0f, 0.0f, 0.0f }, 5.0f)
		.AddRotationKey(Math::Quaternion::Identity, 0.0f)
		.AddRotationKey(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, Math::Constants::DegToRad), 3.0f)
		.AddRotationKey(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, Math::Constants::DegToRad), 4.0f)
		.AddRotationKey(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, Math::Constants::DegToRad), 5.0f)
		.AddScaleKey(Math::Vector3::One, 0.0f)
		.AddScaleKey(Math::Vector3::One * 0.1f, 2.5f)
		.AddScaleKey(Math::Vector3::One, 5.0f)
		.Build();
}

void GameState::Terminate()
{
	mRenderObject.Terminate();
	mStandardEffect.Terminate();
}

void GameState::Update(float deltaTime)
{
	UpdateCamera(deltaTime);

	mAnimationTime += deltaTime;
	while (mAnimationTime > mAnimation.GetDuration())
	{
		mAnimationTime -= mAnimation.GetDuration();
	}
}

void GameState::Render()
{
	mRenderObject.transform = mAnimation.GetTransform(mAnimationTime);
	SimpleDraw::AddGroundPlane(20.0f, Colors::White);
	SimpleDraw::Render(mCamera);

	mStandardEffect.Begin();
	if (gCurrentSphere == SphereRenderTarget::Planet)
	{
		mStandardEffect.Render(mRenderObject);
	}
	mStandardEffect.End();
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

	// Choose sphere to render and edit
	ImGui::Text("Choose sphere to render and edit");
	int currentSphere = (int)gCurrentSphere;
	if (ImGui::Combo("Sphere", &currentSphere, gSphereNames, std::size(gSphereNames)))
	{
		gCurrentSphere = static_cast<SphereRenderTarget>(currentSphere);
	}

	switch (gCurrentSphere)
	{
	case SphereRenderTarget::Planet:
		if (ImGui::CollapsingHeader("Material_Of_Planet", ImGuiTreeNodeFlags_DefaultOpen))
		{

			ImGui::ColorEdit4("Emissive#Material", &mRenderObject.material.emissive.r);
			ImGui::ColorEdit4("Ambient#Material", &mRenderObject.material.ambient.r);
			ImGui::ColorEdit4("Diffuse#Material", &mRenderObject.material.diffuse.r);
			ImGui::ColorEdit4("Specular#Material", &mRenderObject.material.specular.r);
			ImGui::DragFloat("Shininess#Material", &mRenderObject.material.shininess, 0.01f, 0.01f, 10000.0f);
		}
		break;
	default:
		break;
	}

	mStandardEffect.DebugUI();
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
#include "GameState.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;
using namespace IExeEngine::Input;
using namespace IExeEngine::Physics;


void GameState::Initialize()
{
	mCamera.SetPosition({ 0.0f, 1.0f, -3.0f });
	mCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });

	mDirectionalLight.direction = Math::Normalize({ 1.0f, -1.0f, 1.0f });
	mDirectionalLight.ambient = { 0.4f, 0.4f, 0.4f, 1.0f };
	mDirectionalLight.diffuse = { 0.7f, 0.7f, 0.7f, 1.0f };
	mDirectionalLight.specular = { 0.9f, 0.9f, 0.9f, 1.0f };

	// sphere 1
	Mesh mesh = MeshBuilder::CreateSphere(30, 30, 1.0f);
	mBallObject.meshBuffer.Initialize(mesh);
	TextureManager* tm = TextureManager::Get();
	mBallObject.diffuseMapId = tm->LoadTexture(L"misc/Basketball.jpg");
	mBallObject.transform.position.y = 1.0f;

	Mesh plane = MeshBuilder::CreatePlane(30, 30, 0.5f);
	mGroundObject.meshBuffer.Initialize(plane);
	mGroundObject.diffuseMapId = tm->LoadTexture(L"misc/concrete.png");


	std::filesystem::path shaderFile = L"../../Assets/Shaders/Standard.fx";
	mStandardEffect.Initialize(shaderFile);
	mStandardEffect.SetCamera(mCamera);
	mStandardEffect.SetDirectionalLight(mDirectionalLight);

	Mesh boxShape = MeshBuilder::CreateCube(1.0f);
	TextureId boxTexture = tm->LoadTexture(L"earth.jpg");

	float yOffset = 4.5f;
	float xOffset = 0.0f;
	int rowCount = 1;
	int boxIndex = 0;
	while (boxIndex < mBoxes.size())
	{
		xOffset = -((static_cast<float>(rowCount) - 1.0f) * 0.5f);
		for (int r = 0; r < rowCount; r++) {
		
			BoxData& boxData = mBoxes[boxIndex];
			boxData.box.meshBuffer.Initialize(boxShape);
			boxData.box.diffuseMapId = boxTexture;
			boxData.box.transform.position.x = xOffset;
			boxData.box.transform.position.y = yOffset;
			boxData.box.transform.position.z = 4.0f;
			boxData.shape.InitializeBox({ 0.5f, 0.5f, 0.5f });
			xOffset += 1.0f;
			++boxIndex;
		}
		yOffset -= 1.0f;
		rowCount += 1;
	}
	for (int i = mBoxes.size() - 1; i >= 0; --i)
	{
		mBoxes[i].rigidBody.Initialize(mBoxes[i].box.transform, mBoxes[i].shape, 1.0f);
	}

	int rows = 20;
	int columns = 20;
	mClothMesh = MeshBuilder::CreatePlane(rows, columns, 0.5f);
	for (Graphics::Vertex& vertex : mClothMesh.vertices)
	{
		vertex.position.y += 10.0f;
	}

	uint32_t lastVertex = mClothMesh.vertices.size() - 1;
	uint32_t lastVertexOS = lastVertex - columns;
	mClothSoftBody.Initialize(mClothMesh, 1.0f, { lastVertex, lastVertexOS });
	mCloth.meshBuffer.Initialize(nullptr, sizeof(Vertex), mClothMesh.vertices.size());
	mCloth.diffuseMapId = tm->LoadTexture(L"planets/venus.jpg");

}

void GameState::Terminate()
{
	mRenderObject.Terminate();
	mStandardEffect.Terminate();

	mCloth.meshBuffer.Terminate();
	

}

void GameState::Update(float deltaTime)
{
	UpdateCamera(deltaTime);

	if (InputSystem::Get()->IsKeyPressed(Input::KeyCode::SPACE))
	{
		Math::Vector3 spawnPos = mCamera.GetPosition() + (mCamera.GetDirection() * 0.5f);
		mBallRigidBody.SetPosition(spawnPos);
		mBallRigidBody.SetVelocity(mCamera.GetDirection() * 20.0f);

	}
}

void GameState::Render()
{
	mCloth MeshBuffer.Update(mClothMesh.vertices.data(), mClothMesh.vertices.size());
		mStandardEffect.Begin();
		mStandardEffect.SetCamera(mCamera);
		mStandardEffect.Render(mRenderObject);
		mStandardEffect.Render(mGroundObject);
	

	for (BoxData& boxData : mBoxes)
	{
		mStandardEffect.Render(boxData.box);
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

	mStandardEffect.DebugUI();
	PhysicsWorld::Get()->DebugUI();
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
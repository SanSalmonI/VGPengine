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

	IExeEngine::Graphics::RenderObject mGroundObject;
	IExeEngine::Graphics::RenderObject mRenderObject;
	IExeEngine::Graphics::RenderObject mBallObject;
	IExeEngine::Physics::RigidBody mBallRigidBody;
	IExeEngine::Physics::CollisionShape mBallShape;

	struct BoxData
	{
		IExeEngine::Graphics::RenderObject box;
		IExeEngine::Physics::RigidBody rigidBody;
		IExeEngine::Physics::CollisionShape shape;
	};

	using Boxes = std::vector<BoxData>;
	Boxes mBoxes;

	IExeEngine::Graphics::StandardEffect mStandardEffect;

	//cloth
	IExeEngine::Graphics::RenderObject mCloth;
	IExeEngine::Graphics::Mesh mClothMesh;
	IExeEngine::Physics::SoftBody mClothSoftBody;


};
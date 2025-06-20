#include "GameState.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;
using namespace IExeEngine::Input;

enum class PlanetRenderTarget
{
	Sun,
	Mercury,
	Venus,
	Earth,
	Moon,
	Mars,
	Jupiter,
	Saturn,
	Uranus,
	Neptune,
	Pluto
};
PlanetRenderTarget gCurrentPlanet;

const char* gPlanetNames[] = 
{
	"Sun", "Mercury", "Venus", "Earth", "Moon", "Mars",
	"Jupiter", "Saturn", "Uranus", "Neptune", "Pluto"
};

void GameState::Initialize()
{
	// Basic camera
	mCamera.SetPosition({ 0.0f, 50.0f, -150.0f });
	mCamera.SetLookAt({ 0.0f, 0.0f,  0.0f });
	mRenderTargetCamera.SetPosition({ 0.0f, 50.0f, -150.0f });
	mRenderTargetCamera.SetLookAt({ 0.0f, 0.0f,  0.0f });
	mRenderTargetCamera.SetAspectRatio(1.0f);

	mRenderTarget.Initialize(512, 512, RenderTarget::Format::RGBA_U32);
	// SimpleTexturing effect
	mSimpleTextureEffect.Initialize();

	// ----- space sky sphere -------------------------------------------------
	MeshPX sky = MeshBuilder::CreateSkySpherePX(60, 60, 250.0f);
	mSpace.mesh.Initialize(sky);
	mSpace.textureId = TextureManager::Get()->LoadTexture(L"space.jpg");

	// ----- Sun --------------------------------------------------------------
	MeshPX sunSphere = MeshBuilder::CreateSpherePX(60, 60, 5.0f);
	mSun.mesh.Initialize(sunSphere);
	mSun.textureId = TextureManager::Get()->LoadTexture(L"planets/sun.jpg");

	// ----- Mercury ----------------------------------------------------------
	mMercury.radius = 0.3f;     // size
	mMercury.distanceFromCenter = 16.0f;     // orbit radius
	mMercury.orbitTranslation = 0.24f; // orbit translation speed
	mMercury.rotationOnAxis = 1.0f;    // self-rotation

	MeshPX mercurySphere =
		MeshBuilder::CreateSpherePX(60, 60, mMercury.radius);

	mMercury.renderData.mesh.Initialize(mercurySphere);
	mMercury.renderData.textureId =
		TextureManager::Get()->LoadTexture(L"planets/mercury.jpg");

	mMercury.renderData.matWorld =
		Math::Matrix4::Translation(mMercury.distanceFromCenter, 0.0f, 0.0f);

	//----- Venus ------------------------------------------------------------
	mVenus.radius = 0.5f;     // size 
	// orbit parameters (tweak to taste)
	mVenus.distanceFromCenter = 30.0f;    // distance from Sun
	mVenus.orbitTranslation = 0.19f;    // orbital speed
	mVenus.rotationOnAxis = 1.0f;     // self-rotation

	MeshPX venusSphere =
		MeshBuilder::CreateSpherePX(60, 60, mVenus.radius);

	mVenus.renderData.mesh.Initialize(venusSphere);
	mVenus.renderData.textureId =
		TextureManager::Get()->LoadTexture(L"planets/venus.jpg");

	// start “in front” of the Sun
	mVenus.renderData.matWorld =
		Math::Matrix4::Translation(mVenus.distanceFromCenter, 0.0f, 0.0f);

	// ----- Earth -----------------------------------------------------------

	mEarth.radius = 0.5f;      // size
	mEarth.distanceFromCenter = 36.0f;     // orbit radius
	mEarth.orbitTranslation = 0.16f;     // orbital speed
	mEarth.rotationOnAxis = 1.0f;      // self-rotation

	MeshPX earthSphere =
		MeshBuilder::CreateSpherePX(60, 60, mEarth.radius);

	mEarth.renderData.mesh.Initialize(earthSphere);
	mEarth.renderData.textureId =
		TextureManager::Get()->LoadTexture(L"planets/earth.jpg");

	// start “in front” of the Sun
	mEarth.renderData.matWorld =
		Math::Matrix4::Translation(mEarth.distanceFromCenter, 0.0f, 0.0f);

	// ----- Mars ------------------------------------------------------------
	mMars.radius = 0.48f;
	mMars.distanceFromCenter = 42.0f;     // orbit radius from Sun
	mMars.orbitTranslation = 0.13f;     // orbital speed
	mMars.rotationOnAxis = 1.0f;      // self-rotation

	MeshPX marsSphere =
		MeshBuilder::CreateSpherePX(60, 60, mMars.radius);

	mMars.renderData.mesh.Initialize(marsSphere);
	mMars.renderData.textureId =
		TextureManager::Get()->LoadTexture(L"planets/mars.jpg");

	// aesthetic start position
	mMars.renderData.matWorld =
		Math::Matrix4::Translation(mMars.distanceFromCenter, 0.0f, 0.0f);

	// ----- Moon ---------------------------------------------------------
	mMoon.radius = 0.10f;
	mMoon.distanceFromCenter = 1.0f;      // orbit radius from Earth
	mMoon.orbitTranslation = 0.33f;      // orbital speed
	mMoon.rotationOnAxis = 1.0f;       // self-rotation

	mMoon.centerObject = mEarth.renderData.matWorld;

	MeshPX moonSphere =
		MeshBuilder::CreateSpherePX(60, 60, mMoon.radius);

	mMoon.renderData.mesh.Initialize(moonSphere);
	mMoon.renderData.textureId =
		TextureManager::Get()->LoadTexture(L"planets/moon.jpg");

	mMoon.renderData.matWorld =
		Math::Matrix4::Translation(mEarth.distanceFromCenter + mMoon.distanceFromCenter,
			0.0f,
			0.0f);

	// ----- Jupiter --------------------------------------------------------
	mJupiter.radius = 2.0f;
	mJupiter.distanceFromCenter = 51.0f;     // orbit radius
	mJupiter.orbitTranslation = 0.10f;     // orbital speed
	mJupiter.rotationOnAxis = 1.0f;      // self-rotation

	MeshPX jupiterSphere =
		MeshBuilder::CreateSpherePX(60, 60, mJupiter.radius);

	mJupiter.renderData.mesh.Initialize(jupiterSphere);
	mJupiter.renderData.textureId =
		TextureManager::Get()->LoadTexture(L"planets/jupiter.jpg");

	// start on +X side of the Sun
	mJupiter.renderData.matWorld =
		Math::Matrix4::Translation(mJupiter.distanceFromCenter, 0.0f, 0.0f);

	// ----- Saturn ---------------------------------------------------------
	mSaturn.radius = 1.75f;   // size (with rings the mesh is wider)
	mSaturn.distanceFromCenter = 72.0f;
	mSaturn.orbitTranslation = 0.08f;   // orbital speed (feel-free tweak)
	mSaturn.rotationOnAxis = 1.0f;    // self-rotation


	MeshPX saturnObj =
		MeshBuilder::CreateOBJPX(
			L"../../Assets/Models/Planets/Saturn/Saturn.obj", 0.5f);
	mSaturn.renderData.mesh.Initialize(saturnObj); 


	/*MeshPX saturnSphere =
		MeshBuilder::CreateSpherePX(60, 60, mSaturn.radius);
	mSaturn.renderData.mesh.Initialize(saturnSphere);*/


	mSaturn.renderData.textureId =
		TextureManager::Get()->LoadTexture(L"planets/saturn.jpg");

	// start out on +X
	mSaturn.renderData.matWorld =
		Math::Matrix4::Translation(mSaturn.distanceFromCenter, 0.0f, 0.0f);

	// ----- Uranus ---------------------------------------------------------
	mUranus.radius = 0.70f;   // size
	mUranus.distanceFromCenter = 87.0f;   
	mUranus.orbitTranslation = 0.06f;   // orbital speed
	mUranus.rotationOnAxis = 1.0f;    // self-rotation

	MeshPX uranusSphere =
		MeshBuilder::CreateSpherePX(60, 60, mUranus.radius);

	mUranus.renderData.mesh.Initialize(uranusSphere);
	mUranus.renderData.textureId =
		TextureManager::Get()->LoadTexture(L"planets/uranus.jpg");

	// start on +X side of the Sun
	mUranus.renderData.matWorld =
		Math::Matrix4::Translation(mUranus.distanceFromCenter, 0.0f, 0.0f);

	// ----- Neptune --------------------------------------------------------
	mNeptune.radius = 0.70f;   // size
	mNeptune.distanceFromCenter = 96.0f;   
	mNeptune.orbitTranslation = 0.05f;   // orbital speed
	mNeptune.rotationOnAxis = 1.0f;    // self-rotation

	MeshPX neptuneSphere =
		MeshBuilder::CreateSpherePX(60, 60, mNeptune.radius);

	mNeptune.renderData.mesh.Initialize(neptuneSphere);
	mNeptune.renderData.textureId =
		TextureManager::Get()->LoadTexture(L"planets/neptune.jpg");

	// start on +X side of the Sun
	mNeptune.renderData.matWorld =
		Math::Matrix4::Translation(mNeptune.distanceFromCenter, 0.0f, 0.0f);

	// ----- Pluto ----------------------------------------------------------
	mPluto.radius = 0.20f;   // size
	mPluto.distanceFromCenter = 120.0f;
	mPluto.orbitTranslation = 0.04f;   // orbital speed
	mPluto.rotationOnAxis = 1.0f;    // self-rotation

	MeshPX plutoSphere =
		MeshBuilder::CreateSpherePX(60, 60, mPluto.radius);

	mPluto.renderData.mesh.Initialize(plutoSphere);
	mPluto.renderData.textureId =
		TextureManager::Get()->LoadTexture(L"planets/pluto.jpg");

	mPluto.renderData.matWorld =
		Math::Matrix4::Translation(mPluto.distanceFromCenter, 0.0f, 0.0f);
	
}


void GameState::Terminate()
{
	mRenderTarget.Terminate();
	TextureManager::Get()->ReleaseTexture(mSpace.textureId);
	TextureManager::Get()->ReleaseTexture(mSun.textureId);
	TextureManager::Get()->ReleaseTexture(mMercury.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mVenus.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mEarth.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mMoon.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mMars.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mJupiter.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mSaturn.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mUranus.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mNeptune.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mPluto.renderData.textureId);
    TextureManager::Get()->ReleaseTexture(mAsteroid_01.renderData.textureId);
    TextureManager::Get()->ReleaseTexture(mAsteroid_02.renderData.textureId);

	mSpace.mesh.Terminate();
	mSun.mesh.Terminate();
	mMercury.renderData.mesh.Terminate();
	mVenus.renderData.mesh.Terminate();
	mEarth.renderData.mesh.Terminate();
	mMoon.renderData.mesh.Terminate();
	mMars.renderData.mesh.Terminate();
	mJupiter.renderData.mesh.Terminate();
	mSaturn.renderData.mesh.Terminate();
	mUranus.renderData.mesh.Terminate();
	mNeptune.renderData.mesh.Terminate();
	mPluto.renderData.mesh.Terminate();

    mAsteroid_01.renderData.mesh.Terminate();
    mAsteroid_02.renderData.mesh.Terminate();

	mSimpleTextureEffect.Terminate();
}

Math::Vector3 GetObjectPosition(Math::Matrix4& centerObject)
{
	return Math::Vector3(
		centerObject._41, // ( X, 
		centerObject._42, //   Y, 
		centerObject._43  //   Z ) 
	);
}

void GameState::Update(float deltaTime)
{
	UpdateCamera(deltaTime);

	UpdatePlanet(mMercury, deltaTime);
	UpdatePlanet(mVenus, deltaTime);
	UpdatePlanet(mEarth, deltaTime);
	UpdateMoon(mMoon, mEarth, deltaTime);
	UpdatePlanet(mMars, deltaTime);
	UpdatePlanet(mJupiter, deltaTime);
	UpdatePlanet(mSaturn, deltaTime);
	UpdatePlanet(mUranus, deltaTime);
	UpdatePlanet(mNeptune, deltaTime);
	UpdatePlanet(mPluto, deltaTime);

    // Update Render Target Camera to follow the selected planet using ImGui combo box selection
	if (gCurrentPlanet == PlanetRenderTarget::Sun)
	{
		UpdateRenderTargetCamera(deltaTime, Math::Vector3::Zero, 11);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Mercury)
	{
		UpdateRenderTargetCamera(deltaTime, GetObjectPosition(mMercury.renderData.matWorld), mMercury.radius + 1.0f);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Venus)
	{
		UpdateRenderTargetCamera(deltaTime, GetObjectPosition(mVenus.renderData.matWorld), mVenus.radius + 1.0f);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Earth)
	{
		UpdateRenderTargetCamera(deltaTime, GetObjectPosition(mEarth.renderData.matWorld), mEarth.radius + 1.0f);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Moon)
	{
		UpdateRenderTargetCamera(deltaTime, GetObjectPosition(mMoon.renderData.matWorld), mMoon.radius + 1.0f);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Mars)
	{
		UpdateRenderTargetCamera(deltaTime, GetObjectPosition(mMars.renderData.matWorld), mMars.radius + 1.0f);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Jupiter)
	{
		UpdateRenderTargetCamera(deltaTime, GetObjectPosition(mJupiter.renderData.matWorld), mJupiter.radius + 2.0f);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Saturn)
	{
		UpdateRenderTargetCamera(deltaTime, GetObjectPosition(mSaturn.renderData.matWorld), mSaturn.radius + 2.0f);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Uranus)
	{
		UpdateRenderTargetCamera(deltaTime, GetObjectPosition(mUranus.renderData.matWorld), mUranus.radius + 1.0f);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Neptune)
	{
		UpdateRenderTargetCamera(deltaTime, GetObjectPosition(mNeptune.renderData.matWorld), mNeptune.radius + 1.0f);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Pluto)
	{
		UpdateRenderTargetCamera(deltaTime, GetObjectPosition(mPluto.renderData.matWorld), mPluto.radius + 1.0f);
	}
}

void GameState::Render()
{
	SimpleDraw::Render(mCamera);

	// Render to Render Target ImGui Image
	mSimpleTextureEffect.SetCamera(mRenderTargetCamera);
	mRenderTarget.BeginRender();
	mSimpleTextureEffect.Begin();
	if (gCurrentPlanet == PlanetRenderTarget::Sun)
	{
		mSimpleTextureEffect.Render(mSun);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Mercury)
	{
		mSimpleTextureEffect.Render(mMercury.renderData);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Venus)
	{
		mSimpleTextureEffect.Render(mVenus.renderData);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Earth)
	{
		mSimpleTextureEffect.Render(mEarth.renderData);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Moon)
	{
		mSimpleTextureEffect.Render(mMoon.renderData);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Mars)
	{
		mSimpleTextureEffect.Render(mMars.renderData);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Jupiter)
	{
		mSimpleTextureEffect.Render(mJupiter.renderData);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Saturn)
	{
		mSimpleTextureEffect.Render(mSaturn.renderData);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Uranus)
	{
		mSimpleTextureEffect.Render(mUranus.renderData);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Neptune)
	{
		mSimpleTextureEffect.Render(mNeptune.renderData);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Pluto)
	{
		mSimpleTextureEffect.Render(mPluto.renderData);
	}
	mSimpleTextureEffect.End();
	mRenderTarget.EndRender();

	// Render to Scene
	mSimpleTextureEffect.SetCamera(mCamera);
	mSimpleTextureEffect.Begin();

	mSimpleTextureEffect.Render(mSpace);
	mSimpleTextureEffect.Render(mSun);
	mSimpleTextureEffect.Render(mMercury.renderData);
	mSimpleTextureEffect.Render(mVenus.renderData);
	mSimpleTextureEffect.Render(mEarth.renderData);
	mSimpleTextureEffect.Render(mMoon.renderData);
	mSimpleTextureEffect.Render(mMars.renderData);
	mSimpleTextureEffect.Render(mJupiter.renderData);
	mSimpleTextureEffect.Render(mSaturn.renderData);
	mSimpleTextureEffect.Render(mUranus.renderData);
	mSimpleTextureEffect.Render(mNeptune.renderData);
	mSimpleTextureEffect.Render(mPluto.renderData);


	mSimpleTextureEffect.End();
}


void GameState::DebugUI()
{
	ImGui::Begin("ImGui Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Solar System Settings:");

	ImGui::Checkbox("Show Orbit Rings", &orbitRings);
	if (orbitRings)
	{
		SimpleDraw::AddGroundCircle(60, mMercury.distanceFromCenter, Colors::White, Math::Vector3::Zero);
		SimpleDraw::AddGroundCircle(60, mVenus.distanceFromCenter, Colors::White, Math::Vector3::Zero);
		SimpleDraw::AddGroundCircle(60, mEarth.distanceFromCenter, Colors::White, Math::Vector3::Zero);
		SimpleDraw::AddGroundCircle(60, mMars.distanceFromCenter, Colors::White, Math::Vector3::Zero);
		SimpleDraw::AddGroundCircle(60, mJupiter.distanceFromCenter, Colors::White, Math::Vector3::Zero);
		SimpleDraw::AddGroundCircle(60, mSaturn.distanceFromCenter, Colors::White, Math::Vector3::Zero);
		SimpleDraw::AddGroundCircle(60, mUranus.distanceFromCenter, Colors::White, Math::Vector3::Zero);
		SimpleDraw::AddGroundCircle(60, mNeptune.distanceFromCenter, Colors::White, Math::Vector3::Zero);
		SimpleDraw::AddGroundCircle(60, mPluto.distanceFromCenter, Colors::White, Math::Vector3::Zero);
	}

	ImGui::Text("Choose Planet to Render");
	int currentPlanet = (int)gCurrentPlanet;
	if (ImGui::Combo("Planet", &currentPlanet, gPlanetNames, std::size(gPlanetNames)))
	{
		gCurrentPlanet = static_cast<PlanetRenderTarget>(currentPlanet);
	}

	switch (gCurrentPlanet)
	{
	case PlanetRenderTarget::Sun:
		ImGui::Text("Sun");

		break;
	case PlanetRenderTarget::Mercury:
		ImGui::Text("Mercury");
		ImGui::DragFloat("Orbit Translation Speed", &mMercury.orbitTranslation, 0.01f, 0.0f, 10.0f);
		break;
	case PlanetRenderTarget::Venus:
		ImGui::Text("Venus");
		ImGui::DragFloat("Orbit Translation Speed", &mVenus.orbitTranslation, 0.01f, 0.0f, 10.0f);
		break;
	case PlanetRenderTarget::Earth:
		ImGui::Text("Earth");
		ImGui::DragFloat("Orbit Translation Speed", &mEarth.orbitTranslation, 0.01f, 0.0f, 10.0f);
		break;
	case PlanetRenderTarget::Moon:
		ImGui::Text("Moon");
		ImGui::DragFloat("Orbit Translation Speed", &mMoon.orbitTranslation, 0.01f, 0.0f, 10.0f);
		break;
	case PlanetRenderTarget::Mars:
		ImGui::Text("Mars");
		ImGui::DragFloat("Orbit Translation Speed", &mMars.orbitTranslation, 0.01f, 0.0f, 10.0f);
		break;
	case PlanetRenderTarget::Jupiter:
		ImGui::Text("Jupiter");
		ImGui::DragFloat("Orbit Translation Speed", &mJupiter.orbitTranslation, 0.01f, 0.0f, 10.0f);
		break;
	case PlanetRenderTarget::Saturn:
		ImGui::Text("Saturn");
		ImGui::DragFloat("Orbit Translation Speed", &mSaturn.orbitTranslation, 0.01f, 0.0f, 10.0f);
		break;
	case PlanetRenderTarget::Uranus:
		ImGui::Text("Uranus");
		ImGui::DragFloat("Orbit Translation Speed", &mUranus.orbitTranslation, 0.01f, 0.0f, 10.0f);
		break;
	case PlanetRenderTarget::Neptune:
		ImGui::Text("Neptune");
		ImGui::DragFloat("Orbit Translation Speed", &mNeptune.orbitTranslation, 0.01f, 0.0f, 10.0f);
		break;
	case PlanetRenderTarget::Pluto:
		ImGui::Text("Pluto");
		ImGui::DragFloat("Orbit Translation Speed", &mPluto.orbitTranslation, 0.01f, 0.0f, 10.0f);
		break;
	default:
		break;
	}

	ImGui::Separator();
	ImGui::Text("Render Target Camera");
	ImGui::Image(
		mRenderTarget.GetRawData(),
		{ 400, 400 },
		{ 0, 0 },
		{ 1, 1 },
		{ 1, 1, 1, 1 },
		{ 1, 1, 1, 1 });

	ImGui::End();
}

void GameState::UpdateCamera(float deltaTime)
{
	Input::InputSystem* input = Input::InputSystem::Get();
	const float moveSpeed = input->IsKeyDown(Input::KeyCode::LSHIFT) ? 50.0f : 10.0f;
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

//-------------------------------------- Functions to Animate Planets & Moon: ------------------------------------------------------------------

void GameState::UpdatePlanet(Planet& planet, float deltaTime)
{
    // Update orbit and self-rotation angles
	planet.orbitAngle += planet.rotationOnAxis * deltaTime;
	if (planet.orbitAngle >= Math::Constants::TwoPi)
		planet.orbitAngle -= Math::Constants::TwoPi;

    // Update self-rotation angle
	planet.selfRotationAngle += planet.orbitTranslation * deltaTime;
	if (planet.selfRotationAngle >= Math::Constants::TwoPi)
		planet.selfRotationAngle -= Math::Constants::TwoPi;

    // Calculate the transformation matrices
	const auto orbitRotation = Math::Matrix4::RotationY(planet.orbitAngle);
	const auto orbitTranslation = Math::Matrix4::Translation(planet.distanceFromCenter, 0.0f, 0.0f);
	const auto orbitTransform = orbitRotation * orbitTranslation;

	const auto selfRotation = Math::Matrix4::RotationY(planet.selfRotationAngle);

    // Final transformation: rotate on self -> orbit around the Sun -> use Sun's position only
	planet.renderData.matWorld = orbitTransform * planet.centerObject * selfRotation;
}

void GameState::UpdateMoon(Planet& moon, const Planet& earth, float deltaTime)
{
    // Update orbit and self-rotation angles for the moon
	moon.orbitAngle += moon.orbitTranslation * deltaTime;
	if (moon.orbitAngle >= Math::Constants::TwoPi)
		moon.orbitAngle -= Math::Constants::TwoPi;
    // Self-rotation angle for the moon
	moon.selfRotationAngle += moon.rotationOnAxis * deltaTime;
	if (moon.selfRotationAngle >= Math::Constants::TwoPi)
		moon.selfRotationAngle -= Math::Constants::TwoPi;

    // Calculate the transformation matrices for the moon
	auto orbitRotation = Math::Matrix4::RotationY(moon.orbitAngle);
	auto orbitTranslation = Math::Matrix4::Translation(moon.distanceFromCenter, 0.0f, 0.0f);
	auto orbitTransform = orbitTranslation * orbitRotation;

    // Self-rotation matrix for the moon
	auto selfRotation = Math::Matrix4::RotationY(moon.selfRotationAngle);

    // Final transformation: rotate on self -> orbit around the Earth -> use Earth's position
	moon.renderData.matWorld = selfRotation * orbitTransform * earth.renderData.matWorld;
}

void GameState::UpdateRenderTargetCamera(float deltaTime, Math::Vector3 targetPosition, float objectRadius)
{
    // Update the render target camera position based on the selected planet
	if (gCurrentPlanet == PlanetRenderTarget::Sun)
	{
		mRenderTargetCamera.SetPosition({ 0.0f, 1.0f, -10.0f });
		mRenderTargetCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });
	}
	else
	{
		mRenderTargetCamera.SetPosition(targetPosition + Math::Vector3(0.0f, 1.0f, -objectRadius * 1.2f));
		mRenderTargetCamera.SetLookAt(targetPosition);
	}
}
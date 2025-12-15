#include "GameState.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;
using namespace IExeEngine::Input;

namespace
{
    // Helper to render a full screen quad with explicit shader binding
    void RenderQuad(const RenderObject& quad, VertexShader& vs, PixelShader& ps)
    {
        vs.Bind();
        ps.Bind();
        quad.meshBuffer.Render();
    }
}

void GameState::Initialize()
{
    GraphicsSystem* gs = GraphicsSystem::Get();
    const uint32_t screenWidth = gs->GetBackBufferWidth();
    const uint32_t screenHeight = gs->GetBackBufferHeight();

    // 1. Camera & Light
    mCamera.SetPosition({ 0.0f, 0.0f, -5.0f });
    mCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });

    mDirectionalLight.direction = Math::Normalize({ 1.0f, -1.0f, 1.0f });
    mDirectionalLight.ambient = { 0.2f, 0.2f, 0.2f, 1.0f };
    mDirectionalLight.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
    mDirectionalLight.specular = { 0.9f, 0.9f, 0.9f, 1.0f };

    // 2. Objects
    // Ground
    Mesh groundMesh = MeshBuilder::CreatePlane(20, 20, 1.0f);
    mGround.meshBuffer.Initialize(groundMesh);
    mGround.diffuseMapId = TextureManager::Get()->LoadTexture("HexGround/HexGround_Diff.jpg");
    mGround.transform.position.y = -2.0f;
    mGround.material.ambient = { 0.1f, 0.1f, 0.1f, 1.0f };
    mGround.material.diffuse = { 0.5f, 0.5f, 0.5f, 1.0f };

    // Opal Object (Sphere)
    Mesh opalMesh = MeshBuilder::CreateSphere(60, 60, 1.5f);
    mOpalObject.meshBuffer.Initialize(opalMesh);
    mOpalObject.diffuseMapId = TextureManager::Get()->LoadTexture("White.jpg"); 
    mOpalObject.material.ambient = { 0.1f, 0.1f, 0.1f, 1.0f };
    mOpalObject.material.diffuse = { 0.2f, 0.2f, 0.2f, 0.5f }; 
    mOpalObject.material.specular = { 0.9f, 0.9f, 0.9f, 1.0f };
    mOpalObject.material.shininess = 20.0f;
    mOpalObject.transform.position = { -3.0f, 1.5f, 0.0f };

    // Opal Object (Character)
    mCharacter.Initialize("Character01/Character01.model");
    mCharacter.transform.position = { 5.0f, 0.0f, 0.0f };
    mCharacter.transform.scale = { 10.0f, 10.0f, 10.0f }; 
    // mCharacter.transform.rotation...

    // Screen Quad
    MeshPX screenQuadMesh = MeshBuilder::CreateScreenQuadPX();
    mScreenQuad.meshBuffer.Initialize(screenQuadMesh);

    // 3. Effects
    std::filesystem::path standardShader = L"../../Assets/Shaders/Standard.fx";
    mStandardEffect.Initialize(standardShader);
    mStandardEffect.SetCamera(mCamera);
    mStandardEffect.SetDirectionalLight(mDirectionalLight);

    // 4. Render Targets
    mSceneRT.Initialize(screenWidth, screenHeight, RenderTarget::Format::RGBA_U8);
    mOpalMaskRT.Initialize(screenWidth, screenHeight, RenderTarget::Format::RGBA_U8); // Using RGBA8 as R8 fallback
    mOpalNormalRT.Initialize(screenWidth, screenHeight, RenderTarget::Format::RGBA_U8);
    mFireRT.Initialize(screenWidth, screenHeight, RenderTarget::Format::RGBA_U32); // High precision
    mSparkleRT.Initialize(screenWidth, screenHeight, RenderTarget::Format::RGBA_U32); // High precision

    // 5. Shaders
    // GBuffer Shaders
    std::filesystem::path opalGBufferFile = L"../../Assets/Shaders/OpalGBuffer.fx";
    mOpalMeshVS.Initialize<Vertex>(opalGBufferFile); // Uses Standard Vertex
    mOpalMaskPS.Initialize(opalGBufferFile, "PS_Mask");
    mOpalNormalPS.Initialize(opalGBufferFile, "PS_Normal");

    // Post Shaders
    std::filesystem::path opalPostFile = L"../../Assets/Shaders/OpalPost.fx";
    mScreenQuadVS.Initialize<VertexPX>(opalPostFile); // Uses VertexPX for Quad
    mOpalFirePS.Initialize(opalPostFile, "PS_OpalFire");
    mOpalSparklePS.Initialize(opalPostFile, "PS_SparkleGlints");
    mOpalCompositePS.Initialize(opalPostFile, "PS_Composite");

    // 6. Resources & Constants
    mNoiseTexture.Initialize("../../Assets/Textures/ShimmerDistortNoise.png"); 
    mScratchTexture.Initialize("../../Assets/Textures/Scratches.png");
    mSampler.Initialize(Sampler::Filter::Linear, Sampler::AddressMode::Wrap);
    mOpalParamsBuffer.Initialize();
    mTransformBuffer.Initialize();
}

void GameState::Terminate()
{
    mTransformBuffer.Terminate();
    mOpalParamsBuffer.Terminate();
    mSampler.Terminate();
    mScratchTexture.Terminate();
    mNoiseTexture.Terminate();
    
    mOpalCompositePS.Terminate();
    mOpalSparklePS.Terminate();
    mOpalFirePS.Terminate();
    mScreenQuadVS.Terminate();

    mOpalNormalPS.Terminate();
    mOpalMaskPS.Terminate();
    mOpalMeshVS.Terminate();

    mSparkleRT.Terminate();
    mFireRT.Terminate();
    mOpalNormalRT.Terminate();
    mOpalMaskRT.Terminate();
    mSceneRT.Terminate();

    mStandardEffect.Terminate();
    mScreenQuad.Terminate();
    mCharacter.Terminate();
    mOpalObject.Terminate();
    mGround.Terminate();
}

void GameState::Update(float deltaTime)
{
    UpdateCamera(deltaTime);
    
    
}

void GameState::Render()
{
    GraphicsSystem* gs = GraphicsSystem::Get();
    
    // Common transforms
    Math::Matrix4 view = mCamera.GetViewMatrix();
    Math::Matrix4 proj = mCamera.GetProjectionMatrix();
    
    // Update Constants
    mOpalParams.viewMatrix = Math::Transpose(view); // Send View Matrix (World->View logic)
    mOpalParamsBuffer.Update(mOpalParams);
    mOpalParamsBuffer.BindPS(0);


    // --- Pass 1: Scene Pass ---
    mSceneRT.BeginRender();
        mStandardEffect.Begin();
            mStandardEffect.Render(mGround);
            // mStandardEffect.Render(mOpalObject); // Disable opaque render so we can refract the background behind it
        mStandardEffect.End();
    mSceneRT.EndRender();

    // --- Pass 2: Mask Pass ---
    mOpalMaskRT.BeginRender(Colors::Black);
        mOpalMeshVS.Bind();
        mOpalMaskPS.Bind();
        
        // Render Sphere
        {
            TransformData td;
            Math::Matrix4 world = mOpalObject.transform.GetMatrix4();
            Math::Matrix4 wvp = world * view * proj;
            td.world = Math::Transpose(world);
            td.view = Math::Transpose(view);
            td.wvp = Math::Transpose(wvp);
            mTransformBuffer.Update(td);
            mTransformBuffer.BindVS(0);
            mOpalObject.meshBuffer.Render();
        }

        // Render Character
        {
            TransformData td;
            for (const auto& renderObject : mCharacter.renderObjects)
            {
                Math::Matrix4 world = renderObject.transform.GetMatrix4() * mCharacter.transform.GetMatrix4();
                Math::Matrix4 wvp = world * view * proj;
                
                td.world = Math::Transpose(world);
                td.view = Math::Transpose(view);
                td.wvp = Math::Transpose(wvp);
                mTransformBuffer.Update(td);
                mTransformBuffer.BindVS(0);
                renderObject.meshBuffer.Render();
            }
        }
    mOpalMaskRT.EndRender();

    // --- Pass 3: Normal Pass ---
    mOpalNormalRT.BeginRender(Colors::Black);
        mOpalMeshVS.Bind();
        mOpalNormalPS.Bind();
        
        mOpalNormalPS.Bind();
        
        // Render Sphere
        {
            TransformData td;
            Math::Matrix4 world = mOpalObject.transform.GetMatrix4();
            Math::Matrix4 wvp = world * view * proj;
            td.world = Math::Transpose(world);
            td.view = Math::Transpose(view);
            td.wvp = Math::Transpose(wvp);
            mTransformBuffer.Update(td);
            mTransformBuffer.BindVS(0);
            mOpalObject.meshBuffer.Render();
        }

        // Render Character
        {
            TransformData td;
            for (const auto& renderObject : mCharacter.renderObjects)
            {
                Math::Matrix4 world = renderObject.transform.GetMatrix4() * mCharacter.transform.GetMatrix4();
                Math::Matrix4 wvp = world * view * proj;
                
                td.world = Math::Transpose(world);
                td.view = Math::Transpose(view);
                td.wvp = Math::Transpose(wvp);
                mTransformBuffer.Update(td);
                mTransformBuffer.BindVS(0);
                renderObject.meshBuffer.Render();
            }
        }
    mOpalNormalRT.EndRender();

    // --- Pass 4: Fire ---
    mFireRT.BeginRender(Colors::Black);
        mSceneRT.BindPS(0); 
        mOpalMaskRT.BindPS(1); 
        mOpalNormalRT.BindPS(2); 
        mNoiseTexture.BindPS(3); // Match Composite Slot
        // mFireRT is t4 (Output, not input here)
        // mSparkle is t5
        mScratchTexture.BindPS(6); // Match Composite Slot
        mOpalParamsBuffer.BindPS(0);
       
        RenderQuad(mScreenQuad, mScreenQuadVS, mOpalFirePS);
        Texture::UnbindPS(0); Texture::UnbindPS(1); Texture::UnbindPS(2); 
        Texture::UnbindPS(3); Texture::UnbindPS(6);
    mFireRT.EndRender();

    // --- Pass 5: Sparkle ---
    mSparkleRT.BeginRender(Colors::Black);
        mSceneRT.BindPS(0);
        mOpalMaskRT.BindPS(1);
        mOpalNormalRT.BindPS(2);
        mNoiseTexture.BindPS(3);
        mSampler.BindPS(0);
        mOpalParamsBuffer.BindPS(0); // Rebind Params
        RenderQuad(mScreenQuad, mScreenQuadVS, mOpalSparklePS);
        Texture::UnbindPS(0); Texture::UnbindPS(1); Texture::UnbindPS(2); Texture::UnbindPS(3);
    mSparkleRT.EndRender();

    // --- Pass 6: Composite ---
    gs->BeginRender(); // Backbuffer
        mSceneRT.BindPS(0);
        mOpalMaskRT.BindPS(1);
        mOpalNormalRT.BindPS(2);
        mNoiseTexture.BindPS(3);
        mFireRT.BindPS(4);
        mSparkleRT.BindPS(5);
        mScratchTexture.BindPS(6);
        
        mSampler.BindPS(0);
        mOpalParamsBuffer.BindPS(0); // Rebind Params
        RenderQuad(mScreenQuad, mScreenQuadVS, mOpalCompositePS);
        Texture::UnbindPS(0); Texture::UnbindPS(1); Texture::UnbindPS(2); 
        Texture::UnbindPS(3); Texture::UnbindPS(4); Texture::UnbindPS(5);
        Texture::UnbindPS(6);

        Texture::UnbindPS(3); Texture::UnbindPS(4); Texture::UnbindPS(5);
    gs->EndRender();

    // SimpleDraw::Render(mCamera); // Removed to avoid conflict, App handles DebugUI
}

void GameState::DebugUI()
{
    ImGui::Begin("Opal Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    
    static int currentPreset = 0;
    const char* presets[] = { "Welo (Default)", "Black Opal", "Fire Opal", "Crystal Opal", "Disco Ball", "Pearl" };
    if (ImGui::Combo("Preset", &currentPreset, presets, IM_ARRAYSIZE(presets)))
    {
        if (currentPreset == 0) // Welo
        {
            mOpalParams.baseColor = { 0.39f, 0.78f, 1.0f };
			mOpalParams.opalStrength = 2.0f;
            mOpalParams.fireTint = { 0.99f, 1.0f, 0.8f };
            mOpalParams.milkyPower = 0.1f;
            mOpalParams.milkyIntensity = 1.0f;
			mOpalParams.refractionStrength = 0.1f;
			mOpalParams.iridescenceScale = 5.0f;
			mOpalParams.iridescencePower = 1.0f;
            mOpalParams.voronoiScale = 5.0f;
			mOpalParams.voronoiDisplacement = 0.97f;
            mOpalParams.shapeDistortion = 0.23f;
			mOpalParams.noiseScale = 2.0f;
			mOpalParams.facetStrength = 1.0f;
            mOpalParams.featureScale = 1.0f;
            mOpalParams.featureScaleMin = 0.0f;
            mOpalParams.facetStrength = 1.0f;
        }
        else if (currentPreset == 1) // Black Opal
        {
            mOpalParams.fireTint = { 0.99, 1.0f, 0.68f };
            mOpalParams.opalStrength = 2.0f;
            mOpalParams.baseColor = { 0.027f, 0.0f, 0.231f };
            mOpalParams.milkyPower = 0.5f;
            mOpalParams.milkyIntensity = 1.0f;
            mOpalParams.refractionStrength = 0.5f;
            mOpalParams.iridescenceScale = 5.0f;
            mOpalParams.iridescencePower = 1.5f;
            mOpalParams.voronoiScale = 43.8f;
            mOpalParams.voronoiDisplacement = 1.0f;
            mOpalParams.shapeDistortion = 0.04f;
            mOpalParams.noiseScale = 1.21f;
			mOpalParams.featureScale = 0.34f;
            mOpalParams.featureScaleMin = 0.0f;
            mOpalParams.facetStrength = 0.69f;
            mOpalParams.flashSharpness = 24.0f;
            mOpalParams.voronoiBorder = 0.0f;
            mOpalParams.scratchScale = 20.0f;
            mOpalParams.scratchIntensity = 0.992f;
        }
        else if (currentPreset == 2) // Fire Opal
        {
            mOpalParams.fireTint = { 1.0f, 0.90f, 0.6f };
            mOpalParams.opalStrength = 2.0f;
            mOpalParams.baseColor = { 0.8f, 0.2f, 0.05f };
            mOpalParams.milkyPower = 2.5f;
            mOpalParams.milkyIntensity = 1.0f;
            mOpalParams.refractionStrength = 0.35f;
            mOpalParams.iridescenceScale = 5.0f;
            mOpalParams.iridescencePower = 1.5f;
            mOpalParams.voronoiScale = 20.6f;
            mOpalParams.voronoiDisplacement = 1.0f;
            mOpalParams.shapeDistortion = 0.17f;
            mOpalParams.noiseScale = 1.21f;
            mOpalParams.featureScale = 1.0f;
            mOpalParams.featureScaleMin = 0.0f;
            mOpalParams.facetStrength = 0.51f;
            mOpalParams.flashSharpness = 30.0f;
            mOpalParams.voronoiBorder = 0.0f;
            mOpalParams.scratchScale = 20.0f;
            mOpalParams.scratchIntensity = 0.992f;

        }
        else if (currentPreset == 3) // Crystal Opal (Clear)
        {
            mOpalParams.fireTint = { 1.0f, 0.8f, 0.8f };
            mOpalParams.opalStrength = 2.0f;
            mOpalParams.baseColor = { 0.1607843f, 0.7058824f, 1.0f };
            mOpalParams.milkyPower = 1.5f;
            mOpalParams.milkyIntensity = 1.0f;
            mOpalParams.refractionStrength = 0.1f;
            mOpalParams.iridescenceScale = 1.0f;
            mOpalParams.iridescencePower = 5.0f;
            mOpalParams.voronoiScale = 15.5f;
            mOpalParams.voronoiDisplacement = 1.0f;
            mOpalParams.shapeDistortion = 1.37f;
            mOpalParams.noiseScale = 0.5f;
            mOpalParams.featureScale = 1.0f;
            mOpalParams.featureScaleMin = 5.0f;
            mOpalParams.facetStrength = 0.6f;
            mOpalParams.flashSharpness = 30.0f;
            mOpalParams.voronoiBorder = 0.0f;
            mOpalParams.scratchScale = 20.0f;
            mOpalParams.scratchIntensity = 0.999f;


        }
        else if (currentPreset == 4) // Disco
        {
            mOpalParams.fireTint = { 1.0f, 0.8f, 0.8f };
            mOpalParams.opalStrength = 2.0f;
            mOpalParams.baseColor = { 1.0f, 1.0f, 1.0f };
            mOpalParams.milkyPower = 1.5f;
            mOpalParams.milkyIntensity = 1.0f;
            mOpalParams.refractionStrength = 0.1f;
            mOpalParams.iridescenceScale = 1.0f;
            mOpalParams.iridescencePower = 5.0f;
            mOpalParams.voronoiScale = 24.6f;
            mOpalParams.voronoiDisplacement = 0.1f;
            mOpalParams.shapeDistortion = 0.0f;
            mOpalParams.noiseScale = 0.5f;
            mOpalParams.featureScale = 0.0f;
            mOpalParams.featureScaleMin = 0.0f;
            mOpalParams.facetStrength = 0.6f;
            mOpalParams.flashSharpness = 10.0f;
            mOpalParams.voronoiBorder = 0.0f;
            mOpalParams.scratchScale = 20.0f;
            mOpalParams.scratchIntensity = 0.999f;
        }
        else if (currentPreset == 5) // Pearl
        {
            mOpalParams.fireTint = { 1.0f, 0.8f, 0.8f };
            mOpalParams.opalStrength = 2.0f;
            mOpalParams.baseColor = { 0.4f, 0.4f, 0.5019608f };
            mOpalParams.milkyPower = 0.1f;
            mOpalParams.milkyIntensity = 1.0f;
            mOpalParams.refractionStrength = 0.285f;
            mOpalParams.iridescenceScale = 0.3f;
            mOpalParams.iridescencePower = 5.0f;
            mOpalParams.voronoiScale = 0.1f;
            mOpalParams.voronoiDisplacement = 1.0f;
            mOpalParams.shapeDistortion = 0.0f;
            mOpalParams.noiseScale = 0.01f;
            mOpalParams.featureScale = 0.0f;
            mOpalParams.featureScaleMin = 0.0f;
            mOpalParams.facetStrength = 0.59f;
            mOpalParams.flashSharpness = 1.0f;
            mOpalParams.voronoiBorder = 0.0f;
            mOpalParams.scratchScale = 19.0f;
            mOpalParams.scratchIntensity = 0.0f;

        }
    }
    ImGui::Separator();

    ImGui::Text("Exterior (Opal Fire)");
    ImGui::ColorEdit3("Fire Tint", &mOpalParams.fireTint.x);
    ImGui::DragFloat("Opal Strength", &mOpalParams.opalStrength, 0.01f, 0.0f, 2.0f);
    ImGui::ColorEdit3("Body Color", &mOpalParams.baseColor.x);
    ImGui::DragFloat("Milky Power", &mOpalParams.milkyPower, 0.1f, 0.1f, 10.0f);
    ImGui::DragFloat("Milky Intensity", &mOpalParams.milkyIntensity, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Refraction", &mOpalParams.refractionStrength, 0.001f, 0.0f, 0.5f);
    ImGui::DragFloat("Iridescence Scale", &mOpalParams.iridescenceScale, 0.1f, 0.0f, 50.0f);
    ImGui::DragFloat("Iridescence Power", &mOpalParams.iridescencePower, 0.1f, 0.5f, 16.0f);
    ImGui::DragFloat("Voronoi Scale", &mOpalParams.voronoiScale, 0.1f, 0.1f, 50.0f);
    ImGui::DragFloat("Voronoi Jitter", &mOpalParams.voronoiDisplacement, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Shape Distort", &mOpalParams.shapeDistortion, 0.01f, 0.0f, 2.0f);
    ImGui::DragFloat("Noise Scale", &mOpalParams.noiseScale, 0.1f, 0.01f, 10.0f);
    ImGui::DragFloat("Size Max", &mOpalParams.featureScale, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Size Min", &mOpalParams.featureScaleMin, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Facet Strength", &mOpalParams.facetStrength, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Flash Sharpness", &mOpalParams.flashSharpness, 1.0f, 1.0f, 256.0f);
    ImGui::DragFloat("Cell Spacing", &mOpalParams.voronoiBorder, 0.01f, 0.0f, 0.5f);
    
    ImGui::Text("Scratches");
    ImGui::DragFloat("Scratch Scale", &mOpalParams.scratchScale, 0.1f, 0.1f, 20.0f);
    ImGui::DragFloat("Scratch Intensity", &mOpalParams.scratchIntensity, 0.001f, 0.0f, 1.0f);

    ImGui::Text("Debug");
    const char* debugModes[] = { "None", "Fire Texture", "Normals", "Voronoi ID", "Milky Factor", "Refraction" };
    ImGui::Combo("Debug Mode", &mOpalParams.debugMode, debugModes, IM_ARRAYSIZE(debugModes));
    
    ImGui::End();
}

void GameState::UpdateCamera(float deltaTime)
{
    InputSystem* input = InputSystem::Get();
    const float moveSpeed = input->IsKeyDown(KeyCode::LSHIFT) ? 10.0f : 4.0f;
    const float turnSpeed = 0.5f;

    if (input->IsKeyDown(KeyCode::W)) { mCamera.Walk(moveSpeed * deltaTime); }
    else if (input->IsKeyDown(KeyCode::S)) { mCamera.Walk(-moveSpeed * deltaTime); }
    if (input->IsKeyDown(KeyCode::D)) { mCamera.Strafe(moveSpeed * deltaTime); }
    else if (input->IsKeyDown(KeyCode::A)) { mCamera.Strafe(-moveSpeed * deltaTime); }
    if (input->IsKeyDown(KeyCode::E)) { mCamera.Rise(moveSpeed * deltaTime); }
    else if (input->IsKeyDown(KeyCode::Q)) { mCamera.Rise(-moveSpeed * deltaTime); }

    if (input->IsMouseDown(MouseButton::RBUTTON))
    {
        mCamera.Yaw(input->GetMouseMoveX() * turnSpeed * deltaTime);
        mCamera.Pitch(input->GetMouseMoveY() * turnSpeed * deltaTime);
    }
}
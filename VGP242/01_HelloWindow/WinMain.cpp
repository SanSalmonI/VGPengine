#include <IExeEngine/Inc/IExeEngine.h>

class MainState : public IExeEngine::AppState
{
public:
    void Initialize()
    {
        LOG("MAIN STATE INITIALIZED");
        mLifeTime = 2.0f;
    }
    void Terminate() override
    {
        LOG("MAIN STATE TERMINATED");
    }
    void Update(float deltaTime) override
    {
        mLifeTime -= std::max(deltaTime, 0.01f);
        if (mLifeTime <= 0.0f)
        {
            IExeEngine::MainApp().ChangeState("GameState");
        }
    }
private:
    float mLifeTime = 0.0f;
};

class GameState : public IExeEngine::AppState
{
public:
    void Initialize()
    {
        LOG("MAIN STATE INITIALIZED");
        mLifeTime = 2.0f;
    }
    void Terminate() override
    {
        LOG("MAIN STATE TERMINATED");
    }
    void Update(float deltaTime) override
    {
        mLifeTime -= std::max(deltaTime, 0.01f);
        if (mLifeTime <= 0.0f)
        {
            IExeEngine::MainApp().ChangeState("MainState");
        }
    }
private:
    float mLifeTime = 0.0f;
};

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    IExeEngine::AppConfig config;
    config.appName = L"Hello Window";

    IExeEngine::App& myApp = IExeEngine::MainApp();
    myApp.AddState<MainState>("MainState");
    myApp.AddState<GameState>("GameState");
    myApp.Run(config);

    return 0;
}
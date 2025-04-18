#include <IExeEngine/Inc/IExeEngine.h>

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    IExeEngine::AppConfig config;
    config.appName = L"Hello Window";

    IExeEngine::App myApp = IExeEngine::MainApp();
    myApp.Run(config);

    return 0;
}
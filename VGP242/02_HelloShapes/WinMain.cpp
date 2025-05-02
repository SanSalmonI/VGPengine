#include <IExeEngine/Inc/IExeEngine.h>
#include "ShapeState.h"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    IExeEngine::AppConfig config;
    config.appName = L"Hello Shapes";

    config.winWidth = 720;
    config.winHeight = 720;

    IExeEngine::App& myApp = IExeEngine::MainApp();

    myApp.AddState<ShapeState>("ShapeState");
    myApp.AddState<TriForce>("TriForce");
    myApp.AddState<House>("House");
    myApp.AddState<Heart>("Heart");

    myApp.Run(config);

    return 0;
}
#include <IExeEngine/Inc/IExeEngine.h>
#include "ShapeState.h"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    IExeEngine::AppConfig config;
    config.appName = L"Hello Shapes";

    config.winWidth = 1200;
    config.winHeight = 720;

    IExeEngine::App& myApp = IExeEngine::MainApp();

    myApp.AddState<Bunny>("Bunny");
    myApp.AddState<Butterfly>("Butterfly");
    myApp.AddState<Fish>("Fish");

    myApp.Run(config);

    return 0;
}
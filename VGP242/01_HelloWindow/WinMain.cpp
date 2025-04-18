#include <IExeEngine/Inc/IExeEngine.h>

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    IExeEngine::App myApp;
    myApp.Run();

    return 0;
}
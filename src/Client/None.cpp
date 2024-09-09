#include "None.h"
#include <thread>
#include <filesystem>
#include <fstream>

namespace Client
{
    void None::initialize()
    {
        moduleManager.Init();
        fileManager.init();
        Menu::Setup();
    }

    void None::shutdown()
    {
        fileManager.save();
        Menu::Destroy();
    }
}

#pragma once
#ifndef Client_None_H
#define Client_None_H
#include "ModuleManager/ModuleManager.h"
#include "FileManager/FileManager.h"

#include "../Etc/Menu/Menu.h"
namespace Client
{
    class None
    {
    public:
        void initialize();
        void shutdown();
        Module::ModuleManager moduleManager = Module::ModuleManager();
        File::FileManager fileManager = File::FileManager();
    private:
        bool stop_server = false;
    };
    inline None client = None();
}
#endif
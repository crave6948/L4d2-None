#pragma once
#include <string>
#include "../../../SDK/SDK.h"
#include "Utils/UtilsCollector.h"
#include "../../Value/ValueManager.h"
#include "ModuleCategory.h"
#include "../../../Etc/Imgui/imgui.h"
namespace Client::Module
{
    class Module
    {
    public:
        V::ValueManager vManager = V::ValueManager();
        void Create(std::string name, bool state, int keyCode, ModuleCategory category)
        {
            this->name = name;
            this->state = state;
            this->key = keyCode;
            this->category = category;
        };
        // Get Name of this Module
        std::string getName()
        {
            return name;
        };
        bool state = false;
        // Get Enabled State of this Module
        bool getEnabled()
        {
            return state;
        };
        void setEnabled(bool state)
        {
            this->state = state;
            state ? onEnabled() : onDisabled();
        };
        int getKey()
        {
            return key;
        };
        void setKey(int key)
        {
            this->key = key;
        };
        ModuleCategory getCategory()
        {
            return category;
        };
        virtual void onPreCreateMove(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal){};
        virtual void onPostCreateMove(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal){};
        virtual void onPrePrediction(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal){};
        virtual void onPrediction(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal, int PredictedFlags){};
        virtual void onPostPrediction(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal){};
        virtual void onRender2D(){};
        virtual void onFrameStageNotify(ClientFrameStage_t curStage){};
        void toggle();
        virtual void onEnabled(){};
        virtual void onDisabled(){};
        int keytimeout = 0;
        bool ShouldToggle()
        {
            return keytimeout <= 0;
        }
        int animate = 10;
        void updateanimate()
        {
            if (state)
            {
                if (animate > 0)
                {
                    animate--;
                }
            }
            else
            {
                animate = 10;
            }
        }
    private:
        std::string name;
        int key = 0;
        ModuleCategory category;
    };
}
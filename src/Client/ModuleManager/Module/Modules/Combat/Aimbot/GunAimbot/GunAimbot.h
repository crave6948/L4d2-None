#pragma once

#include "../TargetInfo.h"
// using namespace Client::Module::AimbotModule;
namespace Client::Module::AimbotModule::GunAimbotModule
{
    class GunAimbot
    {
    public:
        GunAimbot() {};
        // Gun Config
        V::BooleanValue *gunOnly = new V::BooleanValue("Gun", true);
        V::NumberValue *gunFov = new V::NumberValue("GunFov", 180, 0, 180);
        V::FloatValue *gunRange = new V::FloatValue("GunRange", 1400.f, 100.f, 2000.f);
        V::BooleanValue *gunTrigger = new V::BooleanValue("GunTrigger", true);
        V::NumberValue *gunFovTrigger = new V::NumberValue("GunFovTrigger", 3, 0, 20);
        void registerValues();
        void RenderValueGui();
        void onPreCreateMove(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal);
        void onPostCreateMove(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal);
        void onRender2D(C_TerrorPlayer * pLocal, C_TerrorWeapon *pWeapon);
        TargetInfo GetTarget(C_TerrorPlayer *pLocal, C_TerrorWeapon *pWeapon, CUserCmd *cmd);
        bool isInvaildOrDead(C_TerrorPlayer *pLocal, C_TerrorWeapon *pWeapon);

    private:
        bool isInCrossHair(CUserCmd *cmd, C_TerrorPlayer *pLocal, IClientEntity *target);

        bool isLeftClicking = false;
    };
}
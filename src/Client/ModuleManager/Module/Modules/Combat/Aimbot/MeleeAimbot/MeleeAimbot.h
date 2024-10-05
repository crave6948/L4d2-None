#pragma once
#include "../GunAimbot/GunAimbot.h"
using namespace Client::Module::AimbotModule;
namespace Client::Module::AimbotModule::MeleeAimbotModule
{
    class MeleeAimbot
    {
    public:
        MeleeAimbot() {};
        // Melee Config
        V::BooleanValue *meleeOnly = new V::BooleanValue("Melee", true);
        V::FloatValue *meleeRange = new V::FloatValue("MeleeRange", 150.f, 1.f, 400.f);
        V::FloatValue *meleePreLook = new V::FloatValue("MeleePreLookRange", 100.f, 0.f, 400.f);
        V::NumberValue *meleeFovTrigger = new V::NumberValue("MeleeFovTrigger", 10, 0, 180);
        V::NumberValue *meleeFov = new V::NumberValue("MeleeFov", 180, 0, 180);
        void registerValues();
        void RenderValueGui();
        void onPreCreateMove(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal);
        void onPostCreateMove(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal);
        void onRender2D(C_TerrorPlayer *pLocal, C_TerrorWeapon *pWeapon);
        TargetInfo GetTarget(C_TerrorPlayer *pLocal, C_TerrorWeapon *pWeapon, CUserCmd *cmd);
        bool isInvaildOrDead(C_TerrorPlayer *pLocal, C_TerrorWeapon *pWeapon);

    private:
        bool isInCrossHair(CUserCmd *cmd, C_TerrorPlayer *pLocal, IClientEntity *target);
    };
}

#pragma once
#include "../../../ModuleHeader.h"
#include "../../../../../Rotation/RotationManager.h"

#include <functional>
namespace Client::Module
{
    namespace AimbotModule
    {
        class TargetInfo
        {
        public:
            IClientEntity *target = nullptr;
            Vector targetPosition;
            Helper::Rotation aimRotation;
            int hitGroup;
            int classId;
            TargetInfo()
            {
                this->target = nullptr;
            }
            TargetInfo(IClientEntity *target, Vector targetPosition, Helper::Rotation aim, int hitGroup, int classId)
            {
                this->target = target;
                this->targetPosition = targetPosition;
                this->aimRotation = aim;
                this->hitGroup = hitGroup;
                this->classId = classId;
            }
            auto getTargetInfo()
            {
                return std::make_tuple(this->target, this->targetPosition, this->aimRotation, this->hitGroup, this->classId);
            }
        };
        class Aimbot : public Module
        {
        public:
            Aimbot()
            {
                this->Create("Aimbot", true, VK_NUMPAD7, ModuleCategory::Combat);
                vManager.AddValue(gunOnly);
                vManager.AddValue(gunRange);
                vManager.AddValue(gunFov);
                vManager.AddValue(gunFovTrigger);

                vManager.AddValue(meleeOnly);
                vManager.AddValue(meleeRange);
                vManager.AddValue(meleePreLook);
                vManager.AddValue(meleeFovTrigger);
                vManager.AddValue(meleeFov);

                vManager.AddValue(sortModes);
                vManager.AddValue(rotationMode);
                vManager.AddValue(switchDelay);
                vManager.AddValue(updateDelay);

                vManager.AddValue(infected);
                vManager.AddValue(boomer);
                vManager.AddValue(spitter);
                vManager.AddValue(charger);
                vManager.AddValue(smoker);
                vManager.AddValue(jockey);
                vManager.AddValue(hunter);
                vManager.AddValue(witch);
                vManager.AddValue(witchRage);
                vManager.AddValue(tank);

                vManager.AddValue(debug);
            };
            void RenderValueGui() override
            {
                BooleanCheckBox(gunOnly);
                if (gunOnly->GetValue())
                {
                    FloatSlider(gunRange);
                    IntegerSlider(gunFov);
                    IntegerSlider(gunFovTrigger);
                }

                BooleanCheckBox(meleeOnly);
                if (meleeOnly->GetValue())
                {
                    FloatSlider(meleeRange);
                    FloatSlider(meleePreLook);
                    IntegerSlider(meleeFovTrigger);
                    IntegerSlider(meleeFov);
                }

                ListBox(sortModes);
                ListBox(rotationMode);
                IntegerSlider(switchDelay);
                IntegerSlider(updateDelay);

                BooleanCheckBox(infected);
                BooleanCheckBox(boomer);
                BooleanCheckBox(spitter);
                BooleanCheckBox(charger);
                BooleanCheckBox(smoker);
                BooleanCheckBox(jockey);
                BooleanCheckBox(hunter);
                BooleanCheckBox(witch);
                if (witch->GetValue())
                    BooleanCheckBox(witchRage);
                BooleanCheckBox(tank);

                BooleanCheckBox(debug);
            };
            V::BooleanValue *gunOnly = new V::BooleanValue("Gun", true);
            V::FloatValue *gunRange = new V::FloatValue("Range", 1400.f, 100.f, 2000.f);
            V::NumberValue *gunFov = new V::NumberValue("Fov", 180, 0, 180);
            V::NumberValue *gunFovTrigger = new V::NumberValue("GunFovTrigger", 10, 0, 180);
            
            V::BooleanValue *meleeOnly = new V::BooleanValue("Melee", true);
            V::FloatValue *meleeRange = new V::FloatValue("MeleeRange", 150.f, 1.f, 400.f);
            V::FloatValue *meleePreLook = new V::FloatValue("MeleePreLookRange", 100.f, 0.f, 400.f);
            V::NumberValue *meleeFovTrigger = new V::NumberValue("MeleeFovTrigger", 10, 0, 180);
            V::NumberValue *meleeFov = new V::NumberValue("MeleeFov", 180, 0, 180);

            V::ListValue *sortModes = new V::ListValue("Sort Mode", {"Distance", "Fov", "Both"}, "Both");
            V::ListValue *rotationMode = new V::ListValue("Rotation Mode", {"Legit", "Instant", "Slient", "PerfectSlient"}, "Slient");
            V::NumberValue *switchDelay = new V::NumberValue("SwitchDelay", 400, 0, 1000, "ms");
            V::NumberValue *updateDelay = new V::NumberValue("UpdateDelay", 200, 0, 1000, "ms");

            // infected, special infected, witch, tank
            V::BooleanValue *infected = new V::BooleanValue("Infected", true);
            // specialInfected has boomer, spitter, charger, smoker, jockey, hunter
            V::BooleanValue *boomer = new V::BooleanValue("Boomer", true);
            V::BooleanValue *spitter = new V::BooleanValue("Spitter", true);
            V::BooleanValue *charger = new V::BooleanValue("Charger", true);
            V::BooleanValue *smoker = new V::BooleanValue("Smoker", true);
            V::BooleanValue *jockey = new V::BooleanValue("Jockey", true);
            V::BooleanValue *hunter = new V::BooleanValue("Hunter", true);
            // witch and tank are separate from specialInfected value
            V::BooleanValue *witch = new V::BooleanValue("Witch", true);
            V::BooleanValue *witchRage = new V::BooleanValue("Witch-Rage", true);
            V::BooleanValue *tank = new V::BooleanValue("Tank", true);

            V::BooleanValue *debug = new V::BooleanValue("Debug Mode", false);

            void onPreCreateMove(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal) override;
            void onPostCreateMove(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal) override;
            void onRender2D() override;
            void onEnabled() override;

            TargetInfo targetInfo;

        private:
            bool ShouldRun(C_TerrorPlayer *pLocal, C_TerrorWeapon *pWeapon, CUserCmd *cmd);
            std::pair<bool, int> CheckWeapon(C_TerrorWeapon *pWeapon);
            bool isInCrossHair(CUserCmd *cmd, C_TerrorPlayer *pLocal, IClientEntity *target);
            bool isInvaildOrDead(C_TerrorPlayer *pLocal, C_TerrorWeapon *pWeapon);
            TargetInfo GetTarget(C_TerrorPlayer *pLocal, C_TerrorWeapon *pWeapon, CUserCmd *cmd);
            const std::vector<std::pair<std::function<bool()>, EClientClass>> entityTypes = {
                {std::bind(&V::BooleanValue::GetValue, infected), EClientClass::Infected},
                {std::bind(&V::BooleanValue::GetValue, boomer), EClientClass::Boomer},
                {std::bind(&V::BooleanValue::GetValue, spitter), EClientClass::Spitter},
                {std::bind(&V::BooleanValue::GetValue, charger), EClientClass::Charger},
                {std::bind(&V::BooleanValue::GetValue, smoker), EClientClass::Smoker},
                {std::bind(&V::BooleanValue::GetValue, jockey), EClientClass::Jockey},
                {std::bind(&V::BooleanValue::GetValue, hunter), EClientClass::Hunter},
                {std::bind(&V::BooleanValue::GetValue, witch), EClientClass::Witch},
                {std::bind(&V::BooleanValue::GetValue, tank), EClientClass::Tank}};
            float lastSwitch = 0, lastUpdate = 0;
            bool isLeftClicking = false, hasLeftClickBefore = false;
            std::string className(int classId)
            {
                switch (classId)
                {
                case EClientClass::Infected:
                    return "Infected";
                case EClientClass::Boomer:
                    return "Boomer";
                case EClientClass::Spitter:
                    return "Spitter";
                case EClientClass::Charger:
                    return "Charger";
                case EClientClass::Smoker:
                    return "Smoker";
                case EClientClass::Jockey:
                    return "Jockey";
                case EClientClass::Hunter:
                    return "Hunter";
                case EClientClass::Witch:
                    return "Witch";
                case EClientClass::Tank:
                    return "Tank";
                default:
                    return "Unknown";
                }
            }
            bool isMelee(int weaponId) { return weaponId == WEAPON_MELEE || weaponId == WEAPON_CHAINSAW; };
        };
    }
};
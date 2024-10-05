// #pragma once
// #include "../../../ModuleHeader.h"
// #include "../../../../../Rotation/RotationManager.h"
// #include "GunAimbot/GunAimbot.h"
// #include "MeleeAimbot/MeleeAimbot.h"

// #include <functional>
// namespace Client::Module
// {
//     namespace AimbotModule
//     {
//         class TargetInfo
//         {
//         public:
//             IClientEntity *target = nullptr;
//             Vector targetPosition;
//             Helper::Rotation aimRotation;
//             int hitGroup;
//             int classId;
//             TargetInfo()
//             {
//                 this->target = nullptr;
//             }
//             TargetInfo(IClientEntity *target, Vector targetPosition, Helper::Rotation aim, int hitGroup, int classId)
//             {
//                 this->target = target;
//                 this->targetPosition = targetPosition;
//                 this->aimRotation = aim;
//                 this->hitGroup = hitGroup;
//                 this->classId = classId;
//             }
//             auto getTargetInfo()
//             {
//                 return std::make_tuple(this->target, this->targetPosition, this->aimRotation, this->hitGroup, this->classId);
//             }
//         };
//         class Aimbot : public Module
//         {
//         public:
//             GunAimbot::GunAimbot *gunAimbot;
//             MeleeAimbot::MeleeAimbot *meleeAimbot;
//             Aimbot()
//             {
//                 this->Create("Aimbot", true, VK_NUMPAD7, ModuleCategory::Combat);
//                 gunAimbot = new GunAimbot::GunAimbot();
//                 gunAimbot->registerValues();

//                 vManager.AddValue(sortModes);
//                 vManager.AddValue(silent);
//                 vManager.AddValue(switchDelay);
                
//                 meleeAimbot = new MeleeAimbot::MeleeAimbot();
//                 meleeAimbot->registerValues();

//                 vManager.AddValue(infected);
//                 vManager.AddValue(boomer);
//                 vManager.AddValue(spitter);
//                 vManager.AddValue(charger);
//                 vManager.AddValue(smoker);
//                 vManager.AddValue(jockey);
//                 vManager.AddValue(hunter);
//                 vManager.AddValue(witch);
//                 vManager.AddValue(witchRage);
//                 vManager.AddValue(tank);

//                 vManager.AddValue(debug);
//             };
//             void RenderValueGui() override
//             {
//                 gunAimbot->RenderValueGui();

//                 ListBox(sortModes);
//                 BooleanCheckBox(silent);
//                 IntegerSlider(switchDelay);

//                 meleeAimbot->RenderValueGui();

//                 BooleanCheckBox(infected);
//                 BooleanCheckBox(boomer);
//                 BooleanCheckBox(spitter);
//                 BooleanCheckBox(charger);
//                 BooleanCheckBox(smoker);
//                 BooleanCheckBox(jockey);
//                 BooleanCheckBox(hunter);
//                 BooleanCheckBox(witch);
//                 if (witch->GetValue())
//                     BooleanCheckBox(witchRage);
//                 BooleanCheckBox(tank);

//                 BooleanCheckBox(debug);
//             };

            
//             // General Config
//             V::ListValue *sortModes = new V::ListValue("Sort Mode", {"Distance", "Fov", "Both"}, "Both");
//             V::BooleanValue *silent = new V::BooleanValue("Silent", true);
//             V::NumberValue *switchDelay = new V::NumberValue("SwitchDelay", 400, 0, 1000, "ms");
//             // infected, special infected, witch, tank
//             V::BooleanValue *infected = new V::BooleanValue("Infected", true);
//             // specialInfected has boomer, spitter, charger, smoker, jockey, hunter
//             V::BooleanValue *boomer = new V::BooleanValue("Boomer", true);
//             V::BooleanValue *spitter = new V::BooleanValue("Spitter", true);
//             V::BooleanValue *charger = new V::BooleanValue("Charger", true);
//             V::BooleanValue *smoker = new V::BooleanValue("Smoker", true);
//             V::BooleanValue *jockey = new V::BooleanValue("Jockey", true);
//             V::BooleanValue *hunter = new V::BooleanValue("Hunter", true);
//             // witch and tank are separate from specialInfected value
//             V::BooleanValue *witch = new V::BooleanValue("Witch", true);
//             V::BooleanValue *witchRage = new V::BooleanValue("Witch-Rage", true);
//             V::BooleanValue *tank = new V::BooleanValue("Tank", true);

//             V::BooleanValue *debug = new V::BooleanValue("Debug Mode", false);

//             void onPreCreateMove(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal) override;
//             void onPostCreateMove(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal) override;
//             void onRender2D() override;
//             void onEnabled() override;

//             TargetInfo targetInfo;
//             float lastTime = 0;
//             bool isLeftClicking = false, hasLeftClickBefore = false;
//             std::pair<bool, int> CheckWeapon(C_TerrorWeapon *pWeapon);
//             bool isGun(int weaponId)
//             {
//                 bool isGun = false;
//                 switch (weaponId)
//                 {
//                 case WEAPON_AK47:
//                 case WEAPON_AWP:
//                 case WEAPON_DEAGLE:
//                 case WEAPON_HUNTING_RIFLE:
//                 case WEAPON_M16A1:
//                 case WEAPON_M60:
//                 case WEAPON_MAC10:
//                 case WEAPON_MILITARY_SNIPER:
//                 case WEAPON_MP5:
//                 case WEAPON_PISTOL:
//                 case WEAPON_SCAR:
//                 case WEAPON_SCOUT:
//                 case WEAPON_SSG552:
//                 case WEAPON_UZI:
//                 case WEAPON_AUTO_SHOTGUN:
//                 case WEAPON_SPAS:
//                 case WEAPON_PUMP_SHOTGUN:
//                 case WEAPON_CHROME_SHOTGUN:
//                     isGun = true;
//                 default:
//                     break;
//                 }
//                 return isGun;
//             }
//             bool isMelee(int weaponId) { return weaponId == WEAPON_MELEE || weaponId == WEAPON_CHAINSAW; };

//         private:
//             bool ShouldRun(C_TerrorPlayer *pLocal, C_TerrorWeapon *pWeapon, CUserCmd *cmd);
//             bool isInCrossHair(CUserCmd *cmd, C_TerrorPlayer *pLocal, IClientEntity *target);
//             bool isInvaildOrDead(C_TerrorPlayer *pLocal, C_TerrorWeapon *pWeapon);
//             TargetInfo GetTarget(C_TerrorPlayer *pLocal, C_TerrorWeapon *pWeapon, CUserCmd *cmd);
//             const std::vector<std::pair<std::function<bool()>, EClientClass>> entityTypes = {
//                 {std::bind(&V::BooleanValue::GetValue, infected), EClientClass::Infected},
//                 {std::bind(&V::BooleanValue::GetValue, boomer), EClientClass::Boomer},
//                 {std::bind(&V::BooleanValue::GetValue, spitter), EClientClass::Spitter},
//                 {std::bind(&V::BooleanValue::GetValue, charger), EClientClass::Charger},
//                 {std::bind(&V::BooleanValue::GetValue, smoker), EClientClass::Smoker},
//                 {std::bind(&V::BooleanValue::GetValue, jockey), EClientClass::Jockey},
//                 {std::bind(&V::BooleanValue::GetValue, hunter), EClientClass::Hunter},
//                 {std::bind(&V::BooleanValue::GetValue, witch), EClientClass::Witch},
//                 {std::bind(&V::BooleanValue::GetValue, tank), EClientClass::Tank}};

//             std::string className(int classId)
//             {
//                 switch (classId)
//                 {
//                 case EClientClass::Infected:
//                     return "Infected";
//                 case EClientClass::Boomer:
//                     return "Boomer";
//                 case EClientClass::Spitter:
//                     return "Spitter";
//                 case EClientClass::Charger:
//                     return "Charger";
//                 case EClientClass::Smoker:
//                     return "Smoker";
//                 case EClientClass::Jockey:
//                     return "Jockey";
//                 case EClientClass::Hunter:
//                     return "Hunter";
//                 case EClientClass::Witch:
//                     return "Witch";
//                 case EClientClass::Tank:
//                     return "Tank";
//                 default:
//                     return "Unknown";
//                 }
//             }
//         };
//     }
// };
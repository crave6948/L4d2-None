#include "ThirdPerson.h"

#include "../../../../../None.h"

namespace Client::Module
{
    namespace ThirdPersonModule
    {
        void ThirdPerson::onEnabled()
        {
            isLocking = false;
            isThirdPerson = false;
        }

        void ThirdPerson::onRender2D()
        {
            if (!I::EngineClient->IsInGame() || I::EngineVGui->IsGameUIVisible())
                return;
            if (!debug->GetValue())
                return;
            std::string text = "Third Person : " + std::to_string(I::IInput->m_fCameraInThirdPerson());
            G::Draw.String(EFonts::DEBUG, G::Draw.m_nScreenW / 2, G::Draw.m_nScreenH / 2, Color(255, 255, 255, 255), TXT_CENTERXY, text.c_str());
            Vector pos = I::IInput->m_vecCameraOffset();
            std::string text2 = std::to_string(pos.x) + " " + std::to_string(pos.y) + " " + std::to_string(pos.z);
            G::Draw.String(EFonts::DEBUG, G::Draw.m_nScreenW / 2, G::Draw.m_nScreenH / 2 + 20, Color(255, 255, 255, 255), TXT_CENTERXY, text2.c_str());
            C_TerrorPlayer *pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer *>();
            C_TerrorWeapon *pWeapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon *>();
            auto [should, weaponId] = CheckWeapon(pWeapon);
            if (!should)
                return;
            if (weaponId != EWeaponID::WEAPON_MELEE)
                return;
            // std::string text3 = "Time AttackQueued : " + std::to_string(pWeapon->);
            // G::Draw.String(EFonts::DEBUG, G::Draw.m_nScreenW / 2, G::Draw.m_nScreenH / 2 + 40, Color(255, 255, 255, 255), TXT_CENTERXY, text3.c_str());
        }
        void ThirdPerson::onPostPrediction(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal)
        {
            // rotation = cmd->viewangles;
            isAllowToPerfect = cmd->buttons & IN_ATTACK && ShouldRun(pLocal, pWeapon, cmd) && pWeapon->CanPrimaryAttack();

            auto aimbot = Client::client.moduleManager.aimbot;
            if (aimbot->getEnabled() && aimbot->isAiming)
                return;
            if (freePSilent->GetValue() && isLocking && isAllowToPerfect)
            {
                Vector viewAngles;
                I::EngineClient->GetViewAngles(viewAngles);
                cmd->viewangles = viewAngles;
            }
        }
        void ThirdPerson::onPrePrediction(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal)
        {
            static bool originalThirdPersonState = false;
            if (GetAsyncKeyState(0x43) & 1)
            {
                isLocking = !isLocking;
                if (isLocking)
                {
                    lockRotation = cmd->viewangles;
                    originalThirdPersonState = isThirdPerson;
                    if (!isThirdPerson)
                        isThirdPerson = true;
                }
                else
                {
                    isThirdPerson = originalThirdPersonState;
                    I::EngineClient->SetViewAngles(lockRotation);
                    Helper::rotationManager.DisabledRotation = true;
                }
            }
            if (isLocking)
            {
                auto aimbot = Client::client.moduleManager.aimbot;
                if (aimbot->getEnabled() && aimbot->isAiming)
                    return;
                auto rotation = Helper::Rotation().toRotation(lockRotation);
                Helper::rotationManager.moveTo(rotation, 1, false, Helper::RotationType::Instant);
            }
        }
        void ThirdPerson::onFrameStageNotify(ClientFrameStage_t curStage)
        {
            if (!I::EngineClient->IsConnected() || !I::EngineClient->IsInGame() || I::EngineVGui->IsGameUIVisible())
                return;
            // toggle
            {
                if (GetAsyncKeyState(0x56) & 1)
                {
                    isThirdPerson = !isThirdPerson;
                }
            }
            C_TerrorPlayer *pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer *>();
            if (pLocal != nullptr)
            {
                if (isThirdPerson)
                {
                    // auto rot = Helper::rotationManager.getServerRotationVector();
                    // I::Prediction->SetLocalViewAngles(rot);
                    I::Prediction->SetLocalViewAngles(rotation);
                    I::IInput->m_fCameraInThirdPerson() = true;
                    I::Cvar->FindVar("cam_idealdist")->SetValue(distance->GetValue());
                    I::Cvar->FindVar("cam_collision")->SetValue(true);
                    I::Cvar->FindVar("cam_snapto")->SetValue(true);
                    I::Cvar->FindVar("c_thirdpersonshoulder")->SetValue(true);
                    I::Cvar->FindVar("c_thirdpersonshoulderaimdist")->SetValue(0.f);
                    I::Cvar->FindVar("c_thirdpersonshoulderdist")->SetValue(0.f);
                    I::Cvar->FindVar("c_thirdpersonshoulderheight")->SetValue(0.f);
                    I::Cvar->FindVar("c_thirdpersonshoulderoffset")->SetValue(0);
                }
                else
                {
                    I::IInput->m_fCameraInThirdPerson() = false;
                }
            }
        }
        bool ThirdPerson::getShouldPerfectSilent()
        {
            if (!freePSilent->GetValue())
                return false;
            if (!isLocking)
                return false;
            return isAllowToPerfect;
        }
        bool ThirdPerson::ShouldRun(C_TerrorPlayer *pLocal, C_TerrorWeapon *pWeapon, CUserCmd *cmd)
        {
            if (!I::EngineClient->IsInGame())
            {
                return false;
            }
            if (I::EngineVGui->IsGameUIVisible())
            {
                return false;
            }
            if (cmd->buttons & IN_USE)
                return false;

            // if (!pLocal->CanAttackFull() || pLocal->m_isHangingFromLedge() || pLocal->m_isHangingFromTongue() || pLocal->m_isIncapacitated())
            if (!pLocal->CanAttackFull() || pLocal->m_isHangingFromLedge() || pLocal->m_isHangingFromTongue())
                return false;

            // You could also check if the current spread is -1.0f and not run nospread I guess.
            // But since I wanted to filter out shotungs and just be sure that it isnt ran for other stuff I check the weaponid.
            auto [should, _] = CheckWeapon(pWeapon);

            // check if fastmelee is swaping items
            return should;
        }
        std::pair<bool, int> ThirdPerson::CheckWeapon(C_TerrorWeapon *pWeapon)
        {
            if (!pWeapon)
                return std::make_pair<bool, int>(false, 9999);
            ;
            switch (pWeapon->GetWeaponID())
            {
            case WEAPON_AK47:
            case WEAPON_AWP:
            case WEAPON_DEAGLE:
            case WEAPON_HUNTING_RIFLE:
            case WEAPON_M16A1:
            case WEAPON_M60:
            case WEAPON_MAC10:
            case WEAPON_MILITARY_SNIPER:
            case WEAPON_MP5:
            case WEAPON_PISTOL:
            case WEAPON_SCAR:
            case WEAPON_SCOUT:
            case WEAPON_SSG552:
            case WEAPON_UZI:
            case WEAPON_AUTO_SHOTGUN:
            case WEAPON_SPAS:
            case WEAPON_PUMP_SHOTGUN:
            case WEAPON_CHROME_SHOTGUN:
            case WEAPON_GRENADE_LAUNCHER:
                return std::make_pair<bool, int>(gunOnly->GetValue(), pWeapon->GetWeaponID());
            case WEAPON_MELEE:
            case WEAPON_CHAINSAW:
                return std::make_pair<bool, int>(meleeOnly->GetValue(), pWeapon->GetWeaponID());
            default:
                break;
            }

            return std::make_pair<bool, int>(false, 9999);
        }
    }
}
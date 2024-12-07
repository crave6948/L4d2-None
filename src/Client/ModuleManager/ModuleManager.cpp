#include "ModuleManager.h"
#include "../Rotation/RotationManager.h"
#include "../None.h"

#include <algorithm> // For std::transform
#include <cctype>    // For std::tolower
#include <string>    // For std::string
namespace Client::Module
{

    ModuleManager::ModuleManager()
    {
    }

    ModuleManager::~ModuleManager()
    {
        delete bhop;
        delete aimbot;
        delete noSpread;
        delete autoShoot;
        delete fastMelee;
        delete arraylist;
        delete espHelper;
        delete thirdPerson;
        delete rotations;
        delete fontManager;
        delete fakeLag;
    }

    void ModuleManager::Init()
    {
        featurelist.push_back(bhop);

        featurelist.push_back(aimbot);
        featurelist.push_back(autoShoot);
        featurelist.push_back(noSpread);
        featurelist.push_back(fastMelee);

        featurelist.push_back(arraylist);
        featurelist.push_back(espHelper);
        featurelist.push_back(thirdPerson);
        featurelist.push_back(rotations);

        featurelist.push_back(fontManager);
        featurelist.push_back(fakeLag);
    }

    void ModuleManager::onRender2D()
    {
        for (Module *mod : featurelist)
        {
            if (!mod->getEnabled())
                continue;
            mod->onRender2D();
        }
        if (!I::EngineClient->IsInGame())
        {
            return;
        };
        if (I::EngineVGui->IsGameUIVisible())
        {
            return;
        }
        C_TerrorPlayer *pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer *>();
        int screenW = G::Draw.m_nScreenW, screenH = G::Draw.m_nScreenH;
        if (!renderServerCrosshair.IsZero())
        {
            Vector vec = U::Math.AngleVectors(renderServerCrosshair);
            CGameTrace trace;
            CTraceFilterHitAll filter{pLocal};
            G::Util.Trace(pLocal->Weapon_ShootPosition(), pLocal->Weapon_ShootPosition() + (vec * 1400.0f), (MASK_SHOT | CONTENTS_GRATE), &filter, &trace);
            Vector vViewAngleOnWorld = trace.endpos;
            Vector screen;
            G::Util.W2S(vViewAngleOnWorld, screen);

            G::Draw.Line(screen.x, screen.y, screen.x - 4, screen.y + 4, Color(255, 255, 255, 255));
            G::Draw.Line(screen.x, screen.y, screen.x + 4, screen.y + 4, Color(255, 255, 255, 255));
        }
        
        {
            screenW = screenW / 2;
            screenH = screenH / 2;
            G::Draw.Line(screenW, screenH, screenW - 4, screenH + 4, Color(255, 255, 255, 255));
            G::Draw.Line(screenW, screenH, screenW + 4, screenH + 4, Color(255, 255, 255, 255));
            screenW = screenW * 2;
            screenH = screenH * 2;
        }
    }

    void ModuleManager::onCreateMove(CUserCmd *cmd, C_TerrorPlayer *pLocal, C_TerrorWeapon *pWeapon)
    {
        renderServerCrosshair = Vector(0, 0, 0);
        if (pLocal && !pLocal->deadflag())
        {
            Vector oldViewangles = cmd->viewangles;
            for (Module *mod : featurelist)
            {
                if (!mod->getEnabled())
                    continue;
                mod->onPreCreateMove(cmd, pWeapon, pLocal);
            }

            for (Module *mod : featurelist)
            {
                if (!mod->getEnabled())
                    continue;
                mod->onPostCreateMove(cmd, pWeapon, pLocal);
            }
            if (pWeapon)
            {
                for (Module *mod : featurelist)
                {
                    if (!mod->getEnabled())
                        continue;
                    mod->onPrePrediction(cmd, pWeapon, pLocal);
                }
                F::EnginePrediction.Start(pLocal, cmd);
                {
                    for (Module *mod : featurelist)
                    {
                        if (!mod->getEnabled())
                            continue;
                        mod->onPrediction(cmd, pWeapon, pLocal, F::EnginePrediction.GetPredictedFlags());
                    }
                }
                F::EnginePrediction.Finish(pLocal, cmd);
            }
            Helper::rotationManager.onUpdate();
            if (aimbot->getEnabled() && aimbot->shouldPerfectSilent)
            {
            }
            else
            {
                if (!Helper::rotationManager.getServerRotationVector().IsZero() && !Helper::rotationManager.DisabledRotation)
                    cmd->viewangles = Helper::rotationManager.getServerRotationVector();
            }
            for (Module *mod : featurelist)
            {
                if (!mod->getEnabled())
                    continue;
                mod->onPostPrediction(cmd, pWeapon, pLocal);
            }
            Vector viewForMovement = oldViewangles;
            Vector viewForPerfectSlient = cmd->viewangles;
            if (thirdPerson->getEnabled() && thirdPerson->isLocking && !thirdPerson->freeStrafe->GetValue())
                viewForMovement = cmd->viewangles;
            if (thirdPerson->getEnabled() && thirdPerson->isLocking)
                viewForPerfectSlient = cmd->viewangles;
            // pSilent
            {
                static bool bWasSet = false;
                static int collected = 0;
                bool shouldDoPerfect = false;
                if (aimbot->getEnabled() && aimbot->shouldPerfectSilent)
                    shouldDoPerfect = true;
                if (thirdPerson->getEnabled() && thirdPerson->getShouldPerfectSilent())
                    shouldDoPerfect = true;
                if (noSpread->getEnabled() && noSpread->shouldPerfectSilent)
                    shouldDoPerfect = true;
                if (shouldDoPerfect)
                {
                    *I::pSendPacket = false;
                    bWasSet = true;
                    collected++;
                }
                else if (bWasSet)
                {
                    *I::pSendPacket = true;
                    cmd->viewangles = viewForPerfectSlient;
                    collected = 0;
                    bWasSet = false;
                }
                auto fakeLag = Client::client.moduleManager.fakeLag;
                if (!fakeLag->getEnabled())
                    if (collected > 4)
                    {
                        *I::pSendPacket = true;
                        if (cmd->buttons & IN_ATTACK)
                            cmd->buttons &= ~IN_ATTACK;
                        cmd->viewangles = viewForPerfectSlient;
                        collected = 0;
                        bWasSet = false;
                    }
            }
            renderServerCrosshair = cmd->viewangles;
            thirdPerson->rotation = renderServerCrosshair;
            G::Util.FixMovement(viewForMovement, cmd);
        }
    }
    void ModuleManager::onFrameStageNotify(ClientFrameStage_t curStage)
    {
        for (Module *mod : featurelist)
        {
            if (!mod->getEnabled())
                continue;
            mod->onFrameStageNotify(curStage);
        }
    }
    void ModuleManager::onKey()
    {
        // bool isToggled = keyState & 1;
        // bool isDown = keyState & 0x8000;
        if (GetAsyncKeyState(VK_HOME) & 1 && keyTimeout <= 0)
        {
            Client::client.fileManager.load();
            keyTimeout = 1;
        }
        else
        {
            if (keyTimeout > 0)
                keyTimeout--;
        }
        for (Module *mod : featurelist)
        {
            if (GetAsyncKeyState(mod->getKey()) & 1)
            {
                if (!mod->ShouldToggle())
                    continue;
                mod->toggle();
                mod->keytimeout = 1;
            }
            else
            {
                if (mod->keytimeout > 0)
                    mod->keytimeout--;
            }
        }
    }
    inline std::string toLowercase(std::string str)
    {
        std::transform(str.begin(), str.end(), str.begin(),
                       [](unsigned char c)
                       { return std::tolower(c); });
        return str;
    }
    Module *ModuleManager::getFeature(std::string name)
    {
        for (Module *mod : featurelist)
        {
            if (toLowercase(mod->getName()).compare(toLowercase(name)) == 0)
            {
                return mod;
            }
        }
        return nullptr;
    }
    std::vector<Module *> ModuleManager::getFeatureListByCategory(ModuleCategory category)
    {
        std::vector<Module *> result;
        for (Module *mod : featurelist)
        {
            if (mod->getCategory() == category)
            {
                result.push_back(mod);
            }
        }
        return result;
    }
}

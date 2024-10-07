#include "MeleeAimbot.h"
#include "../../../../../../Rotation/RotationManager.h"
#include "../../../../../../None.h"
// using namespace Client::Module::AimbotModule;
namespace Client::Module::AimbotModule::MeleeAimbotModule
{
    void MeleeAimbot::registerValues()
    {
        auto aimbot = Client::client.moduleManager.aimbot;
        aimbot->vManager.AddValue(meleeOnly);
        aimbot->vManager.AddValue(meleeRange);
        aimbot->vManager.AddValue(meleePreLook);
        aimbot->vManager.AddValue(meleeFovTrigger);
        aimbot->vManager.AddValue(meleeFov);
    }

    void MeleeAimbot::RenderValueGui()
    {
        auto aimbot = Client::client.moduleManager.aimbot;
        aimbot->BooleanCheckBox(meleeOnly);
        if (meleeOnly->GetValue())
        {
            aimbot->FloatSlider(meleeRange);
            aimbot->FloatSlider(meleePreLook);
            aimbot->IntegerSlider(meleeFovTrigger);
            aimbot->IntegerSlider(meleeFov);
        }
    }

    void MeleeAimbot::onPreCreateMove(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal)
    {
        // if we reach this point that mean we have found a target and target always not null
        auto aimbot = Client::client.moduleManager.aimbot;
        Vector hitbox = aimbot->targetInfo.target->As<C_BaseAnimating *>()->GetHitboxPositionByGroup(aimbot->targetInfo.hitGroup);
        Vector aimVector = U::Math.GetAngleToPosition(pLocal->Weapon_ShootPosition(), hitbox);
        aimbot->targetInfo.aimRotation = Helper::Rotation().toRotation(aimVector);
        aimbot->targetInfo.targetPosition = hitbox;
        float distance = pLocal->Weapon_ShootPosition().DistTo(aimbot->targetInfo.targetPosition);
        bool isInCrosshair = isInCrossHair(cmd, pLocal, aimbot->targetInfo.target);

        aimbot->hasLeftClickBefore = true;
        Helper::rotationManager.moveTo(aimbot->targetInfo.aimRotation, distance / 571.43f, isInCrosshair);
    }

    void MeleeAimbot::onPostCreateMove(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal)
    {
        if (cmd->buttons & IN_ATTACK)
        {
            auto aimbot = Client::client.moduleManager.aimbot;
            float distance = pLocal->Weapon_ShootPosition().DistTo(aimbot->targetInfo.targetPosition);
            Vector serverSide = Helper::rotationManager.getServerRotationVector();
            float fov = U::Math.GetFovBetween(serverSide, aimbot->targetInfo.aimRotation.toVector());
            if (fov > meleeFovTrigger->GetValue() || distance > meleeRange->GetValue())
                cmd->buttons &= ~IN_ATTACK;
        }
    }

    void MeleeAimbot::onRender2D(C_TerrorPlayer *pLocal, C_TerrorWeapon *pWeapon)
    {
        float flR = tanf(DEG2RAD(meleeFov->GetValue()) / 2) / tanf(DEG2RAD(pLocal->IsZoomed() ? 30 : 110) / 2) * G::Draw.m_nScreenW;
        G::Draw.OutlinedCircle(G::Draw.m_nScreenW / 2, G::Draw.m_nScreenH / 2, flR, 32, Color(178, 190, 181, 255));
    }

    TargetInfo MeleeAimbot::GetTarget(C_TerrorPlayer *pLocal, C_TerrorWeapon *pWeapon, CUserCmd *cmd)
    {
        auto aimbot = Client::client.moduleManager.aimbot;
        float aimRange = this->meleeRange->GetValue(), aimFov = this->meleeFov->GetValue();
        IClientEntity *foundTarget = nullptr;
        // collect all targets and find the best one (compare them by a score)
        Vector clientViewAngles = Helper::rotationManager.getServerRotationVector();
        if (Helper::rotationManager.DisabledRotation || !aimbot->hasLeftClickBefore)
            I::EngineClient->GetViewAngles(clientViewAngles);
        float currentScore = 1000.f;
        const auto updateTarget = [&](IClientEntity *target, float fov, float distance) -> bool
        {
            float fovScore = (fov * 100) / aimFov;
            float distanceScore = (distance * 100) / aimRange;
            float score = (aimbot->sortModes->GetSelected() == "Fov") ? fovScore : (aimbot->sortModes->GetSelected() == "Distance") ? distanceScore
                                                                                                                                    : distanceScore + fovScore;
            if (score < currentScore)
            {
                foundTarget = target;
                currentScore = score;
                return true;
            }
            return false;
        };
        const auto GetHitbox = [&](int classType) -> int
        {
            if (classType == EClientClass::Tank)
            {
                return HITGROUP_CHEST;
            }
            if (classType == EClientClass::Infected)
            {
                return HITGROUP_CHEST;
            }
            return HITGROUP_HEAD;
        };
        const auto GetFovDistance = [&, clientViewAngles](IClientEntity *target, int classType) -> std::pair<float, float>
        {
            Vector src = pLocal->Weapon_ShootPosition();
            Vector dst = target->As<C_BaseAnimating *>()->GetHitboxPositionByGroup(GetHitbox(classType));
            float distance = src.DistTo(dst);

            float fov = U::Math.GetFovBetween(clientViewAngles, U::Math.GetAngleToPosition(src, dst));
            return std::make_pair(fov, distance);
        };
        const auto doCompare = [&](IClientEntity *target, int classType) -> void
        {
            if (foundTarget == nullptr)
            {
                foundTarget = target;
                return;
            }
            auto [fov, distance] = GetFovDistance(target, classType);
            updateTarget(target, fov, distance);
        };
        const auto checkCondition = [&](IClientEntity *target, int classType) -> bool
        {
            auto [fov, distance] = GetFovDistance(target, classType);
            if (distance > aimRange)
                return false;
            if (fov > aimFov)
                return false;

            Vector hitbox = target->As<C_BaseAnimating *>()->GetHitboxPositionByGroup(GetHitbox(classType));
            CTraceFilterHitscan filter{pLocal};
            auto pHit{G::Util.GetHitEntity(pLocal->Weapon_ShootPosition(), hitbox, &filter)};
            if (!pHit || pHit->entindex() != target->entindex())
                return false;
            return true;
        };
        for (auto &[enabled, classType] : aimbot->entityTypes)
        {
            if (enabled())
            {
                for (auto entity : Utils::g_EntityCache.getEntityFromGroup(classType))
                {
                    if (!checkCondition(entity, classType))
                        continue;
                    if (classType == EClientClass::Witch && aimbot->witchRage->GetValue() && entity->As<C_Witch *>()->m_rage() != 1.0f)
                        continue;
                    doCompare(entity, classType);
                }
            }
        }
        if (foundTarget == nullptr)
            // If no target was found, return an empty TargetInfo
            // structure. This is necessary because the caller of this
            // function may not check if the target is valid or not, so
            // we need to make sure that the returned value is always
            // valid.
            return TargetInfo();
        int classid = foundTarget->GetBaseEntity()->GetClientClass()->m_ClassID;
        int hitgroup = GetHitbox(classid);
        Vector hitbox = foundTarget->As<C_BaseAnimating *>()->GetHitboxPositionByGroup(hitgroup);
        Vector aimVector = U::Math.GetAngleToPosition(pLocal->Weapon_ShootPosition(), hitbox);
        return TargetInfo(foundTarget, hitbox, Helper::Rotation().toRotation(aimVector), hitgroup, classid);
    }

    bool MeleeAimbot::isInvaildOrDead(C_TerrorPlayer *pLocal, C_TerrorWeapon *pWeapon)
    {
        auto aimbot = Client::client.moduleManager.aimbot;
        auto [target, targetPosition, aimRotation, hitGroup, classId] = aimbot->targetInfo.getTargetInfo();
        // check if target is dead or nullptr
        if (target == nullptr)
            return true;
        if (classId == EClientClass::Infected || classId == EClientClass::Witch)
        {
            auto c_infected = target->As<C_Infected *>();
            if (!G::Util.IsInfectedAlive(c_infected->m_usSolidFlags(), c_infected->m_nSequence()))
                return true;
        }
        else
        {
            auto c_terror = target->As<C_TerrorPlayer *>();
            if (c_terror->deadflag())
                return true;
        }
        // check if target is out of range
        float distance = pLocal->Weapon_ShootPosition().DistTo(targetPosition);
        auto [should, weaponId] = aimbot->CheckWeapon(pWeapon);
        float aimRange = meleeRange->GetValue(), aimfov = meleeFov->GetValue();
        if (distance > aimRange)
            return true;
        // check if target is outside FOV
        Vector clientViewAngles = Helper::rotationManager.getServerRotationVector();
        float fov = U::Math.GetFovBetween(clientViewAngles, aimRotation.toVector());
        if (fov > aimfov)
            return true;
        // check if target is visible
        CTraceFilterHitscan filter{pLocal};
        auto pHit{G::Util.GetHitEntity(pLocal->Weapon_ShootPosition(), targetPosition, &filter)};
        if (!pHit || pHit->entindex() != target->entindex())
            return true;
        return false;
    }

    bool MeleeAimbot::isInCrossHair(CUserCmd *cmd, C_TerrorPlayer *pLocal, IClientEntity *target)
    {
        Vector vec = U::Math.AngleVectors(Helper::rotationManager.getServerRotationVector());
        CTraceFilterHitscan filter{pLocal};
        bool shouldhit = false;
        if (auto pHit = G::Util.GetHitEntity(pLocal->Weapon_ShootPosition(), pLocal->Weapon_ShootPosition() + (vec * meleeRange->GetValue()), &filter))
        {
            if (pHit->entindex() != target->entindex())
            {
                switch (pHit->GetClientClass()->m_ClassID)
                {
                case EClientClass::Infected:
                case EClientClass::Boomer:
                case EClientClass::Jockey:
                case EClientClass::Smoker:
                case EClientClass::Hunter:
                case EClientClass::Spitter:
                case EClientClass::Charger:
                case EClientClass::Tank:
                {
                    shouldhit = true;
                    break;
                }
                case EClientClass::Witch:
                {
                    shouldhit = true;
                    if (pHit->As<C_Witch *>()->m_rage() != 1.0f)
                    {
                        shouldhit = false;
                    }
                    break;
                }
                default:
                    break;
                }
            }
            else
            {
                shouldhit = true;
            }
        }
        return shouldhit;
    }
}

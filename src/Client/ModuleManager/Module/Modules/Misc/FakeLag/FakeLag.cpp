#include "FakeLag.h"

namespace Client::Module
{
    namespace FakeLagModule
    {
        void FakeLag::onPreCreateMove(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal)
        {
            oldAngles = cmd->viewangles;
        }
        void FakeLag::onPostPrediction(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal)
        {
            if (collectedPackets < packetLimit->GetValue())
                *I::pSendPacket = false;
            if (pWeapon)
            {
                bool attack = cmd->buttons & IN_ATTACK && pLocal->CanAttackFull() && pWeapon->CanPrimaryAttack(-0.2f);
                if (attack)
                    *I::pSendPacket = true;
            }
            if (*I::pSendPacket)
                collectedPackets = 0;
            else
                collectedPackets++;
        }
    } // namespace FakeLagModule
} // namespace Client::Module
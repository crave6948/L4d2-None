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
            if (shouldCollectPacket)
            {
                *pSendPacket = false;
                WasSet = true;
            }
            else
            {
                if (collectedPackets < packetLimit->GetValue())
                    *pSendPacket = false;
                if (pWeapon)
                {
                    bool attack = cmd->buttons & IN_ATTACK && pWeapon->CanPrimaryAttack();
                    if (attack)
                        *pSendPacket = true;
                }
            }
            if (*pSendPacket)
            {
                collectedPackets = 0;
                if (WasSet)
                {
                    WasSet = false;
                    *pSendPacket = true;
                    cmd->viewangles = oldAngles;
                }
            }
            else
                collectedPackets++;
        }
    } // namespace FakeLagModule
} // namespace Client::Module
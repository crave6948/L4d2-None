#include "FakeLag.h"

namespace Client::Module
{
    namespace FakeLagModule
    {

        void FakeLag::onPostPrediction(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal)
        {
            if (shouldCollectPacket)
                *pSendPacket = false;
            else
            {
                if (collectedPackets < packetLimit->GetValue())
                    *pSendPacket = false;
                if (pWeapon)
                {
                    bool attack = cmd->buttons & IN_ATTACK && pWeapon->CanPrimaryAttack(-0.2f);
                    if (attack)
                        *pSendPacket = true;
                }
            }
            if (*pSendPacket)
                collectedPackets = 0;
            else
                collectedPackets++;
        }
    } // namespace FakeLagModule
} // namespace Client::Module
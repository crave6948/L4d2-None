#include "AutoShoot.h"
namespace Client::Module
{
	namespace AutoShootModule
	{

		bool AutoShoot::isSniper(int id)
		{
			return (id == WEAPON_AWP || id == WEAPON_SCOUT);
		}
		bool AutoShoot::isShotgun(int id)
		{
			return (id == WEAPON_PUMP_SHOTGUN || id == WEAPON_CHROME_SHOTGUN);
		}
		bool AutoShoot::ShouldRun(C_TerrorPlayer *pLocal, C_TerrorWeapon *pWeapon, CUserCmd *cmd)
		{
			if (cmd->buttons & IN_USE)
				return false;

			// if (pLocal->m_isHangingFromLedge() || pLocal->m_isHangingFromTongue() || pWeapon->m_bInReload() || pLocal->m_isIncapacitated())
			if (pLocal->m_isHangingFromLedge() || pLocal->m_isHangingFromTongue())
				return false;

			// You could also check if the current spread is -1.0f and not run nospread I guess.
			// But since I wanted to filter out shotungs and just be sure that it isnt ran for other stuff I check the weaponid.
			if (!pWeapon)
				return false;
			if (pWeapon->m_bInReload())
				return false;
			switch (pWeapon->GetWeaponID())
			{
			case WEAPON_AWP:
			case WEAPON_DEAGLE:
			case WEAPON_HUNTING_RIFLE:
			case WEAPON_MILITARY_SNIPER:
			case WEAPON_PISTOL:
			case WEAPON_SCOUT:
			case WEAPON_AUTO_SHOTGUN:
			case WEAPON_SPAS:
			case WEAPON_PUMP_SHOTGUN:
			case WEAPON_CHROME_SHOTGUN:
				return true;
			default:
				break;
			}

			return false;
		}
		void AutoShoot::onPostCreateMove(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal)
		{
			isClicking = false;
			if (!ShouldRun(pLocal, pWeapon, cmd))
			{
				keepClicks = 0;
				nextPunch = false;
				return;
			}
			if (nextPunch)
			{
				nextPunch = false;
				bool attack = pWeapon->CanSecondaryAttack();
				if (attack && !(cmd->buttons & IN_ATTACK2))
				{
					cmd->buttons |= IN_ATTACK2;
				}
			}
			if (cmd->buttons & IN_ATTACK)
			{
				cmd->buttons &= ~IN_ATTACK;
				bool attack = pWeapon->CanPrimaryAttack();
				if (!attack)
				{
					nextPunch = false;
				}
				if (attack && keepClicks <= 0)
				{
					keepClicks = keepForTicks->GetValue();
					if (getAutoPunch(pWeapon))
						nextPunch = true;
					cmd->buttons |= IN_ATTACK;
				}
				else
				{
					if (attack && keepClicks <= keepForTicks->GetValue() / 2)
					{
						cmd->buttons &= ~IN_ATTACK;
					}
					if (attack && keepClicks > keepForTicks->GetValue() / 2)
					{
						cmd->buttons |= IN_ATTACK;
					}
				}
			}
			else
			{
				if (keepClicks > 0)
					cmd->buttons |= IN_ATTACK;
				nextPunch = false;
			}
			if (keepClicks > 0)
			{
				keepClicks--;
			}
			isClicking = (cmd->buttons & IN_ATTACK) != 0;
		}
		void AutoShoot::onRender2D()
		{
			if (!Debug->GetValue())
				return;
			std::string str = "AutoShoot: nextPunch-> " + std::string(nextPunch ? "true" : "false") + " || IsClicking-> " + std::string(isClicking ? "true" : "false") + " || KeepClicks-> " + std::to_string(keepClicks);
			G::Draw.String(EFonts::DEBUG, 100, 140, Color(255, 255, 255, 255), TXT_DEFAULT, str.c_str());
		}
		bool AutoShoot::getAutoPunch(C_TerrorWeapon *pWeapon)
		{
			if (!pWeapon)
				return false;
			if (!autoPunch->GetValue())
				return false;
			if (!onlySniper->GetValue() && !onlyShotgun->GetValue())
				return false;
			int id = pWeapon->GetWeaponID();
			if (onlySniper->GetValue() && isSniper(id))
				return true;
			if (onlyShotgun->GetValue() && isShotgun(id))
				return true;
			return false;
		}
	}
}
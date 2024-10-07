#include "Aimbot.h"
#include <functional>
#include "../../../../../Rotation/RotationManager.h"
#include "../../../../../None.h"
#define doGunOrMelee(isGun,isMelee,doGun,doMelee) if (isGun) doGun(); else if (isMelee) doMelee();
namespace Client::Module::AimbotModule
{
	void Aimbot::onPreCreateMove(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal)
	{
		isLeftClicking = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);
		if (!isLeftClicking || !ShouldRun(pLocal, pWeapon, cmd))
		{
			// if we are not left clicking, reset target info
			targetInfo = TargetInfo();
			lastTime = 0;
			hasLeftClickBefore = false;
			return;
		}
		auto [shouldUse, weaponId] = CheckWeapon(pWeapon);
		if (!shouldUse)
			return;

		bool allowedToSwitch = false;
		if (I::GlobalVars->realtime - lastTime >= switchDelay->GetValue() / 1000.f)
		{
			allowedToSwitch = true;
		}
		if (allowedToSwitch)
		{
			bool invalid = true;
			doGunOrMelee(isGun(weaponId), isMelee(weaponId), [&]() {
				invalid = gunAimbot->isInvaildOrDead(pLocal, pWeapon);
			}, [&]() {
				invalid = meleeAimbot->isInvaildOrDead(pLocal, pWeapon);
			});
			if (invalid) {
				targetInfo = TargetInfo();
				doGunOrMelee(isGun(weaponId), isMelee(weaponId), [&]() {
					targetInfo = gunAimbot->GetTarget(pLocal, pWeapon, cmd);
				}, [&]() {
					targetInfo = meleeAimbot->GetTarget(pLocal, pWeapon, cmd);
				});
				lastTime = I::GlobalVars->realtime;
			}
		}
		if (targetInfo.target == nullptr)
		{
			targetInfo = TargetInfo();
			return;
		}
		doGunOrMelee(isGun(weaponId), isMelee(weaponId), [&]() {
			gunAimbot->onPreCreateMove(cmd, pWeapon, pLocal);
		}, [&]() {
			meleeAimbot->onPreCreateMove(cmd, pWeapon, pLocal);
		});
	}
	void Aimbot::onPostCreateMove(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal)
	{
		auto [shouldUse, weaponId] = CheckWeapon(pWeapon);
		if (shouldUse)
		{
			doGunOrMelee(isGun(weaponId), isMelee(weaponId), [&]() {
				gunAimbot->onPostCreateMove(cmd, pWeapon, pLocal);
			}, [&]() {
				meleeAimbot->onPostCreateMove(cmd, pWeapon, pLocal);
			});
		}
		if (!silent->GetValue())
			I::EngineClient->SetViewAngles(cmd->viewangles);
	}
	void Aimbot::onRender2D()
	{
		if (!I::EngineClient->IsInGame())
		{
			return;
		};
		if (I::EngineVGui->IsGameUIVisible())
		{
			return;
		}
		C_TerrorPlayer *pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer *>();
		if (!pLocal) return;
		C_TerrorWeapon *pWeapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon *>();
		auto [shouldUse, weaponId] = CheckWeapon(pWeapon);
		if (shouldUse)
		{
			doGunOrMelee(isGun(weaponId), isMelee(weaponId), [&]() {
				gunAimbot->onRender2D(pLocal, pWeapon);
			}, [&]() {
				meleeAimbot->onRender2D(pLocal, pWeapon);
			});
		}

		// Debug Mode Zone
		if (!debug->GetValue())
			return;
		int startX = 2, startY = 100;
		// แสดงข้อมูล จาก TargetInfo
		int getFontHeight = G::Draw.GetFontHeight(EFonts::DEBUG);
		std::string target_index = targetInfo.target ? std::to_string(targetInfo.target->entindex()) : "None";
		G::Draw.String(EFonts::DEBUG, startX, startY, Color(255, 255, 255, 255), TXT_DEFAULT, target_index.c_str());
		startY += getFontHeight + 1;
		// show target eclientclass
		if (targetInfo.target)
		{
			std::string classname = className(targetInfo.classId);
			G::Draw.String(EFonts::DEBUG, startX, startY, Color(255, 255, 255, 255), TXT_DEFAULT, classname.c_str());
			startY += getFontHeight + 1;
		}
	}
	void Aimbot::onEnabled()
	{
		targetInfo = TargetInfo();
		lastTime = I::GlobalVars->realtime;
		isLeftClicking = false;
	}

	bool Aimbot::ShouldRun(C_TerrorPlayer *pLocal, C_TerrorWeapon *pWeapon, CUserCmd *cmd)
	{
		if (!I::EngineClient->IsInGame())
		{
			return false;
		}
		if (I::EngineVGui->IsGameUIVisible())
		{
			return false;
		}
		auto thirdPerson = Client::client.moduleManager.thirdPerson;
		if (thirdPerson->getEnabled() && thirdPerson->isLocking)
			return false;

		if (cmd->buttons & IN_USE)
			return false;

		// if (!pLocal->CanAttackFull() || pLocal->m_isHangingFromLedge() || pLocal->m_isHangingFromTongue() || pLocal->m_isIncapacitated())
		if (!pLocal->CanAttackFull() || pLocal->m_isHangingFromLedge() || pLocal->m_isHangingFromTongue())
			return false;

		// You could also check if the current spread is -1.0f and not run nospread I guess.
		// But since I wanted to filter out shotungs and just be sure that it isnt ran for other stuff I check the weaponid.
		auto [should, _] = CheckWeapon(pWeapon);

		// check if fastmelee is swaping items
		auto fastMelee = Client::client.moduleManager.fastMelee;
		if (meleeAimbot->meleeOnly->GetValue() && fastMelee->getEnabled() && fastMelee->isSwaping())
			return true;
		return should;
	}
	/**
	 * @returns bool - true if the aimbot should run with this weapon, int - weapon id if it return 9999 it mean no weapon id
	 */
	std::pair<bool, int> Aimbot::CheckWeapon(C_TerrorWeapon *pWeapon)
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
			return std::make_pair<bool, int>(this->gunAimbot->gunOnly->GetValue(), pWeapon->GetWeaponID());
		case WEAPON_MELEE:
		case WEAPON_CHAINSAW:
			return std::make_pair<bool, int>(this->meleeAimbot->meleeOnly->GetValue(), pWeapon->GetWeaponID());
		default:
			break;
		}

		return std::make_pair<bool, int>(false, 9999);
	}
}
#undef doGunOrMelee
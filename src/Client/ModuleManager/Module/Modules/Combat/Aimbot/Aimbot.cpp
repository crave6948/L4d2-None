#include "Aimbot.h"
#include "../../../../../Rotation/RotationManager.h"
#include <functional>
#include "../../../../../None.h"

namespace Client::Module::AimbotModule
{
	void Aimbot::onPreCreateMove(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal)
	{
		shouldPerfect = false;
		isLeftClicking = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);
		if (!isLeftClicking || !ShouldRun(pLocal, pWeapon, cmd))
		{
			targetInfo = TargetInfo();
			lastSwitch = 0;
			lastUpdate = 0;
			hasLeftClickBefore = false;
			bWasSet = false;
			if (rotationMode->GetSelected() == "Instant" || rotationMode->GetSelected() == "PerfectSlient")
			{
				Helper::rotationManager.ForceBack(true);
			}
			return;
		}
		bool allowedToSwitch = false;
		if (I::GlobalVars->realtime - lastSwitch >= switchDelay->GetValue() / 1000.f)
		{
			allowedToSwitch = true;
		}
		if (allowedToSwitch && isInvaildOrDead(pLocal, pWeapon))
		{
			targetInfo = TargetInfo();
			targetInfo = GetTarget(pLocal, pWeapon, cmd);
			lastSwitch = I::GlobalVars->realtime;
		}
		if (targetInfo.target == nullptr)
		{
			targetInfo = TargetInfo();
			return;
		}
		auto [_, weaponId] = CheckWeapon(pWeapon);
		if (!_)
			return;
		Vector hitbox = targetInfo.target->GetBaseEntity()->GetBaseAnimating()->GetHitboxPositionByGroup(targetInfo.hitGroup);
		Vector aimVector = U::Math.GetAngleToPosition(pLocal->Weapon_ShootPosition(), hitbox);
		if (I::GlobalVars->realtime - lastUpdate >= updateDelay->GetValue() / 1000.f)
		{
			targetInfo.aimRotation = Helper::Rotation().toRotation(aimVector);
			targetInfo.targetPosition = hitbox;
			lastUpdate = I::GlobalVars->realtime;
		}
		float distance = pLocal->Weapon_ShootPosition().DistTo(targetInfo.targetPosition);
		bool isInCrosshair = isInCrossHair(cmd, pLocal, targetInfo.target);

		hasLeftClickBefore = true;
		Helper::RotationType type = Helper::RotationType::Normal;
		if (rotationMode->GetSelected() == "Instant" || rotationMode->GetSelected() == "PerfectSlient")
		{
			type = Helper::RotationType::Instant;
			if (isMelee(weaponId))
			{
				if (!bWasSet && pWeapon->CanPrimaryAttack())
				{
					// Helper::rotationManager.moveTo(targetInfo.aimRotation, distance / 571.43f, isInCrosshair, type);
					cmd->viewangles = targetInfo.aimRotation.toVector();
					if (rotationMode->GetSelected() == "PerfectSlient")
					shouldPerfect = true;
					bWasSet = true;
				}
				else if (bWasSet) {
					bWasSet = false;
					// Helper::rotationManager.ForceBack(true);
				}
			}else
			{
				if (!bWasSet && pWeapon->CanPrimaryAttack())
				{
					// Helper::rotationManager.moveTo(targetInfo.aimRotation, distance / 571.43f, isInCrosshair, type);
					cmd->viewangles = targetInfo.aimRotation.toVector();
					if (rotationMode->GetSelected() == "PerfectSlient")
					shouldPerfect = true;
					bWasSet = true;
				}
				else if (bWasSet) {
					bWasSet = false;
					// Helper::rotationManager.ForceBack(true);
				}
			}
		}
		else
			Helper::rotationManager.moveTo(targetInfo.aimRotation, distance / 571.43f, isInCrosshair, type);

		if (cmd->buttons & IN_ATTACK)
		{
			Vector serverSide = Helper::rotationManager.getServerRotationVector();
			float fov = U::Math.GetFovBetween(serverSide, targetInfo.aimRotation.toVector());
			if (isMelee(weaponId))
			{
				if (fov > meleeFovTrigger->GetValue() || distance > meleeRange->GetValue())
					cmd->buttons &= ~IN_ATTACK;
			}
			else if (!fov > gunFovTrigger->GetValue() || distance > gunRange->GetValue())
			{
				cmd->buttons &= ~IN_ATTACK;
			}
			// if (rotationMode->GetSelected() == "PerfectSlient" && cmd->buttons & IN_ATTACK && pLocal->CanAttackFull() && pWeapon->CanPrimaryAttack())
			// 	// Client::client.moduleManager.fakeLag->doCollectPacket();
			// 	shouldPerfect = true;
			if (rotationMode->GetSelected() == "PerfectSlient" || rotationMode->GetSelected() == "Instant")
			{
				if (pWeapon->CanPrimaryAttack())
					cmd->buttons |= IN_ATTACK;
			}
		}
	}
	void Aimbot::onPostCreateMove(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal)
	{
		bool thirdperson_lock = Client::client.moduleManager.thirdPerson->isLocking;
		if (thirdperson_lock)
			return;
		if (rotationMode->GetSelected() == "Legit")
		{
			I::EngineClient->SetViewAngles(cmd->viewangles);
		}
	}
	void Aimbot::onRender2D()
	{
		int startX = 2, startY = 100;
		if (!I::EngineClient->IsInGame())
		{
			return;
		};
		if (I::EngineVGui->IsGameUIVisible())
		{
			return;
		}
		C_TerrorPlayer *pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer *>();
		float flR = tanf(DEG2RAD(gunFov->GetValue()) / 2) / tanf(DEG2RAD(pLocal->IsZoomed() ? 30 : 110) / 2) * G::Draw.m_nScreenW;
		G::Draw.OutlinedCircle(G::Draw.m_nScreenW / 2, G::Draw.m_nScreenH / 2, flR, 32, Color(178, 190, 181, 255));
		if (!debug->GetValue())
			return;
		//(pLocal->IsScoped() && !gVisuals.bNoZoom) ? 30.0f :
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
			Vector screen;
			int size = 6;
			G::Util.W2S(targetInfo.targetPosition, screen);
			const int half = size / 2;
			// top left corner
			G::Draw.Line(screen.x - size, screen.y - size, screen.x - half, screen.y - size, Color(255, 255, 255, 255));
			G::Draw.Line(screen.x - size, screen.y - size, screen.x - size, screen.y - half, Color(255, 255, 255, 255));
			// top right corner
			G::Draw.Line(screen.x + size, screen.y - size, screen.x + half, screen.y - size, Color(255, 255, 255, 255));
			G::Draw.Line(screen.x + size, screen.y - size, screen.x + size, screen.y - half, Color(255, 255, 255, 255));
			// bottom left corner
			G::Draw.Line(screen.x - size, screen.y + size, screen.x - half, screen.y + size, Color(255, 255, 255, 255));
			G::Draw.Line(screen.x - size, screen.y + size, screen.x - size, screen.y + half, Color(255, 255, 255, 255));
			// bottom right corner
			G::Draw.Line(screen.x + size, screen.y + size, screen.x + half, screen.y + size, Color(255, 255, 255, 255));
			G::Draw.Line(screen.x + size, screen.y + size, screen.x + size, screen.y + half, Color(255, 255, 255, 255));
		}
	}
	void Aimbot::onEnabled()
	{
		targetInfo = TargetInfo();
		lastSwitch = I::GlobalVars->realtime;
		lastUpdate = I::GlobalVars->realtime;
		isLeftClicking = false;
		hasLeftClickBefore = false;
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
		if (meleeOnly->GetValue() && fastMelee->getEnabled() && fastMelee->isSwaping())
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
			return std::make_pair<bool, int>(gunOnly->GetValue(), pWeapon->GetWeaponID());
		case WEAPON_MELEE:
		case WEAPON_CHAINSAW:
			return std::make_pair<bool, int>(meleeOnly->GetValue(), pWeapon->GetWeaponID());
		default:
			break;
		}

		return std::make_pair<bool, int>(false, 9999);
	}
	bool Aimbot::isInCrossHair(CUserCmd *cmd, C_TerrorPlayer *pLocal, IClientEntity *target)
	{
		Vector vec = U::Math.AngleVectors(Helper::rotationManager.getServerRotationVector());
		CTraceFilterHitscan filter{pLocal};
		bool shouldhit = false;
		if (auto pHit = G::Util.GetHitEntity(pLocal->Weapon_ShootPosition(), pLocal->Weapon_ShootPosition() + (vec * gunRange->GetValue()), &filter))
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
	bool Aimbot::isInvaildOrDead(C_TerrorPlayer *pLocal, C_TerrorWeapon *pWeapon)
	{
		auto [target, targetPosition, aimRotation, hitGroup, classId] = targetInfo.getTargetInfo();
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
		auto [should, weaponId] = CheckWeapon(pWeapon);
		float aimRange = gunRange->GetValue(), aimfov = gunFov->GetValue();
		bool useMelee = isMelee(weaponId);
		if (useMelee)
		{
			aimRange = meleeRange->GetValue() + meleePreLook->GetValue();
			aimfov = meleeFov->GetValue();
		}
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
	TargetInfo Aimbot::GetTarget(C_TerrorPlayer *pLocal, C_TerrorWeapon *pWeapon, CUserCmd *cmd)
	{
		auto [should, weaponId] = CheckWeapon(pWeapon);
		bool useMelee = isMelee(weaponId);
		float aimRange = gunRange->GetValue(), aimFov = gunFov->GetValue();
		if (useMelee)
		{
			aimRange = meleeRange->GetValue() + meleePreLook->GetValue();
			aimFov = meleeFov->GetValue();
		}
		IClientEntity *foundTarget = nullptr;
		// collect all targets and find the best one (compare them by a score)
		Vector clientViewAngles = Helper::rotationManager.getServerRotationVector();
		if (Helper::rotationManager.DisabledRotation || !hasLeftClickBefore)
			I::EngineClient->GetViewAngles(clientViewAngles);
		float currentScore = 1000.f;
		const auto updateTarget = [&](IClientEntity *target, float fov, float distance) -> bool
		{
			float fovScore = (fov * 100) / aimFov;
			float distanceScore = (distance * 100) / aimRange;
			float score = (sortModes->GetSelected() == "Fov") ? fovScore : (sortModes->GetSelected() == "Distance") ? distanceScore
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
			if (useMelee)
			{
				if (classType == EClientClass::Infected)
				{
					return HITGROUP_CHEST;
				}
			}
			return HITGROUP_HEAD;
		};
		const auto GetFovDistance = [&, clientViewAngles](IClientEntity *target, int classType) -> std::pair<float, float>
		{
			Vector src = pLocal->Weapon_ShootPosition();
			Vector dst = target->GetBaseEntity()->GetBaseAnimating()->GetHitboxPositionByGroup(GetHitbox(classType));
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

			Vector hitbox = target->GetBaseEntity()->GetBaseAnimating()->GetHitboxPositionByGroup(GetHitbox(classType));
			CTraceFilterHitscan filter{pLocal};
			auto pHit{G::Util.GetHitEntity(pLocal->Weapon_ShootPosition(), hitbox, &filter)};
			if (!pHit || pHit->entindex() != target->entindex())
				return false;
			return true;
		};
		for (auto &[enabled, classType] : entityTypes)
		{
			if (enabled())
			{
				for (auto entity : Utils::g_EntityCache.getEntityFromGroup(classType))
				{
					if (!checkCondition(entity, classType))
						continue;
					if (classType == EClientClass::Witch && this->witchRage->GetValue() && entity->As<C_Witch *>()->m_rage() != 1.0f)
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
		Vector hitbox = foundTarget->GetBaseEntity()->GetBaseAnimating()->GetHitboxPositionByGroup(hitgroup);
		Vector aimVector = U::Math.GetAngleToPosition(pLocal->Weapon_ShootPosition(), hitbox);
		return TargetInfo(foundTarget, hitbox, Helper::Rotation().toRotation(aimVector), hitgroup, classid);
	}
}

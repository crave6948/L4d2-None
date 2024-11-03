#pragma once
#include "../../../ModuleHeader.h"

namespace Client::Module
{
	namespace AutoShootModule
	{
		class AutoShoot : public Module
		{
		public:
			AutoShoot()
			{
				this->Create("AutoShoot", true, VK_NUMPAD2, ModuleCategory::Combat);
				vManager.AddValue(autoPunch);
				vManager.AddValue(onlySniper);
				vManager.AddValue(onlyShotgun);

				vManager.AddValue(keepForTicks);
				vManager.AddValue(clickCps);
				vManager.AddValue(allowOtherGuns);
				vManager.AddValue(startDelay);

				vManager.AddValue(Debug);
			};
			void RenderValueGui() override
			{
				BooleanCheckBox(autoPunch);
				BooleanCheckBox(onlySniper);
				BooleanCheckBox(onlyShotgun);
				FloatRange(keepForTicks);
				FloatRange(clickCps);
				BooleanCheckBox(allowOtherGuns);
				FloatSlider(startDelay);
				
				BooleanCheckBox(Debug);
			}
			// autoPunch
			V::BooleanValue *autoPunch = new V::BooleanValue("AutoPunch", true);
			// onlySniper
			V::BooleanValue *onlySniper = new V::BooleanValue("OnlySniper", true);
			// onlyShotgun
			V::BooleanValue *onlyShotgun = new V::BooleanValue("OnlyShotgun", true);
			// waitForTicks
			V::FloatRangeValue *keepForTicks = new V::FloatRangeValue("KeepForTicks", V::Range(0.05,0.08), V::Range(0.01, 1), "sec");
			// ClickCPS
			V::FloatRangeValue *clickCps = new V::FloatRangeValue("ClickCps", V::Range(10, 20), V::Range(1, 20), "cps");
			// allowOtherGunToAutoClick
			V::BooleanValue *allowOtherGuns = new V::BooleanValue("AllowOtherGuns", true);
			// startDelay FloatValue
			V::FloatValue *startDelay = new V::FloatValue("StartDelay", 0.3f, 0.0f, 2.0f,"sec");
			// Debug
			V::BooleanValue *Debug = new V::BooleanValue("Debug", false);

			void onPostCreateMove(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal) override;
			void onRender2D() override;
			bool getAutoPunch(C_TerrorWeapon *pWeapon);

		private:
			float lastAttackTime = 0, lastKeepClicks = 0, lastDelay = 0;
			float keepClicks = 0;
			bool nextPunch = false;
			bool isSniper(int id);
			bool isShotgun(int id);
			bool ShouldRun(C_TerrorPlayer *pLocal, C_TerrorWeapon *pWeapon, CUserCmd *cmd);
			bool isClicking = false;
		};
	}
};

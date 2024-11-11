#pragma once
#include "../../../ModuleHeader.h"

namespace Client::Module
{
	namespace FakeLagModule
	{
		class FakeLag : public Module
		{
		public:
			FakeLag()
			{
				this->Create("FakeLag", false, 0, ModuleCategory::Misc);
				vManager.AddValue(packetLimit);
			};
			void RenderValueGui() override
			{
				IntegerSlider(packetLimit);
			}
			V::NumberValue *packetLimit = new V::NumberValue("Packet Limit", 14, 1, 20);

			int collectedPackets = 0;
			bool shouldCollectPacket = false;

			void doCollectPacket() { shouldCollectPacket = true; };
			void onPreCreateMove(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal) override;
			void onPostPrediction(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal) override;

		private:
			Vector oldAngles = Vector(0, 0, 0);
		};
	};
};
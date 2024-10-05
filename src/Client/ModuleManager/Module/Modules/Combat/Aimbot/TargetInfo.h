#pragma once
#include "../../../ModuleHeader.h"
#include "../../../../../Rotation/RotationManager.h"
namespace Client::Module
{
    namespace AimbotModule
    {
        class TargetInfo
        {
        public:
            IClientEntity *target = nullptr;
            Vector targetPosition;
            Helper::Rotation aimRotation;
            int hitGroup;
            int classId;
            TargetInfo()
            {
                // clear target info
                this->target = nullptr;
                this->targetPosition = Vector(0, 0, 0);
                this->aimRotation = Helper::Rotation();
                this->hitGroup = 0;
                this->classId = 0;
            }
            TargetInfo(IClientEntity *target, Vector targetPosition, Helper::Rotation aim, int hitGroup, int classId)
            {
                this->target = target;
                this->targetPosition = targetPosition;
                this->aimRotation = aim;
                this->hitGroup = hitGroup;
                this->classId = classId;
            }
            auto getTargetInfo()
            {
                return std::make_tuple(this->target, this->targetPosition, this->aimRotation, this->hitGroup, this->classId);
            }
        };
    }
}
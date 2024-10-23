#pragma once
#include <string>
#include "../../../SDK/SDK.h"
#include "Utils/UtilsCollector.h"
#include "../../Value/ValueManager.h"
#include "ModuleCategory.h"
#include "../../../Etc/Imgui/imgui.h"
namespace Client::Module
{
    class Module
    {
    public:
        V::ValueManager vManager = V::ValueManager();
        virtual void RenderValueGui() {};

        void Create(std::string name, bool state, int keyCode, ModuleCategory category)
        {
            this->name = name;
            this->state = state;
            this->key = keyCode;
            this->category = category;
        };
        // Get Name of this Module
        std::string getName()
        {
            return name;
        };
        bool state = false;
        // Get Enabled State of this Module
        bool getEnabled()
        {
            return state;
        };
        void setEnabled(bool state)
        {
            this->state = state;
            state ? onEnabled() : onDisabled();
        };
        int getKey()
        {
            return key;
        };
        void setKey(int key)
        {
            this->key = key;
        };
        ModuleCategory getCategory()
        {
            return category;
        };
        virtual void onPreCreateMove(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal) {};
        virtual void onPostCreateMove(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal) {};
        virtual void onPrePrediction(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal) {};
        virtual void onPrediction(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal, int PredictedFlags) {};
        virtual void onPostPrediction(CUserCmd *cmd, C_TerrorWeapon *pWeapon, C_TerrorPlayer *pLocal) {};
        virtual void onRender2D() {};
        virtual void onFrameStageNotify(ClientFrameStage_t curStage) {};
        void toggle();
        virtual void onEnabled() {};
        virtual void onDisabled() {};
        int keytimeout = 0;
        bool ShouldToggle()
        {
            return keytimeout <= 0;
        }
        int animate = 10;
        void updateanimate()
        {
            if (state)
            {
                if (animate > 0)
                {
                    animate--;
                }
            }
            else
            {
                animate = 10;
            }
        }

        void FloatSlider(V::FloatValue *value)
        {
            ImGui::DragFloat(value->GetName().c_str(), &value->m_Value, 0.005f, value->GetMin(), value->GetMax(), ("%0.2f" + value->GetFormat()).c_str(), ImGuiSliderFlags_AlwaysClamp);
        }
        void IntegerSlider(V::NumberValue *value)
        {
            ImGui::DragInt(value->GetName().c_str(), &value->m_Value, 0.005f, value->GetMin(), value->GetMax(), ("%d" + value->GetFormat()).c_str(), ImGuiSliderFlags_AlwaysClamp);
        }
        void ListBox(V::ListValue *value)
        {
            const auto lists = value->GetLists();
            // Create an array to store the const char* pointers
            const char **c_lists = new const char *[lists.size()];

            // Populate the array
            for (size_t i = 0; i < lists.size(); ++i)
            {
                c_lists[i] = lists[i].c_str();
            }
            ImGui::ListBox(value->GetName().c_str(), &value->index, c_lists, lists.size());
            delete c_lists;
        }
        void FloatRange(V::FloatRangeValue *value)
        {
            auto [min, max] = value->GetMaximumRange();
            ImGui::DragFloatRange2(value->GetName().c_str(), &value->m_Value.min, &value->m_Value.max, 0.05F, min, max, ("Min: %.2f" + value->GetFormat()).c_str(), ("Max: %.2f" + value->GetFormat()).c_str(), ImGuiSliderFlags_AlwaysClamp);
        }
        void BooleanCheckBox(V::BooleanValue *value) {
            ImGui::Checkbox(value->GetName().c_str(), &value->m_Value);
        }
        void ColorEditer(V::ColorValue *value) {
            ImGui::ColorEdit4(value->GetName().c_str(), value->color, ImGuiColorEditFlags_NoInputs);
        }
    private:
        std::string name;
        int key = 0;
        ModuleCategory category;
    };
}
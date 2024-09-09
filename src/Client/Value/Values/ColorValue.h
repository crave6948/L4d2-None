#pragma once
#include "NumberValue.h"

namespace V
{
    class ColorValue : public ValueBase
    {
    public:
        float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
        ColorValue(std::string name, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.f) : m_Name(name)
        {
            color[0] = r;
            color[1] = g;
            color[2] = b;
            color[3] = a;
        };
        std::string GetName() const override { return m_Name; }
        std::tuple<float, float, float, float> GetValue() { return std::make_tuple(color[0], color[1], color[2], color[3]); }
        // setValue by parameter tuple
        void SetValue(std::tuple<float, float, float, float> value)
        {
            auto [r, g, b, a] = value;
            #define isInvalid(c) c < 0 || c > 1
            if (isInvalid(r) || isInvalid(g) || isInvalid(b) || isInvalid(a)) return;
            color[0] = r;
            color[1] = g;
            color[2] = b;
            color[3] = a;
        };

    private:
        std::string m_Name;
    };
}
#pragma once
#include "FloatValue.h"
#include <vector>
namespace V
{
    class ListValue : public ValueBase
    {
    public:
        ListValue(std::string name, std::vector<std::string> lists, std::string selected = "") : m_Name(name), m_Lists(lists)
        {
            for (int i = 0; i < lists.size(); i++)
            {
                if (selected == lists[i])
                    index = i;
            }
        }
        std::string GetName() const override { return m_Name; }
        std::string GetSelected() { return m_Lists[index]; }
        void SetSelected(std::string selected)
        {
            for (int i = 0; i < m_Lists.size(); i++)
            {
                if (selected == m_Lists[i])
                    index = i;
            }
        }
        std::vector<std::string> GetLists() { return m_Lists; }
        int index = 0;
    private:
        std::string m_Name;
        std::vector<std::string> m_Lists;
    };
}
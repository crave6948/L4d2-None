#pragma once

#include "../../SDK/SDK.h"
#include "../../Client/None.h"

namespace Hooks
{
	namespace DirectHook
    {
        inline Hook::CTable Table;

        namespace SceneEnd
		{
			using FN = long(__thiscall *)(void *, IDirect3DDevice9*) noexcept;
			constexpr uint32_t Index = 42u;

			long __stdcall Detour(IDirect3DDevice9* device) noexcept;
		}

		namespace Reset
		{
			using FN = HRESULT(__thiscall *)(void *, IDirect3DDevice9*, D3DPRESENT_PARAMETERS*) noexcept;
			constexpr uint32_t Index = 16u;

			HRESULT __stdcall Detour(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept;
		}

        void Init();
    }
}
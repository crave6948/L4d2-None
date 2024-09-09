#include "DirectHook.h"

#include "../../Etc/Imgui/imgui_impl_dx9.h"
#include <intrin.h>
void Hooks::DirectHook::Init()
{
    XASSERT(Table.Init(Client::Menu::device) == false);
    XASSERT(Table.Hook(&SceneEnd::Detour, SceneEnd::Index) == false);
    XASSERT(Table.Hook(&Reset::Detour, Reset::Index) == false);

    Client::Menu::DestroyDirectX();
}

long __stdcall Hooks::DirectHook::SceneEnd::Detour(IDirect3DDevice9 *device) noexcept
{
    static const auto returnAddress = _ReturnAddress();
    const auto result = Table.Original<FN>(Index)(device, device);

    if (_ReturnAddress() == returnAddress)
        return result;
    if (!Client::Menu::setup)
        Client::Menu::SetupMenu(device);
    if (Client::Menu::open)
        Client::Menu::Render();
    return result;
}

HRESULT __stdcall Hooks::DirectHook::Reset::Detour(IDirect3DDevice9 *device, D3DPRESENT_PARAMETERS *params) noexcept
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    const auto result = Table.Original<FN>(Index)(device, device, params);
    ImGui_ImplDX9_CreateDeviceObjects();
    return result;
}

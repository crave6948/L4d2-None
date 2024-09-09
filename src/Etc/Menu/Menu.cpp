#include "Menu.h"
#include "../Imgui/imgui.h"
#include "../Imgui/imgui_impl_win32.h"
#include "../Imgui/imgui_impl_dx9.h"

#include <stdexcept>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam);

LRESULT CALLBACK WindowProcess(
    HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam);

bool Client::Menu::SetupWindowClass(const char *windowClassName) noexcept
{
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = DefWindowProc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = GetModuleHandle(NULL);
    windowClass.hIcon = NULL;
    windowClass.hCursor = NULL;
    windowClass.hbrBackground = NULL;
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = windowClassName;
    windowClass.hIconSm = NULL;

    if (!RegisterClassEx(&windowClass))
        return false;
    return true;
}

void Client::Menu::DestroyWindowClass() noexcept
{
    UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

bool Client::Menu::SetupWindow(const char *windowName) noexcept
{
    window = CreateWindow(
        windowClass.lpszClassName,
        windowName,
        WS_OVERLAPPEDWINDOW,
        0, 0, 100, 100, 0, 0,
        windowClass.hInstance,
        0);
    if (!window)
        return false;
    return true;
}

void Client::Menu::DestroyWindow() noexcept
{
    if (window)
        DestroyWindow(window);
}

bool Client::Menu::SetupDirectX() noexcept
{
    const auto handle = GetModuleHandle("d3d9.dll");
    if (!handle)
        return false;
    using CreateFn = LPDIRECT3D9(__stdcall *)(UINT);
    const auto create = reinterpret_cast<CreateFn>(GetProcAddress(
        handle,
        "Direct3DCreate9"));
    if (!create)
        return false;
    d3d9 = create(D3D_SDK_VERSION);
    if (!d3d9)
        return false;
    D3DPRESENT_PARAMETERS params = {};
    params.BackBufferWidth = 0;
    params.BackBufferHeight = 0;
    params.BackBufferFormat = D3DFMT_UNKNOWN;
    params.BackBufferCount = 0;
    params.MultiSampleType = D3DMULTISAMPLE_NONE;
    params.MultiSampleQuality = NULL;
    params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    params.hDeviceWindow = window;
    params.Windowed = 1;
    params.EnableAutoDepthStencil = 0;
    params.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
    params.Flags = NULL;
    params.FullScreen_RefreshRateInHz = 0;
    params.PresentationInterval = 0;

    if (d3d9->CreateDevice(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_NULLREF,
            window,
            D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT,
            &params,
            &device) < 0)
        return false;
    return true;
}

void Client::Menu::DestroyDirectX() noexcept
{
    if (device)
    {
        device->Release();
        device = NULL;
    }
    if (d3d9)
    {
        d3d9->Release();
        d3d9 = NULL;
    }
}

void Client::Menu::Setup()
{
    if (!SetupWindowClass("testClass001"))
        throw std::runtime_error("Failed to create window class.");
    if (!SetupWindow("testWindow"))
        throw std::runtime_error("Failed to create window.");
    if (!SetupDirectX())
        throw std::runtime_error("Failed to create device.");

    DestroyWindow();
    DestroyWindowClass();
}

void Client::Menu::SetupMenu(LPDIRECT3DDEVICE9 device) noexcept
{
    auto params = D3DDEVICE_CREATION_PARAMETERS{};
    device->GetCreationParameters(&params);

    window = params.hFocusWindow;
    oWindowProcess = reinterpret_cast<WNDPROC>(
        SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProcess)));

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX9_Init(device);

    setup = true;
}

void Client::Menu::Destroy() noexcept
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(oWindowProcess));

    DestroyDirectX();
}

void Client::Menu::Render() noexcept
{
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    AddContextToMenu();

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void Client::Menu::AddContextToMenu()
{
    const auto AddListBox = [](V::ListValue *v)
    {
        const auto lists = v->GetLists();
        // Create an array to store the const char* pointers
        const char **c_lists = new const char *[lists.size()];

        // Populate the array
        for (size_t i = 0; i < lists.size(); ++i)
        {
            c_lists[i] = lists[i].c_str();
        }
        ImGui::ListBox(v->GetName().c_str(), &v->index, c_lists, lists.size());
        delete c_lists;
    };
    const auto makeModuleList = [&](std::string categoryName, Client::Module::ModuleCategory category){
        if (ImGui::BeginTabBar(categoryName.c_str()))
        {
            auto combat = Client::client.moduleManager.getFeatureListByCategory(category);
            for (auto module : combat)
            {
                auto name = module->getName();
                if (ImGui::BeginTabItem(name.c_str()))
                {
                    ImGui::Text("%s", name.c_str());
                    ImGui::Checkbox("Enabled", &module->state);
                    for (auto value : module->vManager.GetValues())
                    {
                        if (auto booleanValue = dynamic_cast<V::BooleanValue *>(value))
                        {
                            ImGui::Checkbox(booleanValue->GetName().c_str(), &booleanValue->m_Value);
                        }
                        else if (auto listValue = dynamic_cast<V::ListValue *>(value))
                        {
                            AddListBox(listValue);
                        }
                        else if (auto numberValue = dynamic_cast<V::NumberValue *>(value))
                        {
                            ImGui::SliderInt(numberValue->GetName().c_str(), &numberValue->m_Value, numberValue->GetMin(), numberValue->GetMax(), ("%d" + numberValue->GetFormat()).c_str(), ImGuiSliderFlags_AlwaysClamp);
                        }
                        else if (auto floatValue = dynamic_cast<V::FloatValue *>(value))
                        {
                            ImGui::SliderFloat(floatValue->GetName().c_str(), &floatValue->m_Value, floatValue->GetMin(), floatValue->GetMax(), ("%0.2f" + floatValue->GetFormat()).c_str(), ImGuiSliderFlags_AlwaysClamp);
                        }
                        else if (auto floatRangeValue = dynamic_cast<V::FloatRangeValue *>(value))
                        {
                            auto [min, max] = floatRangeValue->GetMaximumRange();
                            ImGui::DragFloatRange2(floatRangeValue->GetName().c_str(), &floatRangeValue->m_Value.min, &floatRangeValue->m_Value.max, 0.05F, min, max, ("Min: %.2f" + floatRangeValue->GetFormat()).c_str(), ("Max: %.2f" + floatRangeValue->GetFormat()).c_str(), ImGuiSliderFlags_AlwaysClamp);
                        }
                        else if (auto colorValue = dynamic_cast<V::ColorValue *>(value))
                        {
                            ImGui::ColorEdit4(colorValue->GetName().c_str(), colorValue->color, ImGuiColorEditFlags_NoInputs);
                        }
                    }
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }
    };

    ImGui::Begin("cool menu", &open);
    auto list = Client::client.moduleManager.featurelist;
    std::vector<std::pair<std::string,Client::Module::ModuleCategory>> categoryList = {
        {"Combat", Client::Module::ModuleCategory::Combat},
        {"Visuals", Client::Module::ModuleCategory::Visuals},
        {"Player", Client::Module::ModuleCategory::Player},
        {"Misc", Client::Module::ModuleCategory::Misc}
    };
    if (ImGui::BeginTabBar("category"))
    {
        for (auto [categoryName, category] : categoryList)
        {
            if (ImGui::BeginTabItem(categoryName.c_str()))
            {
                makeModuleList(categoryName, category);
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
    ImGui::ShowDemoWindow();
}

LRESULT CALLBACK WindowProcess(
    HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    if (GetAsyncKeyState(VK_RSHIFT) & 1)
        Client::Menu::open = !Client::Menu::open;
    if (Client::Menu::open && ImGui_ImplWin32_WndProcHandler(
                                  hWnd,
                                  msg,
                                  wParam,
                                  lParam))
        return true;
    return CallWindowProc(
        Client::Menu::oWindowProcess,
        hWnd,
        msg,
        wParam,
        lParam);
};
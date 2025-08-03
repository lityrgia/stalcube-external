#include <iostream>
#include <fstream>
#include <thread>
#include <filesystem>

#include "cheat.h"
#include "overlay.h"
#include "options.h"
#include "gui.h"
#include "aimbot.h"
#include "json.h"
#include "uiaccess.h"

// isTeam в будущем
// вообще думая наперёд, лучше было не делать кучу разных Classes, вместо этого создать один универсальный класс для классов java, основанный на .json

void createConsole() {
    AllocConsole();

    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);
}

std::thread cheatThread;

BOOL enableDebugPrivilege(BOOL bEnable)
{
    HANDLE hToken = nullptr;
    LUID luid;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) return FALSE;
    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) return FALSE;

    TOKEN_PRIVILEGES tokenPriv;
    tokenPriv.PrivilegeCount = 1;
    tokenPriv.Privileges[0].Luid = luid;
    tokenPriv.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;

    if (!AdjustTokenPrivileges(hToken, FALSE, &tokenPriv, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) return FALSE;

    return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    createConsole();
    system("title External cheat output");
    menu::consoleAllocated = true;
    
    enableDebugPrivilege(true);

    Cheat cheat;

    if (!cheat.init()) {
        printf("[cheat]: Failed init cheat\n");
        system("pause");
        return 0;
    }

    std::ifstream offsetsFile(std::filesystem::path(u8"dump_classes.json"));

    if (offsetsFile.is_open()) {
        offsetsFile >> j::cfg;
        offsetsFile.close();
    }
    else {
        printf("[cheat]: Failed to open dump_classes.json");
        system("pause");
        return 0;
    }

    printf("[cheat]: Starting cheat loop\n");

    cheatThread = std::thread(&Cheat::processCheat, std::ref(cheat));
    cheatThread.detach();

    printf("[cheat]: Starting overlay loop\n");

    Overlay overlay;

    if (!overlay.CreateOverlay()) {
        printf("[cheat]: Failed to create overlay\n");
        system("pause");
        return 0;
    }

    printf("[cheat]: Succesfly created overlay\n");

    float centerX = options::monitorX / 2.f;
    float centerY = options::monitorY / 2.f;

    ImGui::CreateContext();
    setupFonts();
    setImguiStyle();
    overlay.imguiInit();

    MSG msg;
    while (overlay.done == false)
    {
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
            {
                overlay.done = true;
                break;
            }
        }

        if (overlay.done == true || GetAsyncKeyState(menu::bEndButton) & 1) break;

        if (GetAsyncKeyState(menu::bToggleButton) & 1)  menu::bMenuVisible = !menu::bMenuVisible;
        if (GetAsyncKeyState(options::aimbotButton) & 1)  options::bAimbotEnabled = !options::bAimbotEnabled;
        if (GetAsyncKeyState(options::espButton) & 1)  options::bEspEnabled = !options::bEspEnabled;
        if (GetAsyncKeyState(options::triggerbotButton) & 1)  options::bTriggerbotEnabled = !options::bTriggerbotEnabled;

        overlay.MakeNewFrame();

        LONG ex = GetWindowLong(overlay.hwnd, GWL_EXSTYLE);
        if (menu::bMenuVisible) {
            guiStart();
            ex &= ~WS_EX_TRANSPARENT;
        }
        else {
            ex |= WS_EX_TRANSPARENT; 
        }
        SetWindowLong(overlay.hwnd, GWL_EXSTYLE, ex);
        SetWindowPos(overlay.hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);


        if (options::bTriggerbotEnabled) processTriggerbot();

        if (options::bEspEnabled) renderMain();

        if (options::bAimbotEnabled) {
            if (options::aimTypeButton == 0) {
                processAimbot();
            }
            else {
                int mouseButton = (options::aimTypeButton == 1) ? VK_LBUTTON : VK_RBUTTON;

                if (GetAsyncKeyState(mouseButton) & 0x8000) processAimbot();
            }
        }
        if (options::bFovEnabled) {
            ImGui::GetBackgroundDrawList()->AddCircle(ImVec2{ centerX, centerY }, options::radiusFOV, ImColor(255, 255, 255, 255), 25, 1.f);
        }

        overlay.RenderGui();
    }

    overlay.ReleaseOverlay();
}
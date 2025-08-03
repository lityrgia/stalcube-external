#include <windows.h>
#include <vector>
#include <mutex>

#include "types.h"
#include "CEntity.h"
#include "aimbot.h"
#include "cheat.h"
#include "options.h"

#define M_PI 3.14159265358979323846

void moveMouseByOffset(int offsetX, int offsetY) {
    POINT pt;
    if (GetCursorPos(&pt)) {
        int newX = pt.x + offsetX;
        int newY = pt.y + offsetY;

        mouse_event(MOUSEEVENTF_MOVE, offsetX, offsetY, 0, 0);
    }
}

void processAimbot() {
    CEntity closest_entity_copy;

    {
        std::lock_guard<std::mutex> lock(gameDataMutex);
        closest_entity_copy = closest_entity;
    }

    float left = closest_entity_copy.w2sbb.x;
    float top = closest_entity_copy.w2sbb.y;
    float right = closest_entity_copy.w2sbb.z;
    float bottom = closest_entity_copy.w2sbb.w;

    float centerX = (left + right) / 2.0f;
    float height = bottom - top;
    float verticalOffset = height * 0.15f;

    float targetY = top + verticalOffset;

    vec2 screenCenter{ options::monitorX / 2.f, options::monitorY / 2.f };

    if (closest_entity_copy.obj != nullptr) {
        moveMouseByOffset(
            -(screenCenter.x - centerX) / options::smoothness,
            -(screenCenter.y - targetY) / options::smoothness
        );
    }
}

void processTriggerbot() {
    bool triggered_copy;

    {
        std::lock_guard<std::mutex> lock(gameDataMutex);
        triggered_copy = triggered_ent;
    }

    if (triggered_copy) {
        static std::chrono::time_point lastTimePoint = std::chrono::steady_clock::now();
        auto curTimePoint = std::chrono::steady_clock::now();
        if (curTimePoint - lastTimePoint >= std::chrono::milliseconds(options::triggerbotDelay))
        {
            const bool shooting = GetAsyncKeyState(VK_LBUTTON) < 0;
            if (!shooting)
            {
                mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
            }

            lastTimePoint = curTimePoint;
        }
    }
}
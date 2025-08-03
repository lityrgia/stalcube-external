#include <vector>
#include <iostream>

#include "util.h"
#include "CEntity.h"
#include "options.h"

void findClosestToCenter(const std::vector<CEntity>& entities, CEntity& closest_entity) {
    float centerX = options::monitorX / 2.0f;
    float centerY = options::monitorY / 2.0f;

    float closestDist = FLT_MAX;
    const CEntity* closestEntity = nullptr;

    for (const auto& entity : entities) {
        if (entity.type != CEntity::Type::Player && entity.type != CEntity::Type::NPC) continue;

        vec4 pos = entity.w2sbb;

        if (pos.z < 0.01f || pos.w < 0.01f)
            continue;

        float dx = pos.x - centerX;
        float dy = pos.y - centerY;
        float dist = std::sqrt(dx * dx + dy * dy);

        if (!options::bFovEnabled || dist < options::radiusFOV) {
            if (dist < closestDist) {
                closestDist = dist;
                closestEntity = &entity;
            }
        }
    }

    if (closestEntity) {
        closest_entity = *closestEntity;
    }
    else {
        closest_entity = nullptr;
    }
}

std::string u16_to_utf8(const std::u16string& u16str) {
    if (u16str.empty()) return {};

    int size_needed = WideCharToMultiByte(
        CP_UTF8, 0,
        reinterpret_cast<LPCWCH>(u16str.data()),
        (int)u16str.size(),
        nullptr, 0, nullptr, nullptr);

    if (size_needed == 0) {
        return {};
    }

    std::string utf8str(size_needed, 0);

    WideCharToMultiByte(
        CP_UTF8, 0,
        reinterpret_cast<LPCWCH>(u16str.data()),
        (int)u16str.size(),
        utf8str.data(),
        size_needed,
        nullptr, nullptr);

    //gpt code

    return utf8str;
}

vec4 multiply(const vec4& vec, const float mat[16]) {
    return vec4{
        vec.x * mat[0] + vec.y * mat[4] + vec.z * mat[8] + vec.w * mat[12],
        vec.x * mat[1] + vec.y * mat[5] + vec.z * mat[9] + vec.w * mat[13],
        vec.x * mat[2] + vec.y * mat[6] + vec.z * mat[10] + vec.w * mat[14],
        vec.x * mat[3] + vec.y * mat[7] + vec.z * mat[11] + vec.w * mat[15]
    };
}

bool worldToScreen(const vec3& pointInWorld, _Out_ vec2& ndc, const float mvp[16]) {
    vec4 point4{ pointInWorld.x, pointInWorld.y, pointInWorld.z, 1.0f };
    vec4 clipSpacePos = multiply(point4, mvp);

    if (clipSpacePos.w == 0.0f)
        return false;

    float ndcX = clipSpacePos.x / clipSpacePos.w;
    float ndcY = clipSpacePos.y / clipSpacePos.w;
    float ndcZ = clipSpacePos.z / clipSpacePos.w;

    if (ndcZ < -1.0f || ndcZ > 1.0f)
        return false;

    ndc.x = (ndcX + 1.0f) * 0.5f * options::monitorX;
    ndc.y = (1.0f - ndcY) * 0.5f * options::monitorY;

    return true;
}
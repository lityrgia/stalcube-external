#pragma once
#include <string>
#include "types.h"

class CEntity;

std::string u16_to_utf8(const std::u16string& u16str);
void findClosestToCenter(const std::vector<CEntity>& entities, CEntity& closest_entity);
bool worldToScreen(const vec3& pointInWorld, _Out_ vec2& ndc, const float mvp[16]);
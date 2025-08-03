#pragma once

#include "jimmy.h"
#include "types.h"
#include <string>

#include "CDataWatcher.h"

class CEntity
{
public:
	enum Type {
		Player,
		NPC,
		GAME_ITEMS,
		Troop,
		Mutant,
		Item,
		Unknown
	};
public:
	Jclass cls{ nullptr };
	Jobj obj;
	CDataWatcher dataWatcher;
	BBox3D bbox_3d{};
	vec4 w2sbb{ FLT_MAX, FLT_MAX, -1.f, -1.f };
	vec3 lastTickPos{};
	vec3 pos{};
	Type type;
	std::u16string name{u"Unknown"};
	float health{0};
	int maxHealth{0};
	bool dead;
	bool isTeam{0};
public:
	CEntity() {}

	CEntity(Jobj obj) : obj(obj) {
		if (obj != nullptr) cls = jmy_getobjectclass(obj);
	}

	bool isDead();
	BBox3D getBoundingBox();
	vec3 getLastTickPos();
	vec3 getPos();
	double getAttribute(const std::u16string& attribute);
	Jobj getDataWatcher();
	std::u16string getName();
};


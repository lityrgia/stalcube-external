#pragma once
#include <vector>

#include "jimmy.h"
#include "CEntity.h"
#include "ClassLoader.h"
#include "json.h"

class CClientWorld
{
public:
	Jobj obj;
	Jclass cls{nullptr};
	ClassLoader& cl;
public:
	CClientWorld(Jobj obj, ClassLoader& cl) : obj(obj), cl(cl) {
		cls = jmy_getobjectclass(obj);
	}

	bool getListEntities(std::vector<CEntity>& entities);
};


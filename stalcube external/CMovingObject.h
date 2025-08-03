#pragma once

#include "jimmy.h"
#include "json.h"

class CMovingObject
{
public:
	Jclass cls{ nullptr };
	Jobj obj;
public:
	CMovingObject(Jobj obj) : obj(obj) {
		cls = jmy_getobjectclass(obj);
	}

	Jobj getEntityHit();
};


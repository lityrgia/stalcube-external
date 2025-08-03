#pragma once

#include "types.h"
#include "jimmy.h"
#include "json.h"

class CTimer
{
public:
	Jclass cls{ nullptr };
	Jobj obj;
public:
	CTimer(Jobj obj) : obj(obj) {
		if (obj != nullptr) cls = jmy_getobjectclass(obj);
	}

	float getRenderPartialTicks();
};


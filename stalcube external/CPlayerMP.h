#pragma once

#include "jimmy.h"
#include "types.h"
#include "json.h"

class CPlayerMP
{
public:
	Jclass cls{nullptr};
	Jobj obj;
public:
	CPlayerMP(Jobj obj) : obj(obj) {
		cls = jmy_getobjectclass(obj);
	}

	vec3 getPosition();
	vec2 getAngles();
};


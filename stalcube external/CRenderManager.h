#pragma once

#include "jimmy.h"
#include "types.h"
#include "cheat.h"
#include "ClassLoader.h"
#include "json.h"
#include "options.h"

//static
class CRenderManager
{
public:
	Jclass cls{ nullptr };
	ClassLoader& cl;
public:
	CRenderManager(ClassLoader& cl) : cl(cl) {
		cls = findClsFromCl(j::cfg["RenderManager"]["obf"], cl.obj);
		//std::cout << "RenderManager cls: " << cls << '\n';
	}

	void getRenderPos(vec3& pos);
};


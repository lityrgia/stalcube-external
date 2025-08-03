#pragma once

#include "jimmy.h"
#include "cheat.h"
#include "ClassLoader.h"
#include "json.h"
#include "options.h"

//юкнннн EADG.PKK

class CMinecraft
{
public:
	Jclass cls;
	Jid minecraft_fid;
	Jobj obj;
public:
	CMinecraft(ClassLoader& cl) {
		cls = findClsFromCl(j::cfg["Minecraft"]["obf"], cl.obj);

		minecraft_fid = getField_id(cls
			, j::cfg["Minecraft"]["fields"]["minecraft"]["obf"]
			, j::cfg["Minecraft"]["fields"]["minecraft"]["sig"]);
		obj = jmy_getstaticobjfield(cls, minecraft_fid);

		std::cout << "minecraft_cls: " << cls << '\n';
		std::cout << "minecraft_fid: " << minecraft_fid << '\n';
		std::cout << "bao_field_obj: " << obj << '\n';
	}

	Jobj getTimer();
	Jobj getObjectMouse();
	Jobj getClientWorld();
	Jobj getPlayerMP();
};


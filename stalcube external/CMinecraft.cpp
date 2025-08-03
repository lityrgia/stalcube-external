#include "CMinecraft.h"
#include "cheat.h"
#include "options.h"

Jobj CMinecraft::getClientWorld() {
	auto clientWorld_fid = getField_id(cls, j::cfg["Minecraft"]["fields"]["clientWorld"]["obf"], j::cfg["Minecraft"]["fields"]["clientWorld"]["sig"]);
	std::cout << "clientWorld_fid: " << clientWorld_fid << '\n';

	auto clientWorld_obj = jmy_getobjfield(obj, clientWorld_fid);
	std::cout << "clientWorld_obj: " << clientWorld_obj << '\n';

	return clientWorld_obj;
}

Jobj CMinecraft::getPlayerMP() {
	auto playerMp_fid = getField_id(cls, j::cfg["Minecraft"]["fields"]["playerMP"]["obf"], j::cfg["Minecraft"]["fields"]["playerMP"]["sig"]);
	std::cout << "playerMp_fid: " << playerMp_fid << '\n';

	auto playerMp_obj = jmy_getobjfield(obj, playerMp_fid);
	std::cout << "playerMp_obj: " << playerMp_obj << '\n';

	return playerMp_obj;
}

Jobj CMinecraft::getTimer() {
	auto timer_fid = getField_id(cls, j::cfg["Minecraft"]["fields"]["timer"]["obf"], j::cfg["Minecraft"]["fields"]["timer"]["sig"]);
	std::cout << "timer_fid: " << timer_fid << '\n';

	auto timer_obj = jmy_getobjfield(obj, timer_fid);
	std::cout << "timer_obj: " << timer_obj << '\n';

	return timer_obj;
}

Jobj CMinecraft::getObjectMouse() {
	auto objMouse_fid = getField_id(cls, j::cfg["Minecraft"]["fields"]["objectMouse"]["obf"], j::cfg["Minecraft"]["fields"]["objectMouse"]["sig"]);
	auto objMouse_obj = jmy_getobjfield(obj, objMouse_fid);

	return objMouse_obj;
}
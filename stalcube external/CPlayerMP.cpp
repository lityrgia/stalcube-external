#include "CPlayerMP.h"
#include "cheat.h"

vec3 CPlayerMP::getPosition() {
	auto posX_fid = getField_id(cls, "bP", "D");
	auto posY_fid = getField_id(cls, "bR", "D");
	auto posZ_fid = getField_id(cls, "bS", "D");

	auto posX = (float)jmy_getdoublefield(obj, posX_fid);
	auto posY = (float)jmy_getdoublefield(obj, posY_fid);
	auto posZ = (float)jmy_getdoublefield(obj, posZ_fid);

	return vec3{ posX, posY, posZ };
}

vec2 CPlayerMP::getAngles() {
	auto yaw_fid = getField_id(cls, "bT", "F");
	auto pitch_fid = getField_id(cls, "bU", "F");

	auto yaw = (float)jmy_getfloatfield(obj, yaw_fid);
	auto pitch = (float)jmy_getfloatfield(obj, pitch_fid);

	return vec2{ yaw, pitch };
}
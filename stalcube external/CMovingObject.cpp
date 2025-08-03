#include "CMovingObject.h"

#include "cheat.h"
#include "options.h"

Jobj CMovingObject::getEntityHit()
{
	auto entityHit_fid = getField_id(cls, j::cfg["MovingObject"]["fields"]["entityHit"]["obf"], j::cfg["MovingObject"]["fields"]["entityHit"]["sig"]);
	auto entityHit = jmy_getobjfield(obj, entityHit_fid);

	//std::cout << "entityHit cls: " << cls << " obj: " << obj << " entityHit: " << entityHit << '\n';

	return entityHit;
}

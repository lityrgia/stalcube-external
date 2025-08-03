#include "CRenderManager.h"
#include "options.h"

void CRenderManager::getRenderPos(vec3& pos)
{
    auto posX_fid = getField_id(this->cls, j::cfg["RenderManager"]["fields"]["pos_x"]["obf"], "D");
    auto posY_fid = getField_id(this->cls, j::cfg["RenderManager"]["fields"]["pos_y"]["obf"], "D");
    auto posZ_fid = getField_id(this->cls, j::cfg["RenderManager"]["fields"]["pos_z"]["obf"], "D");

    auto posX = jmy_getstaticdoublefield(cls, posX_fid);
    auto posY = jmy_getstaticdoublefield(cls, posY_fid);
    auto posZ = jmy_getstaticdoublefield(cls, posZ_fid);

    pos.x = posX;
    pos.y = posY;
    pos.z = posZ;
}

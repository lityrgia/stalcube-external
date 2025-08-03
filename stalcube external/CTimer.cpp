#include "CTimer.h"

#include "cheat.h"
#include "options.h"

float CTimer::getRenderPartialTicks()
{
    auto partial_fid = getField_id(cls, j::cfg["Timer"]["fields"]["partialTicks"]["obf"], "F");
    auto partialTicks = jmy_getfloatfield(obj, partial_fid);

    return partialTicks;
}

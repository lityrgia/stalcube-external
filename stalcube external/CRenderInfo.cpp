#include "CRenderInfo.h"
#include <vector>

#include "options.h"
#include "mem.h"

void CRenderInfo::getMvpMatrix(GLfloat16& mvpMatrix) {

    // Получаем поле mvpMatrix (массив float[16])
    auto mvpMatrix_fid = getField_id(cls,
        j::cfg["SomeRenderElem"]["fields"]["mvpMatrix"]["obf"],
        j::cfg["SomeRenderElem"]["fields"]["mvpMatrix"]["sig"]);
    auto mvpMatrix_obj = jmy_getobjfield(obj, mvpMatrix_fid);

    int matrixSize = jmy_getarraylen(mvpMatrix_obj);

    jmy_getfloatarrayelems(mvpMatrix_obj, mvpMatrix.values, 0, 16);

    //printMatrix(modelMatrix);
}
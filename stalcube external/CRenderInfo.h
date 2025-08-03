#pragma once

#include "jimmy.h"
#include "types.h"
#include "ClassLoader.h"
#include "cheat.h"
#include "json.h"
#include "options.h"

//���� ��� ��� �� ����� ActiveRenderInfo, �� ��� ��� ��� �� ������� �� ARI � ���� �� ���������� ������������
class CRenderInfo
{
public:
	Jclass cls{ nullptr };
	Jobj obj{ nullptr };
	ClassLoader& cl;
public:
	CRenderInfo(ClassLoader& classLoader) : cl(classLoader) {
		cls = findClsFromCl(j::cfg["SomeRenderElem"]["obf"], cl.obj);

		auto someRend_fid = getField_id(cls,
			j::cfg["SomeRenderElem"]["fields"]["someRenderElem"]["obf"],
			j::cfg["SomeRenderElem"]["fields"]["someRenderElem"]["sig"]);
		obj = jmy_getstaticobjfield(cls, someRend_fid);

		std::cout << "render info cls: " << cls << '\n';
	}

	void getMvpMatrix(GLfloat16& mvpMatrix);
};


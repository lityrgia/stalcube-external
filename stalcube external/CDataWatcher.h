#pragma once

#include "jimmy.h"
#include "ClassLoader.h"

class CDataWatcher
{
public:
	Jclass cls{ nullptr };
	Jobj obj;
public:
	Jclass getCls() {
		if (cls == nullptr) {
			cls = jmy_getobjectclass(obj);
		}
		return cls;
	}

	float getHealth();
	template<typename T>
	T getValueFromKey(int key);
};


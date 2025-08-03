#pragma once

#include <iostream>
#include "jimmy.h"

class ClassLoader
{
public:
    Jclass cls;
    Jobj obj;
    Jid classLoader_fid;
public:
	ClassLoader() {}

    void init() {
        cls = jmy_findclass("net/minecraft/launchwrapper/Launch");
        std::cout << "launchcls: " << cls << '\n';

        classLoader_fid = jmy_getfieldid(cls, "classLoader", "Lnet/minecraft/launchwrapper/LaunchClassLoader;");
        std::cout << "classLoader_fid: " << classLoader_fid << '\n';

        obj = jmy_getstaticobjfield(cls, classLoader_fid);
        std::cout << "class obj: " << obj << '\n';
    }
};


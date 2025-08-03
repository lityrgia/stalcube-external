#pragma once
#include <unordered_map>
#include <mutex>

#include "memory.h"
#include "types.h"
#include "json.h"

class CEntity;
class Overlay;

extern std::vector<CEntity> entities;
extern CEntity closest_entity;
extern bool triggered_ent;
extern std::mutex gameDataMutex;

typedef unsigned short Jid;
typedef void* Jclass;
typedef void* Jobj;

struct FieldKey {
    Jclass cls;
    std::string name;
    std::string sig;

    bool operator==(const FieldKey& other) const {
        return cls == other.cls && name == other.name && sig == other.sig;
    }
};

struct FieldKeyHash {
    std::size_t operator()(const FieldKey& key) const {
        std::size_t h1 = std::hash<Jclass>()(key.cls);
        std::size_t h2 = std::hash<std::string>()(key.name);
        std::size_t h3 = std::hash<std::string>()(key.sig);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

struct ClassKey {
    Jclass classLoaderObj;
    std::string className;

    bool operator==(const ClassKey& other) const {
        return classLoaderObj == other.classLoaderObj && className == other.className;
    }
};

struct ClassKeyHash {
    std::size_t operator()(const ClassKey& key) const {
        std::size_t h1 = std::hash<Jclass>()(key.classLoaderObj);
        std::size_t h2 = std::hash<std::string>()(key.className);
        return h1 ^ (h2 << 1);
    }
};

Jid getField_id(Jclass cls, const std::string& name, const std::string& sig);
Jclass findClsFromCl(const std::string& className, Jclass classLoaderObj);
Jclass findClass(const std::string& className);
void printMatrix(const GLfloat16& mat);

class Cheat {
private:
	Memory mem;
	uintptr_t jvm_addy{0};
public:
	void processCheat();
	bool init();
};
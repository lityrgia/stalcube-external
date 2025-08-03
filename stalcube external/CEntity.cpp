#include "CEntity.h"
#include "cheat.h"
#include "util.h"
#include "options.h"

bool CEntity::isDead()
{
    auto isDead_fid = getField_id(cls, j::cfg["Entity"]["fields"]["isDead"]["obf"], "Z");
    auto isDead_bool = jmy_getboolfield(obj, isDead_fid);

    return isDead_bool;
}

BBox3D CEntity::getBoundingBox() {
	auto bb_fid = getField_id(cls, j::cfg["Entity"]["fields"]["BoundingBox"]["obf"], j::cfg["Entity"]["fields"]["BoundingBox"]["sig"]);
	auto bb_obj = jmy_getobjfield(obj, bb_fid);

    auto bb_cls = jmy_getobjectclass(bb_obj);

    double minX = jmy_getdoublefield(bb_obj, getField_id(bb_cls, j::cfg["BoundingBox"]["fields"]["minX"]["obf"], "D"));
    double minY = jmy_getdoublefield(bb_obj, getField_id(bb_cls, j::cfg["BoundingBox"]["fields"]["minY"]["obf"], "D"));
    double minZ = jmy_getdoublefield(bb_obj, getField_id(bb_cls, j::cfg["BoundingBox"]["fields"]["minZ"]["obf"], "D"));
    double maxX = jmy_getdoublefield(bb_obj, getField_id(bb_cls, j::cfg["BoundingBox"]["fields"]["maxX"]["obf"], "D"));
    double maxY = jmy_getdoublefield(bb_obj, getField_id(bb_cls, j::cfg["BoundingBox"]["fields"]["maxY"]["obf"], "D"));
    double maxZ = jmy_getdoublefield(bb_obj, getField_id(bb_cls, j::cfg["BoundingBox"]["fields"]["maxZ"]["obf"], "D"));

    return BBox3D{
        { (float)minX, (float)minY, (float)minZ },
        { (float)maxX, (float)maxY, (float)maxZ }
    };
}

vec3 CEntity::getLastTickPos() {

    double x = jmy_getdoublefield(obj, getField_id(cls, j::cfg["Entity"]["fields"]["lastTickPosX"]["obf"], "D"));
    double y = jmy_getdoublefield(obj, getField_id(cls, j::cfg["Entity"]["fields"]["lastTickPosY"]["obf"], "D"));
    double z = jmy_getdoublefield(obj, getField_id(cls, j::cfg["Entity"]["fields"]["lastTickPosZ"]["obf"], "D"));

    return vec3(x,y,z);
}

vec3 CEntity::getPos() {

    double x = jmy_getdoublefield(obj, getField_id(cls, j::cfg["Entity"]["fields"]["posX"]["obf"], "D"));
    double y = jmy_getdoublefield(obj, getField_id(cls, j::cfg["Entity"]["fields"]["posY"]["obf"], "D"));
    double z = jmy_getdoublefield(obj, getField_id(cls, j::cfg["Entity"]["fields"]["posZ"]["obf"], "D"));

    return vec3(x, y, z);
}

double CEntity::getAttribute(const std::u16string& attribute)
{
    auto tq_fid = getField_id(cls, j::cfg["Entity"]["fields"]["attribute"]["obf"], j::cfg["Entity"]["fields"]["attribute"]["sig"]);
    auto tq_obj = jmy_getobjfield(obj, tq_fid);
    if (!tq_obj) return false;

    auto attributes_fid = getField_id(jmy_getobjectclass(tq_obj), j::cfg["Attribute"]["fields"]["unk1"]["obf"], "Ljava/util/Map;");
    auto attributes_obj = jmy_getobjfield(tq_obj, attributes_fid);

    //std::cout << "attr_fid: " << attributes_fid << " attributes_obj: " << attributes_obj << " tq_obj " << tq_obj << '\n';

    auto hashMap_fid = getField_id(jmy_getobjectclass(attributes_obj), j::cfg["LinkedMap"]["fields"]["table"]["obf"], "Ljava/util/Map;");
    if (!hashMap_fid) return false;
    //std::cout << "map_fid: " << hashMap_fid << '\n';

    auto hashMap_obj = jmy_getobjfield(attributes_obj, hashMap_fid);
    if (!hashMap_obj) return false;
    //std::cout << "map_obj: " << hashMap_obj << '\n';

    auto hashMap_cls = jmy_getobjectclass(hashMap_obj);
    if (!hashMap_cls) return false;
    //std::cout << "hashMap_cls: " << hashMap_cls << '\n';

    auto hashMapEntry_fid = getField_id(hashMap_cls, "head", "Ljava/util/LinkedHashMap$Entry;");
    if (!hashMapEntry_fid) return false;
    //std::cout << "hashMapEntry_fid: " << hashMapEntry_fid << '\n';

    auto hashMapEntry_obj = jmy_getobjfield(hashMap_obj, hashMapEntry_fid);
    if (!hashMapEntry_obj) return false;
    //std::cout << "hashMapEntry_obj: " << hashMapEntry_obj << '\n';

    do
    {
        auto key_str_fid = getField_id(jmy_getobjectclass(hashMapEntry_obj), "key", "Ljava/lang/Object;");
        auto key_str_obj = jmy_getobjfield(hashMapEntry_obj, key_str_fid);

        auto str_value_fid = getField_id(jmy_getobjectclass(key_str_obj), "value", "[C");
        auto str_value_obj = jmy_getobjfield(key_str_obj, str_value_fid);
        auto str_value_length = jmy_getarraylen(str_value_obj);

        //это просто ужас, но мне так впадлу делать отдельные классы

        std::u16string attribute_string;
        attribute_string.resize(str_value_length);
        jmy_getchararrayelems(str_value_obj, (Jchar*)attribute_string.data(), 0, str_value_length);
        //std::cout << u16_to_utf8(attribute_string) << '\n';

        if (attribute_string == attribute) {
            auto attribInstance_fid = getField_id(jmy_getobjectclass(hashMapEntry_obj), "value", "Ljava/lang/Object;");
            auto attribInstance_obj = jmy_getobjfield(hashMapEntry_obj, attribInstance_fid);

            auto attribute_fid = getField_id(jmy_getobjectclass(attribInstance_obj), j::cfg["LinkedMap"]["fields"]["doubleValue"]["obf"], "D");
            auto final_attrib_obj = jmy_getdoublefield(attribInstance_obj, attribute_fid);
            return final_attrib_obj;
        }

        auto after_fid = getField_id(jmy_getobjectclass(hashMapEntry_obj), "after", "Ljava/util/LinkedHashMap$Entry;");
        hashMapEntry_obj = jmy_getobjfield(hashMapEntry_obj, after_fid);

    } while (hashMapEntry_obj != nullptr);

    return 0;
}

Jobj CEntity::getDataWatcher()
{
    auto dataWatcher_fid = getField_id(cls, j::cfg["Entity"]["fields"]["dataWatcher"]["obf"], j::cfg["Entity"]["fields"]["dataWatcher"]["sig"]);
    auto dataWatcher_obj = jmy_getobjfield(obj, dataWatcher_fid);

    //std::cout << "dataw_fid: " << dataWatcher_fid << " dataw_obj: " << dataWatcher_obj << " obj: " << obj << "\n";

    return dataWatcher_obj;
}

std::u16string CEntity::getName() {
    auto nameClear_fid = getField_id(cls, j::cfg["EntityPlayer"]["fields"]["nameClear"]["obf"], "Ljava/lang/String;");
    auto nameClear_obj = jmy_getobjfield(obj, nameClear_fid);

    //std::cout << "name_fid: " << nameClear_fid << " nameCl_obj: " << nameClear_obj << '\n';

    if (!nameClear_fid) {
        return u"Unknown";
    }

    auto value_fid = getField_id(jmy_getobjectclass(nameClear_obj), "value", "[C");
    auto value_obj = jmy_getobjfield(nameClear_obj, value_fid);
    auto value_length = jmy_getarraylen(value_obj);

    //std::cout << "value_fid: " << value_fid << " value_obj: " << value_obj << '\n';

    std::u16string name;
    name.resize(value_length);
    jmy_getchararrayelems(value_obj, (Jchar*)name.data(), 0, value_length);

    return name;
}
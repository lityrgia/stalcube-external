#include "CDataWatcher.h"

#include "cheat.h"
#include "options.h"

template<typename T>
T CDataWatcher::getValueFromKey(int key) {

    auto map_fid = getField_id(getCls(), j::cfg["DataWatcher"]["fields"]["map"]["obf"], j::cfg["DataWatcher"]["fields"]["map"]["sig"]);
    if (!map_fid) return false;
    //std::cout << "map_fid: " << map_fid << '\n';

    auto map_obj = jmy_getobjfield(obj, map_fid);
    if (!map_obj) return false;
    //std::cout << "map_obj: " << map_obj << '\n';

    auto map_cls = jmy_getobjectclass(map_obj);
    if (!map_cls) return false;
    //std::cout << "map_cls: " << map_cls << '\n';

    auto table_fid = getField_id(map_cls, "table", "[Ljava/util/HashMap$Node;");
    if (!table_fid) return false;
    //std::cout << "table_fid: " << table_fid << '\n';

    auto table_obj = jmy_getobjfield(map_obj, table_fid);
    if (!table_obj) return false;
    //std::cout << "table_obj: " << table_obj << '\n';

    int length_table = jmy_getarraylen(table_obj);
    //std::cout << "length_table: " << length_table << '\n';

    std::vector<Jobj> nodes(length_table);

    jmy_getobjarrayelems(table_obj, nodes.data(), 0, length_table);

    for (const auto& node : nodes) {

        if (node != nullptr) {
            auto node_cls = jmy_getobjectclass(node);
            auto key_fid = getField_id(node_cls, "key", "Ljava/lang/Object;");
            auto key_obj = jmy_getobjfield(node, key_fid);
            auto key_value_fid = getField_id(jmy_getobjectclass(key_obj), "value", "I");
            auto key_value_int = jmy_getintfield(key_obj, key_value_fid);

            if (key_value_int == key) {
                auto data_fid = getField_id(node_cls, "value", "Ljava/lang/Object;");
                auto data_obj = jmy_getobjfield(node, data_fid);
                //std::cout << "key_value_int: " << key_value_int << " dataObj: " << data_obj << '\n';

                if constexpr (std::is_same_v<T, float>) {
                    auto float_fid = getField_id(jmy_getobjectclass(data_obj), "iny", "Ljava/lang/Object;");
                    //std::cout << "float_fid: " << float_fid << '\n';
                    auto float_obj = jmy_getobjfield(data_obj, float_fid);
                    auto float_value_fid = getField_id(jmy_getobjectclass(float_obj), "value", "F");
                    auto float_value_val = jmy_getfloatfield(float_obj, float_value_fid);
                    return float_value_val;
                }
                /*if constexpr (std::is_same_v(T, const char*)) {
                    auto string_fid = getField_id(jmy_getobjclass(data_obj), "c", "Ljava/lang/Object;");
                    auto string_obj = jmy_getobjfield(data_obj, float_fid);
                    auto string_value_fid = getField_id(jmy_getobjectclass(string_obj), "value", "[C");
                    
                    return float_val;
                }*/
            }
        }
    }

    return 0;
}

float CDataWatcher::getHealth()
{
    return getValueFromKey<float>(6);
}
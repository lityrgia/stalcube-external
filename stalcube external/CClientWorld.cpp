#include "CClientWorld.h"

#include "options.h"
#include <iostream>
#include "cheat.h"

bool CClientWorld::getListEntities(std::vector<CEntity>& entities) {
    //it would be better to make CMap class but i don't want

	 auto entityList_fid = getField_id(cls, j::cfg["ClientWorld"]["fields"]["entityList"]["obf"], "Ljava/util/Set;");
     if (!entityList_fid) return false;
	 //std::cout << "entityList_fid: " << entityList_fid << '\n';

	 auto entityList_obj = jmy_getobjfield(obj, entityList_fid);
     if (!entityList_obj) return false;
	 //std::cout << "entityList_obj: " << entityList_obj << '\n';

	 auto entityList_cls = jmy_getobjectclass(entityList_obj);
     if (!entityList_cls) return false;
	 //std::cout << "entityList_cls: " << entityList_cls << '\n';

	 auto map_fid = getField_id(entityList_cls, "map", "Ljava/util/HashMap;");
     if (!map_fid) return false;
	 //std::cout << "map_fid: " << map_fid << '\n';

	 auto map_obj = jmy_getobjfield(entityList_obj, map_fid);
     if (!map_obj) return false;
	 //std::cout << "map_obj: " << map_obj << '\n';

     auto entityBase_cls = findClsFromCl(j::cfg["Entity"]["obf"], cl.obj);

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

     entities.clear();

     for (const auto& node : nodes) {

         if (node != nullptr) {
             auto node_cls = jmy_getobjectclass(node);
             auto key_fid = getField_id(node_cls, "key", "Ljava/lang/Object;");
             auto key_obj = jmy_getobjfield(node, key_fid);

             entities.push_back(CEntity(key_obj));
         }
     }

     return true;
}
#include <thread>
#include <chrono>
#include <algorithm>
#include <stddef.h>
#include <codecvt>
#include <locale>

#include "math.h"
#include "jimmy.h"
#include "process.h"
#include "cheat.h"
#include "win.c"
#include "types.h"
#include "util.h"
#include "options.h"

#include "CEntity.h"
#include "ClassLoader.h"
#include "CMinecraft.h"
#include "CClientWorld.h"
#include "CTimer.h"
#include "CPlayerMP.h"
#include "CRenderManager.h"
#include "CMovingObject.h"
#include "CRenderInfo.h"

bool Cheat::init() {
    auto pid = Process::getPidByName(L"java.exe");

    if (pid == 0) {
        printf("[cheat]: Failed to find process ---\n");
        return false;
    }

    std::cout << "[cheat]: Process found with id: " << pid << '\n';

    HWND gameHwnd = Process::FindWindowByPID(pid);

    if (!gameHwnd) {
        printf("[cheat]: Failed to find window ---\n");
        return false;
    }

    RECT clientRect;
    POINT topLeft = { 0, 0 };

    if (GetClientRect(gameHwnd, &clientRect) && ClientToScreen(gameHwnd, &topLeft)) {
        int width = clientRect.right - clientRect.left;
        int height = clientRect.bottom - clientRect.top;

        options::monitorX = width;
        options::monitorY = height;
        options::mon_posX= topLeft.x;
        options::mon_posY = topLeft.y;
    }
    else {
        options::monitorX = 1920;
        options::monitorY = 1080;
    }

    if (!mem.init(pid)) {
        printf("[cheat]: Failed to OpenProcess Error: %lu\n", GetLastError());
        return false;
    }

    jvm_addy = Process::getModuleAddress(pid, L"jvm.dll");

    jmy_init(mem.processHandle, (const void*)jvm_addy);

    return true;
}

std::unordered_map<FieldKey, Jid, FieldKeyHash> field_id_cache;
std::unordered_map<ClassKey, Jclass, ClassKeyHash> class_cache;

Jid getField_id(Jclass cls, const std::string& name, const std::string& sig) {
    FieldKey key{ cls, name, sig };

    auto it = field_id_cache.find(key);
    if (it != field_id_cache.end())
        return it->second;

    Jid fid = jmy_getfieldid(cls, name.c_str(), sig.c_str());
    field_id_cache[key] = fid;
    return fid;
}

Jclass findClass(const std::string& className) {
    return jmy_findclass(className.c_str());
}

Jclass findClsFromCl(const std::string& className, Jclass classLoaderObj) {
    ClassKey key{ classLoaderObj, className };

    auto it = class_cache.find(key);
    if (it != class_cache.end())
        return it->second;

    Jclass cls = jmy_findclassfromcl(className.c_str(), classLoaderObj);
    class_cache[key] = cls;
    return cls;
}

const std::u16string MAX_HEALTH = u"max health";

std::vector<CEntity> entities;
CEntity closest_entity;
bool triggered_ent;
std::mutex gameDataMutex;

void Cheat::processCheat() {
    try {
        ClassLoader classLoader;
        classLoader.init();

        CMinecraft cminecraft(classLoader);

        if (cminecraft.obj == nullptr) {
            std::cout << "[cheat] Error: Looks like wrong mappings\n";
            system("pause");
            return;
        }

        std::vector<CEntity> entities_copy;
        CEntity closest_entity_copy{};
        bool triggered_copy{};

        vec3 renderPos{ 0 };
        GLfloat16 mvpMatrix{ 0 };

        while (true) {
            auto clientWorld_obj = cminecraft.getClientWorld();
            if (clientWorld_obj == nullptr) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                continue;
            }
            if (menu::consoleAllocated) {
                HWND consoleWindow = GetConsoleWindow();
                if (consoleWindow != NULL) {
                    //ShowWindow(consoleWindow, SW_HIDE);
                }
                menu::consoleAllocated = false;
            }

            CClientWorld clientWorld(clientWorld_obj, classLoader);
            CPlayerMP localPlayer(cminecraft.getPlayerMP());
            CTimer timer(cminecraft.getTimer());
            CRenderInfo ari(classLoader);
            CRenderManager rm(classLoader);

            if (localPlayer.obj == nullptr
                || timer.obj == nullptr
                || ari.cls == nullptr
                || rm.cls == nullptr) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                continue;
            }

            auto entityTroop_cls = findClsFromCl(j::cfg["EntityTroop"]["obf"], classLoader.obj);
            auto entityItem_cls = findClsFromCl(j::cfg["EntityItem"]["obf"], classLoader.obj);
            //auto entityMonster_cls = findClsFromCl(j::cfg["EntityMob"]["obf"], classLoader.obj);
            auto entityPlayer_cls = findClsFromCl(j::cfg["EntityPlayer"]["obf"], classLoader.obj);
            auto entityGameItems_cls = findClsFromCl(j::cfg["EntityGameItems"]["obf"], classLoader.obj);
            auto entityNPC_cls = findClsFromCl(j::cfg["EntityNPC"]["obf"], classLoader.obj);

            while (clientWorld.getListEntities(entities_copy)) {
                ari.getMvpMatrix(mvpMatrix);
                rm.getRenderPos(renderPos);

                float renderPartTicks = timer.getRenderPartialTicks();

                for (auto& entity : entities_copy) {
                    if (localPlayer.obj == entity.obj) continue;

                    bool isPlayer = false;
                    entity.dead = entity.isDead();

                    if (jmy_isinstanceof(entity.obj, entityPlayer_cls)) {
                        if (entity.dead) continue;
                        if (options::bNamesEnabled) entity.name = entity.getName();
                        entity.type = CEntity::Type::Player;
                        isPlayer = true;
                    }
                    else if (jmy_isinstanceof(entity.obj, entityTroop_cls)) {
                        entity.name = u"Troop";
                        entity.isTeam = 1;
                        entity.type = CEntity::Type::Troop;
                    }
                    else if (jmy_isinstanceof(entity.obj, entityNPC_cls)) {
                        entity.name = u"NPC";
                        entity.isTeam = 1;
                        entity.type = CEntity::Type::NPC;
                    }
                    else if (jmy_isinstanceof(entity.obj, entityGameItems_cls)) {
                        entity.name = u"Game_item";
                        entity.isTeam = 1;
                        entity.type = CEntity::Type::GAME_ITEMS;
                    }
                    else if (jmy_isinstanceof(entity.obj, entityItem_cls)) {
                        entity.name = u"Item";
                        entity.type = CEntity::Type::Item;
                    }
                    else {
                        entity.name = u"unk";
                        entity.type = CEntity::Type::Unknown;
                        continue;
                    }

                    entity.bbox_3d = entity.getBoundingBox();
                    entity.lastTickPos = entity.getLastTickPos();
                    entity.pos = entity.getPos();
                    entity.dataWatcher.obj = entity.getDataWatcher();
                    
                    if (options::bHealthEnabled) {
                        entity.maxHealth = entity.getAttribute(MAX_HEALTH);
                        entity.health = entity.dataWatcher.getHealth();
                    }

                    //https://www.unknowncheats.me/forum/minecraft/412743-2d-3d-esp-via-jni-opengl-overlay.html
                    //кто-то спросит зачем добавлять вместо entity.pos.x entity.lastTickPos.x если это игрок. Честно я сам не знаю, почему-то у игрока pos.x равен pos.y, btw так тоже работает

                    if (!isPlayer) {
                        entity.bbox_3d.min.x += (-entity.pos.x) + (entity.lastTickPos.x + (entity.pos.x - entity.lastTickPos.x) * renderPartTicks) + (-renderPos.x);
                        entity.bbox_3d.min.y += (-entity.pos.y) + (entity.lastTickPos.y + (entity.pos.y - entity.lastTickPos.y) * renderPartTicks) + (-renderPos.y);
                        entity.bbox_3d.min.z += (-entity.pos.z) + (entity.lastTickPos.z + (entity.pos.z - entity.lastTickPos.z) * renderPartTicks) + (-renderPos.z);
                        entity.bbox_3d.max.x += (-entity.pos.x) + (entity.lastTickPos.x + (entity.pos.x - entity.lastTickPos.x) * renderPartTicks) + (-renderPos.x);
                        entity.bbox_3d.max.y += (-entity.pos.y) + (entity.lastTickPos.y + (entity.pos.y - entity.lastTickPos.y) * renderPartTicks) + (-renderPos.y);
                        entity.bbox_3d.max.z += (-entity.pos.z) + (entity.lastTickPos.z + (entity.pos.z - entity.lastTickPos.z) * renderPartTicks) + (-renderPos.z);
                    }
                    else {
                        entity.bbox_3d.min.x += (-entity.lastTickPos.x) + (entity.lastTickPos.x + (entity.lastTickPos.x - entity.lastTickPos.x) * renderPartTicks) + (-renderPos.x);
                        entity.bbox_3d.min.y += (-entity.pos.y) + (entity.lastTickPos.y + (entity.pos.y - entity.lastTickPos.y) * renderPartTicks) + (-renderPos.y);
                        entity.bbox_3d.min.z += (-entity.pos.z) + (entity.lastTickPos.z + (entity.pos.z - entity.lastTickPos.z) * renderPartTicks) + (-renderPos.z);
                        entity.bbox_3d.max.x += (-entity.lastTickPos.x) + (entity.lastTickPos.x + (entity.lastTickPos.x - entity.lastTickPos.x) * renderPartTicks) + (-renderPos.x);
                        entity.bbox_3d.max.y += (-entity.pos.y) + (entity.lastTickPos.y + (entity.pos.y - entity.lastTickPos.y) * renderPartTicks) + (-renderPos.y);
                        entity.bbox_3d.max.z += (-entity.pos.z) + (entity.lastTickPos.z + (entity.pos.z - entity.lastTickPos.z) * renderPartTicks) + (-renderPos.z);
                    }

                    float boxVertices[8][3]
                    {
                        {entity.bbox_3d.min.x - 0.1f, entity.bbox_3d.min.y - 0.1f, entity.bbox_3d.min.z - 0.1f},
                        {entity.bbox_3d.min.x - 0.1f, entity.bbox_3d.max.y + 0.1f, entity.bbox_3d.min.z - 0.1f},
                        {entity.bbox_3d.max.x + 0.1f, entity.bbox_3d.max.y + 0.1f, entity.bbox_3d.min.z - 0.1f},
                        {entity.bbox_3d.max.x + 0.1f, entity.bbox_3d.min.y - 0.1f, entity.bbox_3d.min.z - 0.1f},
                        {entity.bbox_3d.max.x + 0.1f, entity.bbox_3d.max.y + 0.1f, entity.bbox_3d.max.z + 0.1f},
                        {entity.bbox_3d.min.x - 0.1f, entity.bbox_3d.max.y + 0.1f, entity.bbox_3d.max.z + 0.1f},
                        {entity.bbox_3d.min.x - 0.1f, entity.bbox_3d.min.y - 0.1f, entity.bbox_3d.max.z + 0.1f},
                        {entity.bbox_3d.max.x + 0.1f, entity.bbox_3d.min.y - 0.1f, entity.bbox_3d.max.z + 0.1f},
                    };

                    for (int i = 0; i < 8; i++)
                    {
                        vec2 screenPos;
                        if (worldToScreen(vec3(boxVertices[i][0], boxVertices[i][1], boxVertices[i][2]), screenPos, mvpMatrix.values))
                        {
                            entity.w2sbb.x = (std::min)(screenPos.x, entity.w2sbb.x);
                            entity.w2sbb.y = (std::min)(screenPos.y, entity.w2sbb.y);
                            entity.w2sbb.z = (std::max)(screenPos.x, entity.w2sbb.z);
                            entity.w2sbb.w = (std::max)(screenPos.y, entity.w2sbb.w);
                        }
                    }

                    if (!triggered_copy) {
                        triggered_copy = (options::monitorX / 2 >= entity.w2sbb.x && options::monitorX / 2 <= entity.w2sbb.y &&
                            options::monitorY / 2 >= entity.w2sbb.z && options::monitorY / 2 <= entity.w2sbb.w);
                    }

                   //std::cout << "x: " << entity.w2sbb.x << " y: " << entity.w2sbb.y << '\n';
                }

                findClosestToCenter(entities_copy, closest_entity_copy);

                {
                    std::lock_guard<std::mutex> lock(gameDataMutex);
                    entities = std::move(entities_copy);
                    closest_entity = std::move(closest_entity_copy);
                    triggered_ent = triggered_copy;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(3));
            }
        }
    }
    catch (const nlohmann::json::exception& e) {
        std::cerr << "JSON exception: " << e.what() << '\n';
    }
    catch (const std::exception& e) {
        std::cerr << "Standard exception: " << e.what() << '\n';
    }
    catch (...) {
        std::cerr << "Unknown exception caught\n";
    }
}
#include "systems/SceneManager.h"
#include "core/Mesh.h"
#include "core/Model.h"
#include "core/TextureCache.h"
#include "core/InputManager.h"
#include <iostream>
#include <cmath>
#include <random>

using glm::vec3;

SceneManager::SceneManager() {
    m_player = std::make_unique<Player>();
}

void SceneManager::loadScene(const std::string& name) {
    m_currentScene = name;
    m_objects.clear();
    if (name == "night_shift")
        buildGuaritaScene();
    std::cout << "[Scene] Cena carregada: " << name << "\n";
}

void SceneManager::update(float dt) {
    m_gameTime += dt;

    auto& input = InputManager::get();
    bool moving = input.isKeyPressed(GLFW_KEY_W) ||
                  input.isKeyPressed(GLFW_KEY_S) ||
                  input.isKeyPressed(GLFW_KEY_A) ||
                  input.isKeyPressed(GLFW_KEY_D);

    m_player->update(dt);
    m_creature.setFlashlightInfo(
        m_player->camera.getFlashlightPos(),
        m_player->camera.getFlashlightDir(),
        m_player->camera.flashlightOn
    );
    m_creature.setGameTime(m_gameTime);
    m_creature.setPlayerMoving(moving);
    m_creature.setPlayerPos(m_player->camera.Position);
    m_creature.update(dt);
    if (m_creature.isGameOver())
        m_gameOver = true;

    for (auto& obj : m_objects)
        if (obj->active) obj->update(dt);
}

void SceneManager::addObject(std::unique_ptr<GameObject> obj) {
    m_objects.push_back(std::move(obj));
}

GameObject& SceneManager::make(std::unique_ptr<GameObject> obj) {
    auto& ref = *obj;
    m_objects.push_back(std::move(obj));
    return ref;
}

void SceneManager::buildGuaritaScene() {
    const float ROOM_HALF = 3.0f;
    const float WALL_THICK = 0.1f;
    const float CEILING_Y = 2.5f;
    const float WIN_H = 1.2f;
    const float WIN_Y0 = 0.9f;
    const float WIN_Y1 = WIN_Y0 + WIN_H;
    const float PILLAR_W = 0.35f;
    const float DOOR_W = 0.8f;
    const float DOOR_H = 2.0f;

    // ══════════════════════════════════════════════
    //  PISO E TETO
    // ══════════════════════════════════════════════

    {
        auto floor = std::make_unique<GameObject>("Floor");
        floor->position    = vec3(0.0f, 0.0f, 0.0f);
        floor->scale       = vec3(ROOM_HALF * 2.0f, 0.1f, ROOM_HALF * 2.0f);
        floor->textureName = "piso.jpg";
        addObject(std::move(floor));
    }
    {
        auto ceiling = std::make_unique<GameObject>("Ceiling");
        ceiling->position    = vec3(0.0f, CEILING_Y, 0.0f);
        ceiling->scale       = vec3(ROOM_HALF * 2.0f, 0.1f, ROOM_HALF * 2.0f);
        ceiling->textureName = "teto.jpg";
        addObject(std::move(ceiling));
    }

    // ══════════════════════════════════════════════
    //  PAREDES
    // ══════════════════════════════════════════════

    struct WallDef { std::string name; float posX, posZ; int axis; bool hasDoor; };
    WallDef walls[] = {
        {"N",  0.0f, -ROOM_HALF, 1, false},
        {"S",  0.0f,  ROOM_HALF, 1, true},
        {"L",  ROOM_HALF,  0.0f, 0, false},
        {"O", -ROOM_HALF,  0.0f, 0, false},
    };

    for (auto& w : walls) {
        bool axisX = (w.axis == 1);
        float pH = WIN_Y1 - WIN_Y0;
        float topH = CEILING_Y - WIN_Y1;

        if (w.hasDoor) {
            float doorRight = -ROOM_HALF + DOOR_W;
            float winLeft = doorRight + 0.2f;
            float winL = winLeft + PILLAR_W * 0.5f;
            float winR = ROOM_HALF - PILLAR_W * 0.5f;

            {
                auto base = std::make_unique<GameObject>("WallBase_S");
                base->position = vec3((doorRight + ROOM_HALF) * 0.5f, WIN_Y0 * 0.5f, w.posZ);
                base->scale    = vec3(ROOM_HALF - doorRight, WIN_Y0, WALL_THICK);
                base->textureName = "parede.jpg";
                addObject(std::move(base));
            }
            {
                auto top = std::make_unique<GameObject>("WallTop_S");
                top->position = vec3(0.0f, WIN_Y1 + topH * 0.5f, w.posZ);
                top->scale    = vec3(ROOM_HALF * 2.0f, topH, WALL_THICK);
                top->textureName = "parede.jpg";
                addObject(std::move(top));
            }
            {
                auto above = std::make_unique<GameObject>("WallAboveDoor_S");
                float aboveH = CEILING_Y - DOOR_H;
                above->position = vec3(-ROOM_HALF + DOOR_W * 0.5f, DOOR_H + aboveH * 0.5f, w.posZ);
                above->scale    = vec3(DOOR_W, aboveH, WALL_THICK);
                above->textureName = "parede.jpg";
                addObject(std::move(above));
            }
            for (int side = -1; side <= 1; side += 2) {
                auto pillar = std::make_unique<GameObject>("WallPillarS" + std::string(side > 0 ? "R" : "L"));
                float cx = (side > 0) ? winR : winL;
                pillar->position = vec3(cx, WIN_Y0 + pH * 0.5f, w.posZ);
                pillar->scale    = vec3(PILLAR_W, pH, WALL_THICK);
                pillar->textureName = "parede.jpg";
                addObject(std::move(pillar));
            }
            {
                auto mid = std::make_unique<GameObject>("WallPillarS_Mid");
                mid->position = vec3((doorRight + winLeft) * 0.5f, WIN_Y0 + pH * 0.5f, w.posZ);
                mid->scale    = vec3(winLeft - doorRight, pH, WALL_THICK);
                mid->textureName = "parede.jpg";
                addObject(std::move(mid));
            }
        } else {
            float pOff = ROOM_HALF - PILLAR_W * 0.5f;

            {
                auto base = std::make_unique<GameObject>("WallBase_" + w.name);
                if (axisX) {
                    base->position = vec3(0.0f, WIN_Y0 * 0.5f, w.posZ);
                    base->scale    = vec3(ROOM_HALF * 2.0f, WIN_Y0, WALL_THICK);
                } else {
                    base->position = vec3(w.posX, WIN_Y0 * 0.5f, 0.0f);
                    base->scale    = vec3(WALL_THICK, WIN_Y0, ROOM_HALF * 2.0f);
                }
                base->textureName = "parede.jpg";
                addObject(std::move(base));
            }
            {
                auto top = std::make_unique<GameObject>("WallTop_" + w.name);
                if (axisX) {
                    top->position = vec3(0.0f, WIN_Y1 + topH * 0.5f, w.posZ);
                    top->scale    = vec3(ROOM_HALF * 2.0f, topH, WALL_THICK);
                } else {
                    top->position = vec3(w.posX, WIN_Y1 + topH * 0.5f, 0.0f);
                    top->scale    = vec3(WALL_THICK, topH, ROOM_HALF * 2.0f);
                }
                top->textureName = "parede.jpg";
                addObject(std::move(top));
            }
            for (int side = -1; side <= 1; side += 2) {
                auto pillar = std::make_unique<GameObject>("WallPillar" + w.name + (side > 0 ? "R" : "L"));
                if (axisX) {
                    pillar->position = vec3((float)side * pOff, WIN_Y0 + pH * 0.5f, w.posZ);
                    pillar->scale    = vec3(PILLAR_W, pH, WALL_THICK);
                } else {
                    pillar->position = vec3(w.posX, WIN_Y0 + pH * 0.5f, (float)side * pOff);
                    pillar->scale    = vec3(WALL_THICK, pH, PILLAR_W);
                }
                pillar->textureName = "parede.jpg";
                addObject(std::move(pillar));
            }
        }
    }

    // ══════════════════════════════════════════════
    //  PORTA (.obj)
    // ══════════════════════════════════════════════

    const float DOOR_X = -ROOM_HALF + DOOR_W * 0.5f;
    const float DOOR_Z = ROOM_HALF - 0.005f;

    {
        auto& door = make(std::make_unique<GameObject>("Door"));
        // ← Y corrigido: pivot do modelo provavelmente no centro, então sobe pra metade da altura da porta
        door.position   = vec3(DOOR_X, DOOR_H * 0.5f, DOOR_Z);
        door.rotation.y = 180.0f;
        door.scale      = vec3(2.0f);
        door.model      = std::make_unique<Model>("assets/models/Doorway Front/doorwayFront.obj");
    }

    // ══════════════════════════════════════════════
    //  MESA EM L (madeira)
    // ══════════════════════════════════════════════

    const float DESK_H = 0.75f;
    const float DESK_T = 0.04f;

    {
        auto& top = make(std::make_unique<GameObject>("DeskTopA"));
        top.position    = vec3(1.6f, DESK_H, -2.5f);
        top.mesh        = std::make_unique<Mesh>(Mesh::createBox(2.6f, DESK_T, 0.8f));
        top.textureName = "mesa.jpg";
    }
    float legR = 0.025f;
    float legH = DESK_H - DESK_T;
    float ax0 = 1.6f - 1.25f, ax1 = 1.6f + 1.25f;
    float az0 = -2.5f - 0.35f, az1 = -2.5f + 0.35f;
    for (float lx : {ax0, ax1}) {
        for (float lz : {az0, az1}) {
            auto& leg = make(std::make_unique<GameObject>("DeskLegA"));
            leg.position    = vec3(lx, legH * 0.5f, lz);
            leg.mesh        = std::make_unique<Mesh>(Mesh::createCylinder(legR, legH, 8));
            leg.textureName = "mesa.jpg";
        }
    }

    {
        auto& top = make(std::make_unique<GameObject>("DeskTopB"));
        top.position    = vec3(2.5f, DESK_H, -0.55f);
        top.mesh        = std::make_unique<Mesh>(Mesh::createBox(0.8f, DESK_T, 3.1f));
        top.textureName = "mesa.jpg";
    }
    float bx0 = 2.5f - 0.35f, bx1 = 2.5f + 0.35f;
    float bz0 = -0.55f - 1.5f, bz1 = -0.55f + 1.5f;
    for (float lx : {bx0, bx1}) {
        for (float lz : {bz0, bz1}) {
            auto& leg = make(std::make_unique<GameObject>("DeskLegB"));
            leg.position    = vec3(lx, legH * 0.5f, lz);
            leg.mesh        = std::make_unique<Mesh>(Mesh::createCylinder(legR, legH, 8));
            leg.textureName = "mesa.jpg";
        }
    }

    // ══════════════════════════════════════════════
    //  COMPUTADOR (.obj)
    // ══════════════════════════════════════════════

    {
        auto& comp = make(std::make_unique<GameObject>("Computer"));
        comp.position   = vec3(2.46f, 1.03f, -2.23f);
        comp.rotation.y = -45.0f;
        comp.scale      = vec3(0.1f);
        comp.textureName = "assets/models/Desktop computer/Computer_BaseColor.png";
        comp.model      = std::make_unique<Model>("assets/models/Desktop computer/Computer.obj");
    }

    // ══════════════════════════════════════════════
    //  CADEIRA (.obj)
    // ══════════════════════════════════════════════

    const float CHAIR_X = 0.5f;
    const float CHAIR_Z = -0.3f;

    {
        auto& chair = make(std::make_unique<GameObject>("Chair"));
        chair.position   = vec3(CHAIR_X, 0.565f, CHAIR_Z);
        chair.scale      = vec3(0.025f);
        chair.model      = std::make_unique<Model>("assets/models/Office Chair/48.obj");
    }

    // ══════════════════════════════════════════════
    //  ÁREA EXTERNA (floresta noturna macabra)
    // ══════════════════════════════════════════════
    TextureCache::makeColor("trunk_brown",   12, 7,  3);
    TextureCache::makeColor("foliage_dark",  1, 5,  1);
    TextureCache::makeColor("moon_glow",    100, 90, 80);

    // Chão externo — Y=-1 para evitar z-fighting com piso da sala
    {
        auto& ground = make(std::make_unique<GameObject>("GroundOutside"));
        ground.position    = vec3(0.0f, -1.0f, 0.0f);
        ground.scale       = vec3(80.0f, 0.1f, 80.0f);
        ground.textureName = "ground.jpg";
    }

    // Gerador determinístico de posições
    unsigned int rngSeed = 0;
    auto rng = [&]() -> float {
        rngSeed = rngSeed * 1103515245u + 12345u;
        return (float)(rngSeed >> 16) / 32768.0f;
    };

    // ── Árvores procedurais (tronco + copa) ─────────────────────────
    // Distribuídas em anéis com espaçamento angular uniforme + jitter
    struct TreeDef { float x, z; float h; float rot; };
    std::vector<TreeDef> procTrees;
    procTrees.reserve(70);

    float ringData[][3] = {
        { 6.0f,  8.0f, 10 },
        { 9.0f,  11.0f, 12 },
        { 13.0f, 15.0f, 12 },
        { 17.0f, 19.0f, 10 },
        { 22.0f, 24.0f, 8 },
        { 28.0f, 31.0f, 6 },
        { 35.0f, 38.0f, 5 },
    };
    unsigned int seeds[] = { 137, 256, 789, 1024, 5555, 9999, 7777 };

    for (int ring = 0; ring < 7; ring++) {
        rngSeed = seeds[ring];
        float minD = ringData[ring][0];
        float maxD = ringData[ring][1];
        int cnt    = (int)ringData[ring][2];
        float step = 6.2831853f / cnt;
        float off  = rng() * step;
        for (int i = 0; i < cnt; i++) {
            float ang  = off + step * i + (rng() - 0.5f) * step * 0.35f;
            float dist = minD + rng() * (maxD - minD);
            procTrees.push_back({
                cosf(ang) * dist, sinf(ang) * dist,
                15.0f + rng() * 15.0f,
                rng() * 6.2831853f
            });
        }
    }

    for (auto& t : procTrees) {
        float trunkH = t.h * 0.6f;
        float trunkR = t.h * 0.035f;
        float crownR = t.h * 0.25f;
        {
            auto& trunk = make(std::make_unique<GameObject>("Trunk"));
            trunk.position   = vec3(t.x, 0, t.z);
            trunk.rotation.y = t.rot;
            trunk.mesh = std::make_unique<Mesh>(Mesh::createTaperedCylinder(trunkR, trunkR * 0.25f, trunkH));
            trunk.textureName = "tree.jpg";
        }
        {
            auto& crown = make(std::make_unique<GameObject>("Crown"));
            crown.position = vec3(t.x, trunkH, t.z);
            crown.mesh = std::make_unique<Mesh>(Mesh::createTaperedCylinder(crownR, 0, crownR * 1.2f));
            crown.textureName = "foliage_dark";
        }
    }

    // ── Lua ─────────────────────────────────────────────────────────
    {
        auto& moon = make(std::make_unique<GameObject>("Moon"));
        moon.position   = vec3(6.0f, 7.0f, -8.0f);
        moon.scale      = vec3(0.8f);
        moon.mesh       = std::make_unique<Mesh>(Mesh::createSphere(1.0f));
        moon.textureName = "moon_glow";
    }

    // ══════════════════════════════════════════════
    //  LÂMPADA (.obj) no teto
    // ══════════════════════════════════════════════

    glm::vec3 lampPositions[4] = {
        vec3(-1.2f, CEILING_Y - 0.15f, -1.0f),
        vec3(-1.2f, CEILING_Y - 0.15f,  1.0f),
        vec3( 1.2f, CEILING_Y - 0.15f, -1.0f),
        vec3( 1.2f, CEILING_Y - 0.15f,  1.0f)
    };
    for (int i = 0; i < 4; i++) {
        auto& lamp = make(std::make_unique<GameObject>("Lamp" + std::to_string(i + 1)));
        lamp.position = lampPositions[i];
        lamp.scale    = vec3(0.3f);
        lamp.model    = std::make_unique<Model>("assets/models/Ceiling Light/model.obj");
    }

    // Player — Yaw 90° para já nascer olhando para a porta (parede Sul)
    m_player->camera.Position = vec3(0.0f, 1.7f, 1.5f);
    m_player->camera.Yaw      = 90.0f;

    buildCreature();
}

void SceneManager::buildCreature() {
    m_creature.setForestCenter(glm::vec3(0.0f));
    auto parts = m_creature.buildBody();
    std::vector<GameObject*> rawPtrs;
    for (auto& part : parts) {
        part->active = false;
        GameObject* ptr = part.get();
        addObject(std::move(part));
        rawPtrs.push_back(ptr);
    }
    m_creature.attachBodyParts(rawPtrs);
    m_creature.active = true;
}
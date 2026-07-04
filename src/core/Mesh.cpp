#include "core/Mesh.h"
#include <cmath>
#include <cstring>

Mesh::Mesh(const std::vector<Vertex>& verts, const std::vector<unsigned int>& indices) {
    m_indexCount = (int)indices.size();
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

Mesh::~Mesh() {
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_ebo) glDeleteBuffers(1, &m_ebo);
}

Mesh::Mesh(Mesh&& other) noexcept {
    m_vao = other.m_vao; m_vbo = other.m_vbo; m_ebo = other.m_ebo; m_indexCount = other.m_indexCount;
    other.m_vao = other.m_vbo = other.m_ebo = 0; other.m_indexCount = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        if (m_vao) glDeleteVertexArrays(1, &m_vao);
        if (m_vbo) glDeleteBuffers(1, &m_vbo);
        if (m_ebo) glDeleteBuffers(1, &m_ebo);
        m_vao = other.m_vao; m_vbo = other.m_vbo; m_ebo = other.m_ebo; m_indexCount = other.m_indexCount;
        other.m_vao = other.m_vbo = other.m_ebo = 0; other.m_indexCount = 0;
    }
    return *this;
}

void Mesh::draw() const {
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
}

// ─── Fábrica: paralelepípedo ─────────────────────────────────────
Mesh Mesh::createBox(float w, float h, float d) {
    float hw = w * 0.5f, hh = h * 0.5f, hd = d * 0.5f;
    std::vector<Vertex> verts = {
        {{-hw, -hh, -hd}, { 0, 0, -1}, {0, 0}}, {{ hw, -hh, -hd}, { 0, 0, -1}, {w, 0}},
        {{ hw,  hh, -hd}, { 0, 0, -1}, {w, h}}, {{-hw,  hh, -hd}, { 0, 0, -1}, {0, h}},
        {{-hw, -hh,  hd}, { 0, 0,  1}, {0, 0}}, {{ hw, -hh,  hd}, { 0, 0,  1}, {w, 0}},
        {{ hw,  hh,  hd}, { 0, 0,  1}, {w, h}}, {{-hw,  hh,  hd}, { 0, 0,  1}, {0, h}},
        {{-hw,  hh,  hd}, {-1, 0,  0}, {d, h}}, {{-hw,  hh, -hd}, {-1, 0,  0}, {0, h}},
        {{-hw, -hh, -hd}, {-1, 0,  0}, {0, 0}}, {{-hw, -hh,  hd}, {-1, 0,  0}, {d, 0}},
        {{ hw,  hh,  hd}, { 1, 0,  0}, {d, h}}, {{ hw,  hh, -hd}, { 1, 0,  0}, {0, h}},
        {{ hw, -hh, -hd}, { 1, 0,  0}, {0, 0}}, {{ hw, -hh,  hd}, { 1, 0,  0}, {d, 0}},
        {{-hw, -hh, -hd}, { 0, -1,  0}, {0, d}}, {{ hw, -hh, -hd}, { 0, -1,  0}, {w, d}},
        {{ hw, -hh,  hd}, { 0, -1,  0}, {w, 0}}, {{-hw, -hh,  hd}, { 0, -1,  0}, {0, 0}},
        {{-hw,  hh, -hd}, { 0,  1,  0}, {0, d}}, {{ hw,  hh, -hd}, { 0,  1,  0}, {w, d}},
        {{ hw,  hh,  hd}, { 0,  1,  0}, {w, 0}}, {{-hw,  hh,  hd}, { 0,  1,  0}, {0, 0}},
    };
    std::vector<unsigned int> idx = {
        0,1,2,2,3,0, 4,5,6,6,7,4, 8,9,10,10,11,8,
        12,13,14,14,15,12, 16,17,18,18,19,16, 20,21,22,22,23,20
    };
    return Mesh(verts, idx);
}

// ─── Fábrica: cilindro ───────────────────────────────────────────
Mesh Mesh::createCylinder(float radius, float height, int segments) {
    std::vector<Vertex> verts;
    std::vector<unsigned int> idx;
    float hh = height * 0.5f;

    // Topo e base: centro
    verts.push_back({{0, -hh, 0}, {0, -1, 0}, {0.5f, 0.5f}}); // base center idx 0
    verts.push_back({{0,  hh, 0}, {0,  1, 0}, {0.5f, 0.5f}}); // top  center idx 1

    for (int i = 0; i <= segments; i++) {
        float a = (float)i / (float)segments * 6.2831853f;
        float ca = cos(a), sa = sin(a);
        float u = (float)i / (float)segments;

        // Base
        verts.push_back({{ca * radius, -hh, sa * radius}, {0, -1, 0}, {u, 1}});
        // Topo
        verts.push_back({{ca * radius,  hh, sa * radius}, {0,  1, 0}, {u, 1}});
        // Lado inferior
        verts.push_back({{ca * radius, -hh, sa * radius}, {ca, 0, sa}, {u, 0}});
        // Lado superior
        verts.push_back({{ca * radius,  hh, sa * radius}, {ca, 0, sa}, {u, 1}});
    }

    int baseCenter = 0, topCenter = 1;
    for (int i = 0; i < segments; i++) {
        int b0 = 2 + i * 4;
        int t0 = 3 + i * 4;
        int b1 = 2 + (i + 1) * 4;
        int t1 = 3 + (i + 1) * 4;
        int lb0 = 4 + i * 4;
        int lt0 = 5 + i * 4;
        int lb1 = 4 + (i + 1) * 4;
        int lt1 = 5 + (i + 1) * 4;

        // Base triangle
        idx.push_back(baseCenter); idx.push_back(b0); idx.push_back(b1);
        // Top triangle
        idx.push_back(topCenter); idx.push_back(t1); idx.push_back(t0);
        // Side quad
        idx.push_back(lb0); idx.push_back(lb1); idx.push_back(lt1);
        idx.push_back(lt1); idx.push_back(lt0); idx.push_back(lb0);
    }

    return Mesh(verts, idx);
}

// ─── Fábrica: árvore (tronco + copa, mesh única) ─────────────────
Mesh Mesh::createTree(float height, float trunkR, float crownR, int segs) {
    std::vector<Vertex> verts;
    std::vector<unsigned int> idx;

    auto addTapered = [&](float rBot, float rTop, float y0, float y1) {
        unsigned int base = (unsigned int)verts.size();
        float hh = (y1 - y0) * 0.5f;
        verts.push_back({{0, y0, 0}, {0, -1, 0}, {0.5f, 0.5f}});
        verts.push_back({{0, y1, 0}, {0,  1, 0}, {0.5f, 0.5f}});
        for (int i = 0; i <= segs; i++) {
            float a = (float)i / (float)segs * 6.2831853f;
            float ca = cosf(a), sa = sinf(a);
            float u = (float)i / (float)segs;
            // base ring
            verts.push_back({{ca * rBot, y0, sa * rBot}, {0, -1, 0}, {u, 1}});
            // top ring
            verts.push_back({{ca * rTop, y1, sa * rTop}, {0,  1, 0}, {u, 1}});
            // side bottom
            glm::vec3 n = glm::normalize(glm::vec3(ca, (float)(rBot - rTop) / hh, sa));
            verts.push_back({{ca * rBot, y0, sa * rBot}, n, {u, 0}});
            // side top
            verts.push_back({{ca * rTop, y1, sa * rTop}, n, {u, 1}});
        }
        unsigned int cbot = base, ctop = base + 1;
        for (int i = 0; i < segs; i++) {
            unsigned int b0 = base + 2 + i * 4, t0 = base + 3 + i * 4;
            unsigned int b1 = base + 2 + (i + 1) * 4, t1 = base + 3 + (i + 1) * 4;
            unsigned int lb0 = base + 4 + i * 4, lt0 = base + 5 + i * 4;
            unsigned int lb1 = base + 4 + (i + 1) * 4, lt1 = base + 5 + (i + 1) * 4;
            idx.push_back(cbot); idx.push_back(b0); idx.push_back(b1);
            idx.push_back(ctop); idx.push_back(t1); idx.push_back(t0);
            idx.push_back(lb0); idx.push_back(lb1); idx.push_back(lt1);
            idx.push_back(lt1); idx.push_back(lt0); idx.push_back(lb0);
        }
    };

    // Tronco: de y=0 até y=height*0.55
    addTapered(trunkR, trunkR * 0.35f, 0, height * 0.55f);

    // Galhos tortos saindo do tronco
    for (int j = 0; j < 3; j++) {
        float ang = (float)j * 2.094f + 0.3f;
        float bx = sinf(ang) * trunkR * 0.8f;
        float bz = cosf(ang) * trunkR * 0.8f;
        float by = height * (0.25f + j * 0.12f);
        float branchLen = crownR * (0.4f + (j % 2) * 0.3f);
        float br = trunkR * 0.25f;
        // galho como cilindro fino inclinado
        float tipX = bx + sinf(ang + 0.5f) * branchLen;
        float tipZ = bz + cosf(ang + 0.5f) * branchLen;
        float tipY = by + branchLen * 0.3f;
        float mx = (bx + tipX) * 0.5f, my = (by + tipY) * 0.5f, mz = (bz + tipZ) * 0.5f;
        float len = sqrtf((tipX - bx) * (tipX - bx) + (tipY - by) * (tipY - by) + (tipZ - bz) * (tipZ - bz));
        if (len < 0.01f) continue;
        // Vetor direção
        glm::vec3 dir(tipX - bx, tipY - by, tipZ - bz);
        glm::vec3 up(0, 1, 0);
        if (fabs(dir.y) > 0.99f) up = glm::vec3(1, 0, 0);
        glm::vec3 right = glm::normalize(glm::cross(dir, up));
        glm::vec3 fwd   = glm::normalize(glm::cross(right, dir));
        // Cilindro fino orientado
        unsigned int base = (unsigned int)verts.size();
        for (int i = 0; i <= segs; i++) {
            float a = (float)i / (float)segs * 6.2831853f;
            float ca = cosf(a), sa = sinf(a);
            float u = (float)i / (float)segs;
            glm::vec3 r = right * (br * ca) + fwd * (br * sa);
            glm::vec3 p0 = glm::vec3(bx, by, bz) + r;
            glm::vec3 p1 = glm::vec3(tipX, tipY, tipZ) + r * 0.3f;
            glm::vec3 n = glm::normalize(r);
            verts.push_back({p0, n, {u, 0}});
            verts.push_back({p1, n, {u, 1}});
        }
        for (int i = 0; i < segs; i++) {
            unsigned int a0 = base + i * 2, a1 = base + i * 2 + 1;
            unsigned int b0 = base + (i + 1) * 2 % (segs * 2), b1 = base + (i + 1) * 2 % (segs * 2) + 1;
            idx.push_back(a0); idx.push_back(b0); idx.push_back(b1);
            idx.push_back(b1); idx.push_back(a1); idx.push_back(a0);
        }
    }

    // Copa: 4 cones inclinados no topo
    for (int j = 0; j < 4; j++) {
        float ang = (float)j * 1.5708f + 0.7f;
        float tilt = 0.3f + (j % 2) * 0.2f;
        float cx = sinf(ang) * tilt * crownR * 0.3f;
        float cz = cosf(ang) * tilt * crownR * 0.3f;
        float baseY = height * 0.5f + (j % 2) * height * 0.05f;
        float coneH = crownR * (0.7f + (j % 3) * 0.15f);
        float coneR = crownR * 0.45f;
        addTapered(coneR, 0, baseY, baseY + coneH);
    }

    // Copa central
    addTapered(crownR * 0.5f, 0, height * 0.55f, height * 1.1f);

    return Mesh(verts, idx);
}

// ─── Fábrica: esfera (para a lua) ─────────────────────────────────
Mesh Mesh::createSphere(float radius, int latBands, int lonBands) {
    std::vector<Vertex> verts;
    std::vector<unsigned int> idx;
    for (int lat = 0; lat <= latBands; lat++) {
        float theta = (float)lat / (float)latBands * 3.14159265f;
        float sinTheta = sinf(theta), cosTheta = cosf(theta);
        for (int lon = 0; lon <= lonBands; lon++) {
            float phi = (float)lon / (float)lonBands * 6.2831853f;
            float sinPhi = sinf(phi), cosPhi = cosf(phi);
            glm::vec3 n(sinTheta * cosPhi, cosTheta, sinTheta * sinPhi);
            verts.push_back({n * radius, n, {(float)lon / lonBands, (float)lat / latBands}});
        }
    }
    for (int lat = 0; lat < latBands; lat++) {
        for (int lon = 0; lon < lonBands; lon++) {
            int first = lat * (lonBands + 1) + lon;
            int second = first + lonBands + 1;
            idx.push_back(first); idx.push_back(second); idx.push_back(first + 1);
            idx.push_back(first + 1); idx.push_back(second); idx.push_back(second + 1);
        }
    }
    return Mesh(verts, idx);
}

// ─── Fábrica: cilindro cônico ────────────────────────────────────
Mesh Mesh::createTaperedCylinder(float radiusBot, float radiusTop, float height, int segments) {
    std::vector<Vertex> verts;
    std::vector<unsigned int> idx;
    float hh = height * 0.5f;

    verts.push_back({{0, -hh, 0}, {0, -1, 0}, {0.5f, 0.5f}});
    verts.push_back({{0,  hh, 0}, {0,  1, 0}, {0.5f, 0.5f}});

    for (int i = 0; i <= segments; i++) {
        float a = (float)i / (float)segments * 6.2831853f;
        float ca = cos(a), sa = sin(a);
        float u = (float)i / (float)segments;
        verts.push_back({{ca * radiusBot, -hh, sa * radiusBot}, {0, -1, 0}, {u, 1}});
        verts.push_back({{ca * radiusTop,  hh, sa * radiusTop}, {0,  1, 0}, {u, 1}});

        glm::vec3 n = glm::normalize(glm::vec3(ca, (radiusBot - radiusTop) / height, sa));
        verts.push_back({{ca * radiusBot, -hh, sa * radiusBot}, n, {u, 0}});
        verts.push_back({{ca * radiusTop,  hh, sa * radiusTop}, n, {u, 1}});
    }

    int baseCenter = 0, topCenter = 1;
    for (int i = 0; i < segments; i++) {
        int b0 = 2 + i * 4, t0 = 3 + i * 4, b1 = 2 + (i + 1) * 4, t1 = 3 + (i + 1) * 4;
        int lb0 = 4 + i * 4, lt0 = 5 + i * 4, lb1 = 4 + (i + 1) * 4, lt1 = 5 + (i + 1) * 4;
        idx.push_back(baseCenter); idx.push_back(b0); idx.push_back(b1);
        idx.push_back(topCenter); idx.push_back(t1); idx.push_back(t0);
        idx.push_back(lb0); idx.push_back(lb1); idx.push_back(lt1);
        idx.push_back(lt1); idx.push_back(lt0); idx.push_back(lb0);
    }
    return Mesh(verts, idx);
}

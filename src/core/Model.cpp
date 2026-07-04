#include "core/Model.h"
#include "core/Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <vector>
#include <functional>

Model::Model(const std::string& path) {
    load(path);
}

void Model::load(const std::string& path) {
    Assimp::Importer importer;
    // ── FlipUVs removido — estava embaralhando as texturas (teclado no monitor) ──
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_OptimizeMeshes);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "[Model] Erro ao carregar: " << path << " - " << importer.GetErrorString() << "\n";
        return;
    }

    struct TempMesh { std::vector<Vertex> verts; std::vector<unsigned int> indices; };
    std::vector<TempMesh> tempMeshes;

    std::function<void(aiNode*)> collectNode = [&](aiNode* node) {
        for (unsigned i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            TempMesh tmp;

            aiColor3D kd(1.0f, 1.0f, 1.0f);
            if (mesh->mMaterialIndex < scene->mNumMaterials) {
                aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
                mat->Get(AI_MATKEY_COLOR_DIFFUSE, kd);
            }
            m_meshKd.push_back(glm::vec3(kd.r, kd.g, kd.b));

            for (unsigned j = 0; j < mesh->mNumVertices; j++) {
                Vertex v;
                v.pos = glm::vec3(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
                if (mesh->HasNormals())
                    v.normal = glm::vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);
                if (mesh->mTextureCoords[0])
                    v.texCoord = glm::vec2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y);
                else
                    v.texCoord = glm::vec2(0.0f);
                m_min = glm::min(m_min, v.pos);
                m_max = glm::max(m_max, v.pos);
                tmp.verts.push_back(v);
            }
            for (unsigned f = 0; f < mesh->mNumFaces; f++) {
                auto& face = mesh->mFaces[f];
                for (unsigned j = 0; j < face.mNumIndices; j++)
                    tmp.indices.push_back(face.mIndices[j]);
            }
            tempMeshes.push_back(std::move(tmp));
        }
        for (unsigned i = 0; i < node->mNumChildren; i++)
            collectNode(node->mChildren[i]);
    };

    collectNode(scene->mRootNode);

    glm::vec3 center = (m_min + m_max) * 0.5f;
    for (auto& tmp : tempMeshes) {
        for (auto& v : tmp.verts)
            v.pos -= center;
        m_meshes.push_back(std::make_unique<Mesh>(tmp.verts, tmp.indices));
    }
    m_min -= center;
    m_max -= center;

    for (auto& kd : m_meshKd) {
        unsigned int tex = 0;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        unsigned char pixel[3] = {
            (unsigned char)(glm::clamp(kd.r, 0.0f, 1.0f) * 255.0f),
            (unsigned char)(glm::clamp(kd.g, 0.0f, 1.0f) * 255.0f),
            (unsigned char)(glm::clamp(kd.b, 0.0f, 1.0f) * 255.0f)
        };
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, pixel);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        m_meshTextures.push_back(tex);
    }

    glm::vec3 size = m_max - m_min;
    std::cout << "[Model] Carregado: " << path << " (" << m_meshes.size() << " meshes)\n";
    std::cout << "[Model] Tamanho real (sem escala aplicada): "
              << size.x << " x " << size.y << " x " << size.z
              << "   |   Min: (" << m_min.x << ", " << m_min.y << ", " << m_min.z << ")"
              << "   Max: (" << m_max.x << ", " << m_max.y << ", " << m_max.z << ")\n";
}

Model::~Model() {
    for (auto tex : m_meshTextures)
        if (tex) glDeleteTextures(1, &tex);
}

void Model::draw(bool usePerMesh) const {
    for (size_t i = 0; i < m_meshes.size(); i++) {
        if (usePerMesh && i < m_meshTextures.size() && m_meshTextures[i])
            glBindTexture(GL_TEXTURE_2D, m_meshTextures[i]);
        m_meshes[i]->draw();
    }
}
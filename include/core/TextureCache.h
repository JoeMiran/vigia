#pragma once
#include <unordered_map>
#include <string>
#include <iostream>
#include <glad/glad.h>

#include "stb_image.h"

class TextureCache {
public:
    static unsigned int get(const std::string& name) {
        auto it = s_cache.find(name);
        if (it != s_cache.end()) return it->second;

        std::string path;
        if (name.find("assets/") == 0 || name.find("assets\\") == 0)
            path = name;
        else
            path = "assets/textures/" + name;
        unsigned int id = load(path);
        s_cache[name] = id;
        return id;
    }

    static unsigned int makeColor(const std::string& name, unsigned char r, unsigned char g, unsigned char b) {
        auto it = s_cache.find(name);
        if (it != s_cache.end()) return it->second;

        unsigned int id;
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        unsigned char pixel[3] = {r, g, b};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, pixel);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        s_cache[name] = id;
        return id;
    }

private:
    static unsigned int load(const std::string& path) {
        unsigned int id;
        glGenTextures(1, &id);

        int w, h, ch;
        stbi_set_flip_vertically_on_load(true);
        const int REQ_CH = 4;  // força RGBA para evitar problemas de alinhamento com GL_RGB
        std::cout << "[DEBUG] TextureCache: antes de stbi_load(" << path << ")\n";
        unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, REQ_CH);
        std::cout << "[DEBUG] TextureCache: depois de stbi_load, data=" << (void*)data << ", w=" << w << " h=" << h << " ch=" << ch << "\n";

        if (data) {
            GLenum fmt = (REQ_CH == 4) ? GL_RGBA : GL_RGB;
            glBindTexture(GL_TEXTURE_2D, id);
            std::cout << "[DEBUG] TextureCache: antes de glTexImage2D (fmt=" << (fmt == GL_RGBA ? "RGBA" : "RGB") << ")\n";
            glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
            std::cout << "[DEBUG] TextureCache: antes de glGenerateMipmap\n";
            glGenerateMipmap(GL_TEXTURE_2D);
            std::cout << "[DEBUG] TextureCache: depois de glGenerateMipmap\n";
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            std::cout << "[Texture] Carregada: " << path << "\n";
        } else {
            std::cerr << "[Texture] FALHA: " << path << "\n";
            // retorna textura branca padrão
            unsigned char white[] = {255, 255, 255};
            glBindTexture(GL_TEXTURE_2D, id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, white);
        }

        stbi_image_free(data);
        return id;
    }

    static inline std::unordered_map<std::string, unsigned int> s_cache;
};
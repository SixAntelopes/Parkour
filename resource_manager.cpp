#include "resource_manager.h"
#include <iostream>
#include <sstream>
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// 初始化静态变量
std::map<std::string, Texture2D>    ResourceManager::Textures;
std::map<std::string, Shader>       ResourceManager::Shaders;

Shader ResourceManager::LoadShader(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, std::string name) {
    Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return Shaders[name];
}

Shader &ResourceManager::GetShader(std::string name) {
    return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const char *file, bool alpha, std::string name, bool is_gif) {
    Textures[name] = loadTextureFromFile(file, alpha, is_gif);
    return Textures[name];
}

Texture2D &ResourceManager::GetTexture(std::string name) {
    return Textures[name];
}

void ResourceManager::Clear() {
    for (auto iter : Shaders)   // 恰当地删除所有渲染器
        glDeleteProgram(iter.second.ID);
    for (auto iter : Textures)  // 恰当地删除所有纹理
        glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile) {
    // 从文件路径获取顶点和片段源代码
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    try {
        // 打开文件
        std::ifstream vertexShaderFile(vShaderFile);
        std::ifstream fragmentShaderFile(fShaderFile);
        std::stringstream vShaderStream, fShaderStream;
        // 读取文件缓冲器内容到流
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        // 关闭文件处理器
        vertexShaderFile.close();
        fragmentShaderFile.close();
        // 把流转换成字符串
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        // 如果有几何源代码路径，加载生成几何渲染器
        if (gShaderFile != nullptr) {
            std::ifstream geometryShaderFile(gShaderFile);
            std::stringstream gShaderStream;
            gShaderStream << geometryShaderFile.rdbuf();
            geometryShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    } catch (std::exception e) {
        std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
    }
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();
    const char *gShaderCode = geometryCode.c_str();
    // 从源代码生成渲染器
    Shader shader;
    shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
    return shader;
}

unsigned char* loadGifFromFile(const char *file, int **delays, int *width, int *height, int *frames, int *nrChannels) {
    FILE* pf = fopen(file, "rb");
    if (!pf) return stbi__errpuc("can't fopen", "Unable to open file");
    fseek(pf, 0L, SEEK_END);    // 移动文件读写指针到最后
    int size = (int)ftell(pf);  // 移动后需要使用这函数获取目前位置
    void* buffer = malloc(size);
    fseek(pf, 0L, SEEK_SET);    // 移回去
    fread(buffer, size, 1, pf); // 把文件输到缓冲中
    fclose(pf);
    unsigned char* data = stbi_load_gif_from_memory((stbi_uc*)buffer, size, delays, width, height, frames, nrChannels, 0);
    free(buffer);
    return data;
}

Texture2D ResourceManager::loadTextureFromFile(const char *file, bool alpha, bool is_gif) {
    // 生成纹理对象
    Texture2D texture;
    if (alpha) {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    // 加载图片
    int width, height, nrChannels;
    if (is_gif) {
        int* delays = nullptr;
        int frames;
        unsigned char* data = loadGifFromFile(file, &delays, &width, &height, &frames, &nrChannels);
        int stride_bytes = width * height * nrChannels;
        // 生成纹理
        texture.Generate(width, height, data, *delays/20, frames, stride_bytes);
// std::cout << texture.frame << " " << texture.frames << std::endl;
// std::cout << *delays << " " << texture.delays << std::endl;
    } else {
        unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
        // 生成纹理
        texture.Generate(width, height, data);
        // 释放图片数据
        stbi_image_free(data);
    }
    return texture;
}

void ResourceManager::UpdateGif(std::string name) {
    Texture2D *gifTexture = &Textures[name];
// std::cout << gifTexture->delay << " " << gifTexture->delays << std::endl;
    if (++gifTexture->delay > gifTexture->delays) {
        gifTexture->delay = 0;
        if (++(gifTexture->frame) == gifTexture->frames)
            gifTexture->frame = 0;
// std::cout << gifTexture->frame << " " << gifTexture->frames << std::endl;
// std::cout << dt << std::endl;
    }
    glBindTexture(GL_TEXTURE_2D, gifTexture->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, gifTexture->Internal_Format, gifTexture->Width, gifTexture->Height, 0, gifTexture->Image_Format, GL_UNSIGNED_BYTE, gifTexture->gif_data + gifTexture->stride_bytes*gifTexture->frame);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void ResourceManager::SpeedUpGif(std::string name, int mult) {
    if (mult < 0)
        Textures[name].delays *= -mult;
    else Textures[name].delays /= mult;
}
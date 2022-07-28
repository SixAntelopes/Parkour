#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#include <map>
#include <string>
#include <glad/glad.h>
#include "texture.h"
#include "shader.h"

class ResourceManager {
public:
    // 资源存贮
    static std::map<std::string, Shader>    Shaders;
    static std::map<std::string, Texture2D> Textures;
    // 从文件加载生成渲染
    static Shader    LoadShader(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, std::string name);
    static Shader    &GetShader(std::string name);  // 获取存贮的渲染器
    // 从文件加载生成纹理
    static Texture2D LoadTexture(const char *file, bool alpha, std::string name, bool is_gif = false);
    static Texture2D &GetTexture(std::string name); // 获取存贮的纹理
    static void      UpdateGif(std::string name); // 更新gif
    static void      SpeedUpGif(std::string name, int nult); // 加速gif
    static void      Clear();   // 恰当地取消被分配的资源
private:
    ResourceManager() { }   // 没有实际的对象，其成员和方法都是静态且公开
    // 从文件加载生成渲染
    static Shader    loadShaderFromFile(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile = nullptr);
    // 从文件加载生成纹理
    static Texture2D loadTextureFromFile(const char *file, bool alpha, bool is_gif);
};

#endif
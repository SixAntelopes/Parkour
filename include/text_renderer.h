#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <map>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "shader.h"

// 保持使用FreeType加载的一个字符的状态信息
struct Character {
    unsigned int TextureID; // ID控制字形纹理
    glm::ivec2   Size;      // 字形大小
    glm::ivec2   Bearing;   // 基线到字形 左边/顶部 的偏移
    unsigned int Advance;   // 前后字形的水平偏移
};

class TextRenderer {
public:
    // 保持包含预编译字符的列表
    std::map<char, Character> Characters;
    Shader TextShader; // 使用文本渲染的着色器
    // 构造函数
    TextRenderer(unsigned int width, unsigned int height);
    // 从给的字符预渲染一列表字符
    void Load(std::string font, unsigned int fontSize);
    // 使用预编译字符列表渲染字符串
    void RenderText(std::string text, float x, float y, float scale
                    , glm::vec3 color = glm::vec3(1.0f));
private:
    // 渲染状态
    unsigned int VAO, VBO;
};

#endif
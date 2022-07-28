#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "sprite_renderer.h"
#include "shader.h"

class PostProcessor {
public:
    // 状态
    Shader PostProcessingShader;
    Texture2D Texture;
    unsigned int Width, Height;
    bool Shake; // 选项
    // 构造函数
    PostProcessor(Shader shader, unsigned int width, unsigned int height);
    // 渲染游戏前准备特效的帧缓冲
    void BeginRender();
    // 渲染游戏后渲染，于是存储所有渲染数据到纹理对象
    void EndRender();
    // 以包围屏幕的精灵来渲染特效纹理方框
    void Render(float time);
private:
    // 渲染状态
    unsigned int MSFBO, FBO; // MSFBO = 多重采样的FBO. FBO是常见的，用于位传送MS颜色缓冲到纹理
    unsigned int RBO; // RBO 是用于多重采样颜色缓冲
    unsigned int VAO;
    // 为渲染特效纹理初始化方块
    void initRenderData();
};

#endif
#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "texture.h"
#include "shader.h"

class SpriteRenderer {
public:
    SpriteRenderer(Shader &shader); //初始化渲染器
    ~SpriteRenderer();
    // 渲染精灵
    void DrawSprite(Texture2D &texture, glm::vec2 position,
        glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f,
        glm::vec3 color = glm::vec3(1.0f));
private:
    Shader       shader;
    unsigned int quadVAO;
    void initRenderData();  // 初始化缓冲和顶点属性
};
#endif
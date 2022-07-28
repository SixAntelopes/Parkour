#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.h"
#include "texture.h"
#include "game_object.h"

// 一个粒子以及状态
struct Particle {
    glm::vec2 Position, Velocity;
    glm::vec4 Color;
    float     Life;
    Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};

class ParticleGenerator {
public:
    // 构造函数
    ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);
    // 更新所有粒子
    void Update(float dt, GameObject &object, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
    void Draw(); // 渲染所有粒子
private:
    std::vector<Particle> particles; // 状态
    unsigned int amount;
    // 渲染状态
    Shader shader;
    Texture2D texture;
    unsigned int VAO;
    // 初始化循环与顶点属性
    void init();
    // 返回第一个消亡的粒子
    unsigned int firstUnusedParticle();
    // 重新激活粒子
    void respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};

#endif
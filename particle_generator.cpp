#include "particle_generator.h"
#include <iostream>
ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount)
    : shader(shader), texture(texture), amount(amount) {
    this->init();
}

void ParticleGenerator::Update(float dt, GameObject &object, unsigned int newParticles, glm::vec2 offset) {
    // 新增新粒子 
    for (unsigned int i = 0; i < newParticles; ++i) {
        int unusedParticle = this->firstUnusedParticle();
        this->respawnParticle(this->particles[unusedParticle], object, offset);
    }
    // 更新所有粒子
    for (unsigned int i = 0; i < this->amount; ++i) {
        Particle &p = this->particles[i];
        p.Life -= dt; // 减短生命
        if (p.Life > 0.0f) {    // 粒子还活着，就更新
            p.Position -= p.Velocity * dt;
            p.Color.a -= dt * 2.5f;
        }
    }
}

// 渲染所有粒子
void ParticleGenerator::Draw() {
    // 更改混合模式使有变强的效果
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    this->shader.Use();
    for (Particle particle : this->particles) {
        if (particle.Life > 0.0f) {
            this->shader.SetVector2f("offset", particle.Position);
            this->shader.SetVector4f("color", particle.Color);
            this->texture.Bind();
            glBindVertexArray(this->VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
    // 重置默认混合模式
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init() {
    // 设立网格及其属性
    unsigned int VBO;
    float particle_quad[] = {
        0.0f, 2.0f, 0.0f, 1.0f,
        2.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 2.0f, 0.0f, 1.0f,
        2.0f, 2.0f, 1.0f, 1.0f,
        2.0f, 0.0f, 1.0f, 0.0f
    }; 
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(this->VAO);
    // 设置网格缓冲
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
    // 设置网格属性
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // 创造默认粒子实例
    for (unsigned int i = 0; i < this->amount; ++i)
        this->particles.push_back(Particle());
}

// 存贮并快速获取消亡的粒子
unsigned int lastUsedParticle = 0;
unsigned int ParticleGenerator::firstUnusedParticle() {
    // 先搜索最后消亡的粒子
    for (unsigned int i = lastUsedParticle; i < this->amount; ++i){
        if (this->particles[i].Life <= 0.0f) {
            lastUsedParticle = i;
            return i;
        }
    }
    // 如果没，把前面搜索下
    for (unsigned int i = 0; i < lastUsedParticle; ++i){
        if (this->particles[i].Life <= 0.0f) {
            lastUsedParticle = i;
            return i;
        }
    }
    // 如果没有消亡粒子就返回第一个粒子
    lastUsedParticle = 0;
    return 0;
}

void ParticleGenerator::respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset) {
    float random = ((rand() % 100) - 50) / 10.0f;
    float rColor = 0.5f + ((rand() % 100) / 100.0f);
    particle.Position = object.Position + random + offset;
    particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.Life = 1.0f;
    particle.Velocity = object.Velocity * 0.1f;
}

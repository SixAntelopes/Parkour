#ifndef GAMELEVEL_H
#define GAMELEVEL_H
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "game_object.h"
#include "sprite_renderer.h"
#include "resource_manager.h"

class GameLevel {
public:
    unsigned int score = 0;
    glm::vec2   UnitSize;
    glm::vec2   LevelSize;
    int         LevelTrack;
    glm::vec2   LevelVelocity;
    Texture2D   LevelSprite1;
    Texture2D   LevelSprite2;
    bool        add_brick;
    std::vector<GameObject> Bricks;
    // 车道线
    Texture2D   LaneSprite;
    std::vector<GameObject> Lanes;
    GameLevel(glm::vec2 unit_size, glm::vec2 size, glm::vec2 velocity
            , Texture2D sprite1, Texture2D sprite2, Texture2D laneSprite);    // 构造函数
    void AddBrick(unsigned int width);
    void Update(float dt);
    // // 从文件加载关卡
    // void Load(const char *file, unsigned int levelWidth, unsigned int levelHeight);
    void Draw(SpriteRenderer &renderer); // 渲染关卡
    // // 检查关卡是否结束
    // bool IsCompleted();
private:
    // 初始化关卡
    // void init(std::vector<std::vector<unsigned int>> tileData
    //             , unsigned int levelWidth, unsigned int levelHeight);
};

#endif
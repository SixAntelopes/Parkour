#include "game_level.h"
// #include <fstream>
// #include <sstream>
#include <cstdlib>
#include <iostream>

GameLevel::GameLevel(glm::vec2 unit_size, glm::vec2 size, glm::vec2 velocity
                    , Texture2D sprite1, Texture2D sprite2, Texture2D laneSprite)
            : UnitSize(unit_size), LevelSize(size), LevelVelocity(velocity)
                , LevelSprite1(sprite1), LevelSprite2(sprite2), LaneSprite(laneSprite) {
        LevelTrack = size.x / unit_size.x;
        add_brick = true;
        glm::vec2 laneSize(10.0f, 100.0f);
        for (int i = 0; i != 4; ++i) {
            for (int j = 1; j <= 4; ++j) {
                glm::vec2 pos(j * UnitSize.x - laneSize.x, i * (laneSize.y + 160.0f));
                GameObject obj(pos, laneSize, this->LaneSprite);
                this->Lanes.push_back(obj);
            }
        }
    }

void GameLevel::AddBrick(unsigned int width) {
    int offset = rand() % 10 - rand() % 10;
    int height = -UnitSize.y + offset;
    glm::vec2 pos(width + offset, height);
    glm::vec2 size(UnitSize.y + rand() % 30, UnitSize.y);
    Texture2D sprite;
    glm::vec3 color = glm::vec3(1.0f);
    if (rand() % 11 == 0) sprite = LevelSprite2;
        else {
            sprite = LevelSprite1;
            float RColor = 0.5f + ((rand() % 100) / 100.0f);
            float GColor = 0.5f + ((rand() % 100) / 100.0f);
            float BColor = 0.5f + ((rand() % 100) / 100.0f);
            color = glm::vec3(RColor, GColor, BColor);
        }
    for (GameObject &tile : this->Bricks) {
        if (tile.Destroyed) {
            tile.Position = pos;
            tile.Size = size;
            tile.Sprite = sprite;
            tile.Color = color;
            tile.Destroyed = false;
            return;
        }
    }
    GameObject obj(pos, size, sprite, color);
    this->Bricks.push_back(obj);
}

void GameLevel::Update(float dt) {
    // 车道线
    for (GameObject &lane : Lanes) {
        lane.Position += this->LevelVelocity * dt;
        if (lane.Position.y >= this->LevelSize.y) {
            glm::vec2 pos(lane.Position.x, -lane.Size.y + (lane.Position.y - this->LevelSize.y));
            lane.Position = pos;
        }
    }
    if(this->add_brick) {
        // srand((unsigned int)(glfwGetTime()/dt));
        int brick_num_max = rand()%(LevelTrack-2)+2;
        int brick_num = 0;
        // std::cout << LevelTrack << "# " << brick_num << "@ ";
        for (int i = 0; i <= brick_num_max; ++i) {
            if (brick_num != brick_num_max && rand() % 2 == 0) {
            // int brick_id = rand()%LevelTrack;
            // std::cout << brick_id << " ";
                AddBrick(i*UnitSize.x);
                ++brick_num;
            }
        }
        // std::cout << std::endl;
    }
    this->add_brick = true;
    int subScore = 0;
    for (GameObject &tile : this->Bricks) {
        if (!tile.Destroyed) {
            tile.Position += this->LevelVelocity * dt;
            // if (tile.Position.y + tile.Size.y >= this->LevelSize.y + UnitSize.y) {
            if (tile.Position.y >= this->LevelSize.y) {
                subScore = LevelVelocity.y;
                tile.Destroyed = true;
            }
            if (tile.Position.y <= UnitSize.y * 5)
                this->add_brick = false;
        }
    }
    score += subScore / 100;
    // std::cout << score << std::endl;
}

// void GameLevel::Load(const char *file, unsigned int levelWidth, unsigned int levelHeight) {
//     // 清理旧数据
//     this->Bricks.clear();
//     // 从文件加载
//     unsigned int tileCode;
//     GameLevel level;
//     std::string line;
//     std::ifstream fstream(file);
//     std::vector<std::vector<unsigned int>> tileData;
//     if (fstream) {
//         // 从关卡文件一行行读取
//         while (std::getline(fstream, line)) {
//             std::istringstream sstream(line);
//             std::vector<unsigned int> row;
//             while (sstream >> tileCode) // 读每个空格隔开的字
//                 row.push_back(tileCode);
//             tileData.push_back(row);
//         }
//         if (tileData.size() > 0)
//             this->init(tileData, levelWidth, levelHeight);
//     }
// }

void GameLevel::Draw(SpriteRenderer &renderer) {
    for (GameObject &lane : this->Lanes)
        lane.Draw(renderer);
    for (GameObject &tile : this->Bricks)
        if (!tile.Destroyed)
            tile.Draw(renderer);
}

// bool GameLevel::IsCompleted() {
//     for (GameObject &tile : this->Bricks)
//         if (!tile.IsSolid && !tile.Destroyed)
//             return false;
//     return true;
// }

// void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight) {
//     // 计算维度
//     unsigned int height = tileData.size();
//     unsigned int width = tileData[0].size();
//     float unit_width = levelWidth / static_cast<float>(width), unit_height = levelHeight / height;
//     // 通过数据初始化关卡
//     for (unsigned int y = 0; y < height; ++y) {
//         for (unsigned int x = 0; x < width; ++x) {
//             // 检查方块类型
//             if (tileData[y][x] == 1) {
//                 glm::vec2 pos(unit_width * x, unit_height * y);
//                 glm::vec2 size(unit_width, unit_height);
//                 GameObject obj(pos, size, ResourceManager::GetTexture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
//                 obj.IsSolid = true;
//                 this->Bricks.push_back(obj);
//             } else
//                 if (tileData[y][x] > 1) {
//                     glm::vec3 color = glm::vec3(1.0f);
//                     if (tileData[y][x] == 2)
//                         color = glm::vec3(0.2f, 0.6f, 1.0f);
//                     else if (tileData[y][x] == 3)
//                         color = glm::vec3(0.0f, 0.7f, 0.0f);
//                     else if (tileData[y][x] == 4)
//                         color = glm::vec3(0.8f, 0.8f, 0.4f);
//                     else if (tileData[y][x] == 5)
//                         color = glm::vec3(1.0f, 0.5f, 0.0f);
//                     glm::vec2 pos(unit_width * x, unit_height * y);
//                     glm::vec2 size(unit_width, unit_height);
//                     this->Bricks.push_back(GameObject(pos, size, ResourceManager::GetTexture("block"), color));
//                 }
//         }
//     }
// }

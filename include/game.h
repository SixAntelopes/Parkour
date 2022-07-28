#ifndef GAME_H
#define GAME_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// #include "power_up.h"

// 游戏当前状态
enum GameState {
    GAME_ACTIVE,    // 活跃
    GAME_MENU,  // 菜单
    GAME_WIN    // 胜利
};

// 四种碰撞方向
enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

// 把碰撞数据自定义
typedef std::tuple<bool, Direction, glm::vec2> Collision;

// 初始化角色大小
const glm::vec2 PLAYER_SIZE(60.0f, 45.0f);
// 初始化角色速度
const float PLAYER_VELOCITY(1000.0f);
// 初始化关卡速度
const glm::vec2 INITIAL_VELOCITY(0.0f, 200.0f);
// 初始化生命值
const int INITIAL_LIVE = 5;
// // 初始化挡板大小
// const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
// // 初始化挡板速度
// const float PLAYER_VELOCITY(800.0f);
// // 初始化球速度
// const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -200.0f);
// // 球半径
// const float BALL_RADIUS = 12.5f;

class Game {
    public:
        GameState               State;  // 游戏状态
        bool                    Keys[1024]; // 处理输入的数组
        bool                    KeysProcessed[1024];
        unsigned int            Width, Height;
        // std::vector<GameLevel>  Levels;
        // std::vector<PowerUp>    PowerUps;
        // unsigned int            Level;
        unsigned int            Lives;
        unsigned int            bestScore;
        unsigned int            player_gif_delay;
        
        Game(unsigned int width, unsigned int height);  // 构造函数
        ~Game();    // 析构函数
        void Init();    // 初始化游戏状态（着色器、纹理、关卡）
        // 游戏循环
        void ProcessInput(float dt);    // 处理输入
        void Update(float dt);  // 更新游戏状态
        void Render();  // 渲染
        void DoCollisions(); //碰撞处理
        // 复位
        void ResetLevel();
        // void ResetPlayer();
        // // 道具
        // void SpawnPowerUps(GameObject &block);
        // void UpdatePowerUps(float dt);
};

#endif
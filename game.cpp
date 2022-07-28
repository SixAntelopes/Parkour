#include <windows.h>
#include <algorithm>
#include <sstream>

#include <iostream>
#include <string>

#include <irrKlang.h>
using namespace irrklang;

#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include "game_level.h"
// #include "ball_object.h"
// #include "particle_generator.h"
#include "post_processor.h"
#include "text_renderer.h"
// #include "power_up.h"
#include "pipe_server.h"

// 游戏相关状态数据
SpriteRenderer  *Renderer;
GameObject      *Player;
GameLevel       *Level;
// BallObject      *Ball;
// ParticleGenerator *Particles;
// PostProcessor     *Effects;
ISoundEngine      *SoundEngine = createIrrKlangDevice();
TextRenderer    *Text;
NamePipeServ    *namePipe;

float ShakeTime = 0.0f;
bool collided = false;  // 是否碰撞
int add_time = 1;

Game::Game(unsigned int width, unsigned int height) 
	: State(GAME_MENU), Keys(), Width(width), Height(height), Lives(INITIAL_LIVE), bestScore(0) { 
}

Game::~Game() {
	delete Renderer;
    delete Player;
    delete Level;
    // delete Ball;
    // delete Particles;
    // delete Effects;
    delete namePipe;
    SoundEngine->drop();
}

void Game::Init() {
    // 加载着色器
    ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");
    // ResourceManager::LoadShader("shaders/particle.vs", "shaders/particle.frag", nullptr, "particle");
    ResourceManager::LoadShader("shaders/post_processing.vs", "shaders/post_processing.frag", nullptr, "postprocessing");
    // 配置着色器
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
    // ResourceManager::GetShader("particle").SetMatrix4("projection", projection);
    // 加载纹理
    ResourceManager::LoadTexture("textures/pixel_miku.gif", true, "player", true);
    player_gif_delay = ResourceManager::GetTexture("player").delays;
    // ResourceManager::LoadTexture("textures/awesomeface.png", true, "player");
    ResourceManager::LoadTexture("textures/background.png", false, "background");
    ResourceManager::LoadTexture("textures/title.png", true, "title");
    ResourceManager::LoadTexture("textures/start_sign.png", true, "start_sign");
    // ResourceManager::LoadTexture("textures/awesomeface.png", true, "face");
    // ResourceManager::LoadTexture("textures/block.png", false, "block");
    // ResourceManager::LoadTexture("textures/block_solid.png", false, "block_solid");
    ResourceManager::LoadTexture("textures/obstacle.png", true, "obstacle");
    ResourceManager::LoadTexture("textures/banana.png", true, "banana");
    ResourceManager::LoadTexture("textures/lane.png", false, "lane");
    // ResourceManager::LoadTexture("textures/paddle.png", true, "paddle");
    // ResourceManager::LoadTexture("textures/particle.png", true, "particle");
    // ResourceManager::LoadTexture("textures/powerup_speed.png", true, "powerup_speed");
    // ResourceManager::LoadTexture("textures/powerup_sticky.png", true, "powerup_sticky");
    // ResourceManager::LoadTexture("textures/powerup_increase.png", true, "powerup_increase");
    // ResourceManager::LoadTexture("textures/powerup_confuse.png", true, "powerup_confuse");
    // ResourceManager::LoadTexture("textures/powerup_chaos.png", true, "powerup_chaos");
    // ResourceManager::LoadTexture("textures/powerup_passthrough.png", true, "powerup_passthrough");
    // 设置用于渲染的控制
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    // Particles = new ParticleGenerator(ResourceManager::GetShader("particle")
    //                                 , ResourceManager::GetTexture("particle"), 500);
    // Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), this->Width, this->Height);
    Text = new TextRenderer(this->Width, this->Height);
    Text->Load("fonts/Zfull-GB.ttf", 24);
    // 加载关卡
    Level = new GameLevel(glm::vec2(100.0f, 50.0f), glm::vec2(this->Width - 2*100.0f, this->Height)
                        , INITIAL_VELOCITY, ResourceManager::GetTexture("obstacle")
                        , ResourceManager::GetTexture("banana")
                        , ResourceManager::GetTexture("lane"));
    // GameLevel one; one.Load("levels/one.lvl", this->Width, this->Height / 2);
    // GameLevel two; two.Load("levels/two.lvl", this->Width, this->Height / 2);
    // GameLevel three; three.Load("levels/three.lvl", this->Width, this->Height / 2);
    // GameLevel four; four.Load("levels/four.lvl", this->Width, this->Height / 2);
    // this->Levels.push_back(one);
    // this->Levels.push_back(two);
    // this->Levels.push_back(three);
    // this->Levels.push_back(four);
    // this->Level = 0;
    // 配置对象
    glm::vec2 playerPos = glm::vec2(Level->UnitSize.x/2.0f-PLAYER_SIZE.x/2.0f, this->Height-PLAYER_SIZE.y*2);
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("player"));
    // glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
    // Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
    // 音频
    SoundEngine->play2D("audio/parkour.mp3", true);
    // 命名管道
    namePipe = new NamePipeServ();
    WinExec("./capHead.exe", SW_SHOWNORMAL);
}

void Game::Update(float dt) {
    ResourceManager::UpdateGif("player");
    if (collided) {
        Sleep(100);
        collided = false;
    }
    if (Level->score > add_time * 1000) {
        ++Lives;
        ++add_time;
    }
    // 更新关卡
    if (this->State == GAME_ACTIVE)
        Level->Update(dt);
    // // 更新球
    // Ball->Move(dt, this->Width);
    // 检测碰撞
    this->DoCollisions();
    // 更新粒子
    // Particles->Update(dt, *Player, 4, glm::vec2(Player->Size.x / 4.0f));
    // // update PowerUps
    // this->UpdatePowerUps(dt)a;
    // 减少shake时间
    // if (ShakeTime > 0.0f) {
    //     ShakeTime -= dt;
    //     if (ShakeTime <= 0.0f)
    //         Effects->Shake = false;
    // }
    // // 检查是否触底
    // if (Ball->Position.y >= this->Height) {
    //     --this->Lives;
    //     // 玩家是否失去所有生命? : 游戏结束
    //     if (this->Lives == 0) {
    //         this->ResetLevel();
    //         this->State = GAME_MENU;
    //     }
    //     this->ResetPlayer();
    // }
    // // 检查获胜状态
    // if (this->State == GAME_ACTIVE && this->Levels[this->Level].IsCompleted()) {
    //     this->ResetLevel();
    //     this->ResetPlayer();
    //     Effects->Chaos = true;
    //     this->State = GAME_WIN;
    // }
}

void Game::ProcessInput(float dt) {
    if (this->State == GAME_MENU) {
        if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER]) {
            this->State = GAME_ACTIVE;
            this->KeysProcessed[GLFW_KEY_ENTER] = true;
            // 清理旧数据
            Level->Bricks.clear();
            Player->Position.x = Level->UnitSize.x/2.0f - PLAYER_SIZE.x/2.0f;
            this->Lives = INITIAL_LIVE;
            bestScore = Level->score;
            Level->score = 0;
        }
        // if (this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W]) {
        //     this->Level = (this->Level + 1) % 4;
        //     this->KeysProcessed[GLFW_KEY_W] = true;
        // }
        // if (this->Keys[GLFW_KEY_S] && !this->KeysProcessed[GLFW_KEY_S]) {
        //     if (this->Level > 0)
        //         --this->Level;
        //     else
        //         this->Level = 3;
        //     //this->Level = (this->Level - 1) % 4;
        //     this->KeysProcessed[GLFW_KEY_S] = true;
        // }
    }
    // if (this->State == GAME_WIN) {
    //     if (this->Keys[GLFW_KEY_ENTER]) {
    //         this->KeysProcessed[GLFW_KEY_ENTER] = true;
    //         Effects->Chaos = false;
    //         this->State = GAME_MENU;
    //     }
    // }
    if (this->State == GAME_ACTIVE) {
        Player->Color = glm::vec3(1.0f);
        Player->Position.y = this->Height- 2 * PLAYER_SIZE.y;
        Level->LevelVelocity = INITIAL_VELOCITY;
        ResourceManager::GetTexture("player").delays = player_gif_delay;
        char ctrl_btn = '\0';
        ctrl_btn = namePipe->readPipe();    // 从管道读取
        this->Keys[ctrl_btn] = true;
        float velocity = PLAYER_VELOCITY * dt;
        // 移动挡板
        // std::cout << this->Keys[GLFW_KEY_A] << " " << this->KeysProcessed[GLFW_KEY_A] << std::endl;
        if (this->Keys[GLFW_KEY_R]) {
            // std::cout << ctrl_btn << std::endl;
            this->KeysProcessed[GLFW_KEY_A] = false;
            this->KeysProcessed[GLFW_KEY_D] = false;
        }
        if (this->Keys[GLFW_KEY_A] && !this->KeysProcessed[GLFW_KEY_A]) {
            this->KeysProcessed[GLFW_KEY_A] = true;
            // std::cout << ctrl_btn << std::endl;
            // Player->Position.x -= velocity;
            Player->Position.x -= Level->UnitSize.x;
            if (Player->Position.x < 0) {
            Player->Position.x = Level->UnitSize.x/2.0f - PLAYER_SIZE.x/2.0f;
                // if (Ball->Stuck)
                //     Ball->Position.x -= velocity;
            }
        }
        if (this->Keys[GLFW_KEY_D] && !this->KeysProcessed[GLFW_KEY_D]) {
            this->KeysProcessed[GLFW_KEY_D] = true;
            // std::cout << ctrl_btn << std::endl;
            // Player->Position.x += velocity;
            Player->Position.x += Level->UnitSize.x;
            if (Player->Position.x > Level->LevelSize.x - PLAYER_SIZE.x/2.0f) {
                Player->Position.x = Level->LevelSize.x - Level->UnitSize.x/2.0f - Player->Size.x/2.0f;
                // if (Ball->Stuck)
                //     Ball->Position.x += velocity;
            }
        }
        if (this->Keys[GLFW_KEY_SPACE]) {
            // std::cout << ctrl_btn << std::endl;
            Level->LevelVelocity = glm::vec2(0.0f, 4 * INITIAL_VELOCITY.y);
            ResourceManager::SpeedUpGif("player", 10);
        }
        this->Keys[ctrl_btn] = false;
    }
}

void Game::Render() {
        // , glm::vec3(0.0f, 1.0f, 0.0f)
    if(this->State == GAME_ACTIVE || this->State == GAME_MENU || this->State == GAME_WIN) {
        // 渲染之前处理帧缓冲
        // Effects->BeginRender();
            // 渲染背景
            Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f)
                                , glm::vec2(this->Width, this->Height), 0.0f);
            // 渲染关卡
            Level->Draw(*Renderer);
            // 渲染粒子
            // Particles->Draw();
            // 渲染角色
            Player->Draw(*Renderer);
        //     // 渲染道具
        //     for (PowerUp &powerUp : this->PowerUps)
        //         if (!powerUp.Destroyed)
        //             powerUp.Draw(*Renderer);
        //     // 渲染球
        //     Ball->Draw(*Renderer);
        // 结束渲染后处理帧缓冲
        // Effects->EndRender();
        // 渲染特效方框
        // Effects->Render(glfwGetTime());
        // 渲染文本（不在特效中）
        Renderer->DrawSprite(ResourceManager::GetTexture("title"), glm::vec2(Level->LevelSize.x+25.0f, 25.0f)
                            , glm::vec2(150.0f, 37.5f), 0.0f);
        // Text->RenderText("Parkour", Level->LevelSize.x+20.0f, 10.0f, 1.5f, glm::vec3(0.22f, 0.79f, 0.73f));
        std::stringstream s1, s2, s3; s1 << this->Lives; s2 << Level->score; s3 << bestScore;
        Text->RenderText("Best:  " + s3.str(), Level->LevelSize.x+25.0f, 150.0f, 1.0f);
        Text->RenderText("Score:  " + s2.str(), Level->LevelSize.x+25.0f, 200.0f, 1.0f);
        Text->RenderText("Lives:  " + s1.str(), Level->LevelSize.x+25.0f, 350.0f, 1.0f);
    }
    if (this->State == GAME_MENU) {
        Renderer->DrawSprite(ResourceManager::GetTexture("start_sign"), glm::vec2(this->Width / 4.0f, this->Height / 2.0f)
                            , glm::vec2(300.0f, 36.3f), 0.0f);
        // Text->RenderText("Press ENTER to start", this->Width / 4.0f, this->Height / 2.0f, 1.0f);
        // Text->RenderText("Press W or S to select level", 245.0f, this->Height / 2.0f + 20.0f, 0.75f);
    }
    // if (this->State == GAME_WIN) {
    //     Text->RenderText("You WON!!!", 320.0f, this->Height / 2.0f - 20.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    //     Text->RenderText("Press ENTER to retry or ESC to quit", 130.0f, this->Height / 2.0f, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
    // }
}

// void Game::ResetPlayer() {
//     // 重置player/ball状态
//     Player->Size = PLAYER_SIZE;
//     Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
//     Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)) , INITIAL_BALL_VELOCITY);
//     // 禁用所有特效
//     Effects->Chaos = Effects->Confuse = false;
//     Ball->PassThrough = Ball->Sticky = false;
//     Player->Color = glm::vec3(1.0f);
//     Ball->Color = glm::vec3(1.0f);
// }

// 道具
// bool IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type);

// void Game::UpdatePowerUps(float dt) {
//     for (PowerUp &powerUp : this->PowerUps) {
//         powerUp.Position += powerUp.Velocity * dt;
//         if (powerUp.Activated) {
//             powerUp.Duration -= dt;
//             if (powerUp.Duration <= 0.0f) {
//                 // 从列表把道具移除
//                 powerUp.Activated = false;
//                 // 仅当没有其他此道具激活中时重置
//                 if (powerUp.Type == "sticky") {
//                     if (!IsOtherPowerUpActive(this->PowerUps, "sticky")) {
//                         Ball->Sticky = false;
//                         Player->Color = glm::vec3(1.0f);
//                     }
//                 } else
//                     if (powerUp.Type == "pass-through") {
//                         if (!IsOtherPowerUpActive(this->PowerUps, "pass-through")) {
//                             Ball->PassThrough = false;
//                             Ball->Color = glm::vec3(1.0f);
//                         }
//                 } else
//                     if (powerUp.Type == "confuse") {
//                         if (!IsOtherPowerUpActive(this->PowerUps, "confuse")) {
//                             Effects->Confuse = false;
//                         }
//                 } else
//                     if (powerUp.Type == "chaos") {
//                         if (!IsOtherPowerUpActive(this->PowerUps, "chaos")) {
//                             Effects->Chaos = false;
//                         }
//                 }
//             }
//         }
//     }
//     this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
//         [](const PowerUp &powerUp) { return powerUp.Destroyed && !powerUp.Activated; }
//     ), this->PowerUps.end());
// }

// bool ShouldSpawn(unsigned int chance) {
//     unsigned int random = rand() % chance;
//     return random == 0;
// }
// void Game::SpawnPowerUps(GameObject &block) {
//     if (ShouldSpawn(75)) // 1/75机率
//         this->PowerUps.push_back(PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f
//                             , block.Position, ResourceManager::GetTexture("powerup_speed")));
//     if (ShouldSpawn(75))
//         this->PowerUps.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f
//                             , block.Position, ResourceManager::GetTexture("powerup_sticky")));
//     if (ShouldSpawn(75))
//         this->PowerUps.push_back(PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f
//                             , block.Position, ResourceManager::GetTexture("powerup_passthrough")));
//     if (ShouldSpawn(75))
//         this->PowerUps.push_back(PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f
//                             , block.Position, ResourceManager::GetTexture("powerup_increase")));
//     if (ShouldSpawn(15)) // 不好道具会经常生成
//         this->PowerUps.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f
//                             , block.Position, ResourceManager::GetTexture("powerup_confuse")));
//     if (ShouldSpawn(15))
//         this->PowerUps.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f
//                             , block.Position, ResourceManager::GetTexture("powerup_chaos")));
// }

// void ActivatePowerUp(PowerUp &powerUp) {
//     if (powerUp.Type == "speed") {
//         Ball->Velocity *= 1.2;
//     } else if (powerUp.Type == "sticky") {
//         Ball->Sticky = true;
//         Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
//     } else if (powerUp.Type == "pass-through") {
//         Ball->PassThrough = true;
//         Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
//     } else if (powerUp.Type == "pad-size-increase") {
//         Player->Size.x += 50;
//     } else if (powerUp.Type == "confuse") {
//         if (!Effects->Chaos)
//             Effects->Confuse = true; // 只有chaos未激活时激活
//     } else if (powerUp.Type == "chaos") {
//         if (!Effects->Confuse)
//             Effects->Chaos = true;
//     }
// }

// bool IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type) {
//     for (const PowerUp &powerUp : powerUps) {
//         if (powerUp.Activated)
//             if (powerUp.Type == type)
//                 return true;
//     }
//     return false;
// }

// 碰撞侦测
// bool CheckCollision(GameObject &one, GameObject &two);
Collision CheckCollision(GameObject &one, GameObject &two);
// Direction VectorDirection(glm::vec2 closest);

void Game::DoCollisions() {
    for (GameObject &box : Level->Bricks) {
        if (!box.Destroyed) {
            Collision collision = CheckCollision(*Player, box);
            // std::cout << std::get<1>(collision) << std::endl;
            if (std::get<0>(collision)) { // 如果碰撞了
                // 方块非坚硬就毁掉
                // if (!box.IsSolid) {
                //     box.Destroyed = true;
                //     // this->SpawnPowerUps(box);
                //     // SoundEngine->play2D("audio/bleep.mp3", false);
                // } else {   // 如果坚硬方块就激活shake特效
                        // ShakeTime = 0.05f;
                        // Effects->Shake = true;
                        SoundEngine->play2D("audio/solid.mp3", false);
//                     }
                // 碰撞处理
                // Direction dir = std::get<1>(collision);
                // glm::vec2 diff_vector = std::get<2>(collision);
                // // 不碰撞当穿墙道具启动
                // if (!(Player->PassThrough && !box.IsSolid)) {
                // std::cout << Player->PassThrough << std::endl;
                // 水平碰撞
                // if (dir == LEFT || dir == RIGHT) {
                //     Player->Velocity.x = -Player->Velocity.x; // 反转水平速度
                //     // 重定位
                //     // float penetration = Player->Size.x / 2.0f - std::abs(diff_vector.x);
                //     float penetration = Player->Size.x / 2.0f;
                //     if (dir == LEFT)
                //         Player->Position.x += penetration; // 把球右移
                //     else
                //         Player->Position.x -= penetration; // 把球左移
                //     // Level->score /= 2;  // 分数减一半
                // } else { // 垂直碰撞
                    Player->Velocity.y = -Player->Velocity.y; // 反转垂直速度
                    // Player->Velocity.y = -1 * abs(Player->Velocity.y);
                    // 重定位
                    // float penetration = Player->Size.x / 2.0f - std::abs(diff_vector.y);
                    float penetration = Player->Size.x / 2.0f;
                    // if (dir == UP)
                    //     Player->Position.y -= penetration; // 把球上移
                    // else
                        Player->Position.y += penetration; // 把球下移
                // }
                    box.Destroyed = true;   //消除方块
                    Player->Color = glm::vec3(1.0f, 0.0f, 0.0f);    //变色
                    Level->LevelVelocity = INITIAL_VELOCITY;    // 减速
                    Level->score -= Level->score / 5;  // 分数减1/5
                    add_time = Level->score / 1000 + 1;
                    collided = true;
                    --this->Lives;  // 减命
                    if (this->Lives == 0)   // 游戏结束
                        this->State = GAME_MENU;
                this->Keys[GLFW_KEY_SPACE] = false;
                this->Keys[GLFW_KEY_A] = false;
                this->Keys[GLFW_KEY_D] = false;
            }
//         }
    }
//     // 顺便检测如果碰撞了道具，激活他们
//     for (PowerUp &powerUp : this->PowerUps) {
//         if (!powerUp.Destroyed) {
//             // 先检测如果道具是否过了底边，保持不活跃并摧毁
//             if (powerUp.Position.y >= this->Height)
//                 powerUp.Destroyed = true;
//             if (CheckCollision(*Player, powerUp)) { // 被碰撞了，激活他
//                 ActivatePowerUp(powerUp);
//                 powerUp.Destroyed = true;
//                 powerUp.Activated = true;
//                 SoundEngine->play2D("audio/powerup.rf64", false);
//             }
//         }
//     }
//     Collision result = CheckCollision(*Ball, *Player); // 检测挡板碰撞
//     if (!Ball->Stuck && std::get<0>(result)) {
//         // 检测在哪里碰撞，并在那改变速度
//         float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
//         float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
//         float percentage = distance / (Player->Size.x / 2.0f);
//         // 然后接着移动
//         float strength = 2.0f;
//         glm::vec2 oldVelocity = Ball->Velocity;
//         Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength; 
//         // Ball->Velocity.y = -Ball->Velocity.y;
//         // 保持速度一致
//         Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
//         Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y); // 修复粘板
//         // 如果粘板道具激活，则在计算速度后粘板
//         Ball->Stuck = Ball->Sticky;
//         SoundEngine->play2D("audio/bleep.rf64", false);
    }
}

// AABB - AABB 碰撞
// bool CheckCollision(GameObject &one, GameObject &two) {
//     // x轴碰撞
//     bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
//         two.Position.x + two.Size.x >= one.Position.x;
//     // y轴碰撞
//     bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
//         two.Position.y + two.Size.y >= one.Position.y;
//     // 只有两轴碰撞才算碰撞
//     return collisionX && collisionY;
// }

// AABB - 圆碰撞
Collision CheckCollision(GameObject &one, GameObject &two) {
    // 获取圆中心
    float radius = one.Size.x / 2.0f;
    glm::vec2 center(one.Position + radius);
    // 计算 AABB 数据（中心，半边长
    glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
    glm::vec2 aabb_center(two.Position.x + aabb_half_extents.x, two.Position.y + aabb_half_extents.y);
    // 计算两中心点矢量差
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // 得到碰撞箱距离圆最近的点
    glm::vec2 closest = aabb_center + clamped;
    // 检测最近的点是否被圆碰到
    difference = closest - center;
    if (glm::length(difference) < radius)
    //     return std::make_tuple(true, VectorDirection(difference), difference);
        return std::make_tuple(true, UP, difference);
    // else
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

// 计算碰撞方向
// Direction VectorDirection(glm::vec2 target) {
//     glm::vec2 compass[] = {
//         glm::vec2(0.0f, 1.0f),	// up
//         glm::vec2(1.0f, 0.0f),	// right
//         glm::vec2(0.0f, -1.0f),	// down
//         glm::vec2(-1.0f, 0.0f)	// left
//     };
//     float max = 0.0f;
//     unsigned int best_match = -1;
//     for (unsigned int i = 0; i < 4; i++) {
//         float dot_product = glm::dot(glm::normalize(target), compass[i]);
//         if (dot_product > max) {
//             max = dot_product;
//             best_match = i;
//         }
//     }
//     return (Direction)best_match;
// }
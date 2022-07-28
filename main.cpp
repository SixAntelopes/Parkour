#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "game.h"
#include "resource_manager.h"
// 回调函数声明
// void framebuffer_size_callback(GLFWwindow*, int, int);
void key_callback(GLFWwindow*, int, int, int, int);
const int SCREEN_WIDTH = 600;   // 屏幕长度
const int SCREEN_HEIGHT = 900;  // 屏幕宽度
const float FRAME_NUM = 1.0f / 30.0f; // 帧数
Game Parkout(SCREEN_WIDTH, SCREEN_HEIGHT);

int main(int argc, char *argv[]) {
    // 初始化GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, false);
    // 创建一个窗口对象
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT,
                                        "Parkout", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    // 加载glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // 注册回调
    glfwSetKeyCallback(window, key_callback);
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 初始化游戏
    Parkout.Init();
    // 配置OpenGL
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    // 增量时间变量
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        // 计算增量时间
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        if (deltaTime < FRAME_NUM)
            continue;
        lastFrame = currentFrame;
        glfwPollEvents();
        // 管理用户输入
        Parkout.ProcessInput(deltaTime);
        // 更新游戏状态
        Parkout.Update(deltaTime);
        // 渲染
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        Parkout.Render();
        glfwSwapBuffers(window);
    }
    // 删除所有资源
    ResourceManager::Clear();
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)
            Parkout.Keys[key] = true;
        else if (action == GLFW_RELEASE) {
            Parkout.Keys[key] = false;
            Parkout.KeysProcessed[key] = false;
        }
    }
}

// void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
//     glViewport(0, 0, width, height);
// }

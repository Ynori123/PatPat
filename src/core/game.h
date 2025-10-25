#ifndef GAME_H
#define GAME_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <glm/glm.hpp>

// 获取句柄
#include "SDL3/SDL_system.h"

#include <string>


// 定义HitTest穿透
#ifndef SDL_HETTEST_TRANSPARENT
#define SDL_HETTEST_TRANSPARENT ((SDL_HitTestResult)2)
#endif // !SDL_HETTEST_TRANSPARENT

class DesktopPet; // 前向声明
class CatPet; // 前向声明

// 单例模式
class Game
{
public:
    static Game& getInstance(){
        static Game instance;
        return instance;
    }

    void init();
    void update(float deltaTime);
    void handleEvent();
    void render();
    void run();
    void clean();

    // getters 
    SDL_Renderer* getRenderer() const {return renderer_;}

    // Getters
    std::string getTitle();

private:
    Game(){};   // 私有化构造函数，防止外部实例化
    Game(const Game&) = delete; // 禁止拷贝构造
    Game& operator=(const Game&) = delete;  // 禁止赋值操作

    // Windows窗口相关
    HWND hwnd_ = nullptr;    // 保存Windows窗口句柄
    bool is_transparent_ = false;    // 是否透明

    // SDL相关
    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;

    // UI相关
    glm::vec2 window_size_ = glm::vec2(800, 600);   // 窗口大小，有默认值，实际上不需要

    // 游戏相关
    std::string title_ = "PatPat";   // 游戏标题
    bool is_running_ = false;
    Uint64 FPS_ = 60;  // 帧率
    Uint64 frame_delay_ = 0; // 帧延时（间隔），即每帧耗时，单位ns
    float dt = 0.0f; // 每帧时间差，单位秒，测试用

    // 桌宠相关
    CatPet* pet_ = nullptr; // 桌宠指针

};

#endif // GAME_H
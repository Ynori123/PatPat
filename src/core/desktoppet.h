#ifndef DESKTOPPET_H
#define DESKTOPPET_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include "animation.h"
#include <glm/glm.hpp>

class Game; // 前置声明


// 状态
enum class PetState{
    IDLE,
    WALK,
    CLICK
};

class DesktopPet{
public:
    DesktopPet();
    virtual ~DesktopPet();

    virtual void init() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void handleEvent(SDL_Event& event) = 0;
    virtual void clean() = 0;

    // Animation related
    virtual bool loadAnimations() = 0; // 加载动画
    virtual void playAnimation(PetState state); // 播放动画

    // Actual actions
    // virtual void updatePosition(float deltaTime){
    //     posX_ += moveSpeed_.x * deltaTime;
    //     posY_ += moveSpeed_.y * deltaTime;
    // }

    // Getters
    virtual std::vector<AnimationFrame> getFrames(std::string& spritePath, int frameHeight, int frameWidth, int frameCount); // 从动画中提取帧添加到动画帧列表中
    int getWidth() const {return petWidth_;}
    int getHeight() const {return petHeight_;}
    int getWidth(int Scale = 1) const {return petWidth_ * Scale;}
    int getHeight(int Scale = 1) const {return petHeight_ * Scale;}
    int getViewScale() const {return viewScale_;}
    virtual void getPosition(int& x, int& y) const;
    SDL_Rect getRect() const {return SDL_Rect{posX_, posY_, petWidth_, petHeight_};}
    bool getMovementState() const;

    // Setters
    virtual void setPosition(int x, int y);
    virtual void setWidthAndHeight(SDL_Texture* texture, int totalFrames);
    virtual void setRenderer(SDL_Renderer* renderer) {renderer_ = renderer;}


protected:

    virtual void setState(PetState state); // 设置状态
    virtual void handleEventClick(SDL_Event& event) = 0; // 处理点击事件

    // animation
    SDL_Renderer* renderer_; // 渲染器
    SDL_Texture* spriteSheet_; // 精灵表
    std::vector<std::string> animationPaths_; // 动画路径
    virtual void changePetScale() {petWidth_ *= viewScale_; petHeight_ *= viewScale_;} // 改变宠物缩放

    // mappers
    std::unordered_map<PetState, std::unique_ptr<Animation>> animations_; // 状态对应的动画
    std::unordered_map<PetState, bool> movementStates_; // 状态对应的是否为移动动画

    PetState currentState_; // 当前状态
    int petWidth_, petHeight_; // 宠物宽高
    int posX_, posY_; // 宠物位置
    int viewScale_ = 3; // 视图缩放
    glm::vec2 moveSpeed_ = {20, 0}; // 移动速度（像素/帧）
    
    // As for Timers, I recommend set them in child classes since it will give more flexibility
};


#endif // DESKTOPPET_H
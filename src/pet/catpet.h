#ifndef CATPET_H
#define CATPET_H

#include "core/desktoppet.h"
#include "../tools/Timer.h"

class CatPet : public DesktopPet{
public:
    CatPet();
    ~CatPet();

    void init() override;
    void update(float dt) override;
    void render() override;
    void handleEvent(SDL_Event& event) override;
    void clean() override;
    bool loadAnimations() override;

    // actual actions
    void walkAround(float dt); // 四处走动
    void setWalkAction(float dt);

protected:
    virtual void setState(PetState state) override; // 设置状态
    virtual void handleEventClick(SDL_Event& event) override; // 处理点击事件

    glm::vec2 moveSpeed_ = {360, 0}; // 移动速度（像素/秒）

    // Timer
    Timer walkTimer_;
    float walkInterval_min_ = 1.0f; // 每隔5秒尝试走动一次
    float walkInterval_max_ = 5.0f; // 每隔15秒尝试走动一次

    // actual actions
    int target_positionX_ = 500;  // walk to target position X

    // animations
    bool flipX_ = false; // 是否水平翻转
    int viewScale_ = 3; // 视图缩放
    void changePetScale() override {petWidth_ *= viewScale_; petHeight_ *= viewScale_;} // 改变宠物缩放
};

#endif // CATPET_H
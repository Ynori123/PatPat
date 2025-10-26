#include "desktoppet.h"

DesktopPet::DesktopPet()
{
    renderer_ = nullptr;
    spriteSheet_ = nullptr;
    currentState_ = PetState::IDLE;
    petWidth_ = 0;
    petHeight_ = 0;
    posX_ = 0;
    posY_ = 0;
    viewScale_ = 1;
}

DesktopPet::~DesktopPet()
{
}

void DesktopPet::getPosition(int& x, int& y) const
{
    x = posX_;
    y = posY_;
}

void DesktopPet::setPosition(int x, int y)
{
    posX_ = x;
    posY_ = y;
}

void DesktopPet::setWidthAndHeight(SDL_Texture* texture, int totalFrames)
{
    // 如果获取失败就使用默认值
    float textureWidth = 48.0f * totalFrames, textureHeight = 48.0f;
    if(!SDL_GetTextureSize(texture, &textureWidth, &textureHeight)){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to get texture size");
        return;
    }

    petWidth_ = static_cast<int>(textureWidth / totalFrames);
    petHeight_ = static_cast<int>(textureHeight);

    SDL_Log("DesktopPet::setWidthAndHeight -> frame size=%dx%d (from texture %.0fx%.0f, totalFrames=%d)",
            petWidth_, petHeight_, textureWidth, textureHeight, totalFrames);

    return;
}

void DesktopPet::setState(PetState state)
{
    currentState_ = state;
}

void DesktopPet::playAnimation(PetState state)
{
    // 暂时借用AI的，明天再改，先要把animation.cpp和.h改好
    auto it = animations_.find(state);
    if (it != animations_.end() && it->second) {
        // 假设 Animation::render 需要传入渲染器和位置等参数
        it->second->render(renderer_, posX_, posY_, petWidth_, petHeight_);
    } else {
        // 没有对应动画，播放待机动画
        auto idleIt = animations_.find(PetState::IDLE);
        if (idleIt != animations_.end() && idleIt->second) {
            idleIt->second->render(renderer_, posX_, posY_, petWidth_, petHeight_);
        } else {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No animation found for state and no idle animation available");
        }
    }
}

std::vector<AnimationFrame> DesktopPet::getFrames(std::string &spritePath, int frameHeight, int frameWidth, int frameCount)
{

    std::vector<AnimationFrame> frames;

    // 载入spriteSheet
    SDL_Texture* spriteSheet = IMG_LoadTexture(renderer_, spritePath.c_str());
    if(!spriteSheet){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load texture: %s", SDL_GetError());
        return {};
    }

    // 获取宽高
    float width = 0, height = 0;
    if(!SDL_GetTextureSize(spriteSheet, &width, &height)){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to get texture size: %s", SDL_GetError());
        return {};
    }
    // 转化成int
    int texWidth = static_cast<int>(width);
    int texHeight = static_cast<int>(height);

    // 计算该帧的宽和高(每一帧一样宽)
    int singleFrameWidth = texWidth / frameCount;
    int singleFrameHeight = texHeight;

    AnimationFrame frame;
    for(int i = 0; i < frameCount; ++i){
        frame.souceRect = {i * singleFrameWidth, 0, singleFrameWidth, singleFrameHeight};
        frame.duration = 100; // 每帧持续时间，毫秒

        frames.push_back(frame);    // 添加到帧列表
    }

    // 释放资源
    SDL_DestroyTexture(spriteSheet);

    return frames;
}

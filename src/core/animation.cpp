#include "animation.h"

Animation::Animation()
{
}

Animation::~Animation()
{
}

void Animation::init(SDL_Texture* texture,
                     const std::vector<AnimationFrame>& frames,
                     bool is_loop)
{
    if(texture == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Texture is null");
        return;
    }

    texture_ = texture;
    frames_ = frames;
    isLooping_ = is_loop;
    currentFrame_ = 0;
    frameTimer_ = 0;
    isFinished_ = false;

}

void Animation::update(float deltaTime)
{
    if(isFinished_ || frames_.empty()){
        return;
    }

    // frameTimer_ 统计时间，超出动画的duration，则切换到下一帧，duration int 毫秒
    frameTimer_ += static_cast<int>(deltaTime * 1000); // 转换为毫秒
    if(frameTimer_ >= frames_[currentFrame_].duration){
        currentFrame_++;
        frameTimer_ = 0;

        if(currentFrame_ >= static_cast<int>(frames_.size())){
            if(isLooping_){
                currentFrame_ = 0;
            }else{
                isFinished_ = true;
                currentFrame_ = static_cast<int>(frames_.size()) - 1; // 保持在最后一帧
            }
        }
    }

}

void Animation::render(SDL_Renderer* renderer,
                        int x, int y, int width, int heifht,
                        bool flipHorizontal)
{
    if(renderer == nullptr || texture_ == nullptr || frames_.empty()){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer or texture is null, or frames are empty");
        return;
    }

    // 获取当前帧的源矩形
    SDL_Rect srcRect = frames_[currentFrame_].souceRect;

    // 设置要渲染的位置和大小
    SDL_FRect destRect = { 
        static_cast<float>(x), 
        static_cast<float>(y),
        static_cast<float>(width),
        static_cast<float>(heifht) };

    // 当前帧转化为 float 版本的矩形
    SDL_FRect  srcFRect = { 
        static_cast<float>(srcRect.x),
        static_cast<float>(srcRect.y),
        static_cast<float>(srcRect.w),
        static_cast<float>(srcRect.h)
    };

    // 渲染当前帧
    if(flipHorizontal){
        SDL_FRect flipDestRect = destRect;
        flipDestRect.x += destRect.w;
        flipDestRect.w = -destRect.w; // 水平翻转宽度为负
        SDL_RenderTexture(renderer, texture_, &srcFRect, &flipDestRect);
    } else{
        SDL_RenderTexture(renderer, texture_, &srcFRect, &destRect);
    }

}

void Animation::resetAnimation()
{
    currentFrame_ = 0;
    frameTimer_ = 0;
    isFinished_ = false;
}

void Animation::setLooping(bool is_loop){
    isLooping_ = is_loop;
}

int Animation::getFrameCount() const
{
    return static_cast<int>(frames_.size());
}

void Animation::clean(){
    if(texture_ != nullptr){
        SDL_DestroyTexture(texture_);
        texture_ = nullptr;
    }
    frames_.clear();
}

// --------------------------------------------------------------
// functions for AnimationDescription

Manifest loadManifestFromFile(const std::string& filepath){
    SDL_Texture* tex
}

// Set defaults for any missing fields in AnimationDescription
bool normalizeWithDefaults(AnimationDescription& d, const Defaults& defaults){
    if(d.fps <= 0) d.fps = defaults.fps;
    if(d.frameWidth <= 0) d.frameWidth = defaults.frameWidth;
    if(d.frameHeight <= 0) d.frameHeight = defaults.frameHeight;
    if(d.layout != "row" && d.layout != "column") d.layout = defaults.layout;
    // no need to set loop since it has a default value
}

/* 
// now no need to build frames from aimed rectangles
// because i just use regular png sprite sheet
// same as grid layout
// let me do it later if needed
// When there are many png sheets, it is better to put them together into a big sheet
// and use the grid layout to cut them out
// It will greatly reduce the io operations
*/
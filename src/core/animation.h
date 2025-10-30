#ifndef ANIMATION_H
#define ANIMATION_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <vector>
#include <string>
#include <unordered_map>

// 动画帧结构体
struct AnimationFrame {
    SDL_Rect souceRect; // 帧的源矩形（一般是整个sprite）
    int duration;   // 当前帧的持续时间，毫秒
};

// 可选：用于从 JSON 指定的矩形构建帧
struct AnimFrameRect {
    int x = 0, y = 0, w = 0, h = 0;
    int durationMS = 100; // ms
};


class Animation{
public:
    Animation();
    ~Animation();

    void init(SDL_Texture* texture,
              const std::vector<AnimationFrame>& frames,
              bool is_loop = true);

    void update(float deltaTime);

    void render(SDL_Renderer* renderer, 
                int x, int y, int width, int height, 
                bool flipHorizontal = false);

    void clean();
    
    // Setters
    void resetAnimation();   // 重置动画（帧，帧索引）
    void setLooping(bool is_loop_); // 设置是否循环播放

    // Getters
    int getFrameCount() const;
    bool isFinished() const { return isFinished_; }
    bool isLooping() const { return isLooping_; }

private:
    SDL_Texture* texture_; // 动画纹理
    std::vector<AnimationFrame> frames_; // 动画帧容器
    int currentFrame_; // 当前帧索引
    int frameTimer_; // 帧计时器
    bool isLooping_; // 是否循环播放
    bool isFinished_; // 是否播放完毕

};

struct AnimationDescription {
    std::string name; // 动画名称
    std::string path; // 资源路径(相对路径)
    int frames{-1};    // 帧数量（可选，<=0 表示自动计算）
    int frameWidth{-1};  // 帧宽度
    int frameHeight{-1}; // 帧高度
    int rows{-1}, cols{-1}; // 行数和列数
    int fps{-1};         // 每秒帧数
    bool loop{true};       // 是否循环
    std::string layout; // 布局方式 "row" 或 "column"
    std::vector<AnimFrameRect> rects; // 可选，手动指定每帧的源矩形
    bool is_movement{false}; // 是否为移动动画
};

struct Defaults {
    int frameWidth{48};  // 默认帧宽度
    int frameHeight{48}; // 默认帧高度
    int fps{8};         // 默认每秒帧数
    bool loop{true};       // 默认循环
    std::string layout{"row"}; // 默认布局方式
    bool is_movement{false}; // 默认是否为移动动画
};
struct Manifest {
    int version{1}; // 版本号
    std::string basePath; // 基础路径
    Defaults defaults; // 默认值
    std::unordered_map<std::string, AnimationDescription> animations;
};



#endif // AINIMATION_H
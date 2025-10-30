#include "game.h"
#include "pet/catpet.h"
#include "../tools/tools.h"
#include "../tools/hittest.h"



void Game::init()
{
    // SDL 初始化
    if(!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO)){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init Error: %s", SDL_GetError());
        return;
    }
    // 不需要对SDL_image初始化，会自动初始化
    // SDL3_Mixer初始化
    if(Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) != (MIX_INIT_MP3 | MIX_INIT_OGG)){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Mix_Init Error: %s", SDL_GetError());
        return;
    }
    if(!Mix_OpenAudio(0,NULL)){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Mix_OpenAudio Error: %s", SDL_GetError());
        return;
    }
    Mix_AllocateChannels(16); // 分配16个音频通道
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4); // 设置背景音乐音量为最大音量的1/4
    Mix_Volume(-1, MIX_MAX_VOLUME / 4); // 设置所有音效音量为最大音量的1/4

    // SDL3_ttf初始化
    if(!TTF_Init()){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_Init Error: %s", SDL_GetError());
        return;
    }

    // 获取屏幕大小（整型像素）
    int screenW = 0, screenH = 0;
    tools::UI::getWindowSize(screenW, screenH);
    window_size_.x = screenW;
    window_size_.y = screenH;
    SDL_Log("Window size: %d x %d", window_size_.x, window_size_.y);

    // 创建 透明 置顶 无边框 窗口
    Uint32 windowFlages = SDL_WINDOW_TRANSPARENT | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_BORDERLESS;
    SDL_CreateWindowAndRenderer(getTitle().c_str(), window_size_.x, window_size_.y, windowFlages, &window_, &renderer_);
    if(!window_ || !renderer_){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindowAndRenderer Error: %s", SDL_GetError());
    }

    // 启动与构建信息
// #ifdef NDEBUG
//     SDL_Log("Build: Release");
// #else
//     SDL_Log("Build: Debug");
// #endif
//     SDL_Log("Target FPS: %llu", static_cast<unsigned long long>(FPS_));

    // 透明窗口与混合设置
    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 0); // 清屏为全透明
    SDL_Log("Renderer blend mode set to BLEND, clear color RGBA(0,0,0,0)");

    // 获取 HWND 设置窗口扩展样式
    SDL_Window *window = SDL_GetWindowFromID(SDL_GetWindowID(window_)); // 获取SDL_Window指针
    if(window){
        // 获取窗口句柄，windows平台专用
        HWND hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(window), "SDL.window.win32.hwnd", NULL);
        if(hwnd){
            SDL_Log("now we have hwnd: %p", hwnd);
            hwnd_ = hwnd; // 保存窗口句柄
            // 设置窗口拓展
            LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
            exStyle |= WS_EX_LAYERED; // 设置窗口为分层窗口，支持透明
            SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);

            // is_transparent_ = false;
        }
    }
    
    // 强制窗口获得焦点，确保鼠标事件分发
    SDL_RaiseWindow(window_);

    // to do: 初始化桌宠
    pet_ = new CatPet();
    pet_->setRenderer(renderer_);
    pet_->init(); // CatPet::init 内部已负责加载动画与设置初始状态

    // 不再使用 SDL 窗口 HitTest 进行点击穿透（该回调用于边框拖拽/调整大小）
    // 改为基于 Win32 WS_EX_TRANSPARENT 动态切换鼠标穿透
    SDL_Log("HitTest disabled; using WS_EX_TRANSPARENT toggling based on mouse position");

    // 设置窗口逻辑分辨率
    SDL_SetRenderLogicalPresentation(renderer_, window_size_.x, window_size_.y, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    // frame delay ns（按目标 FPS 计算）
    frame_delay_ = static_cast<Uint64>(1'000'000'000ULL / (FPS_ > 0 ? FPS_ : 60));

    // 初始化FPS统计
    fps_last_report_ns_ = SDL_GetTicksNS();
    fps_frame_count_ = 0;
    fps_last_value_ = 0.0f;

    // now we are running
    is_running_ = true;
}

void Game::update(float deltaTime)
{
    if(pet_){
        pet_->update(deltaTime);
    }
    // 每帧末尾根据鼠标位置切换点击穿透
    if(hwnd_ && pet_){
        SDL_Point mouse = tools::UI::getClientMousePosition(hwnd_);
        SDL_Rect rect = pet_->getRect();
        int scale = pet_->getViewScale();
        rect.w *= scale;
        rect.h *= scale;
        tools::UI::ChangeTransparentState(hwnd_, mouse, rect, is_transparent_);
    }
}

void Game::handleEvent()
{
    SDL_Event e;
    while(SDL_PollEvent(&e)){
        if(e.type == SDL_EVENT_QUIT){
            is_running_ = false;
        }
        if(pet_){
            pet_->handleEvent(e);
        }
    }


}

void Game::render()
{
    SDL_RenderClear(renderer_);
    if(pet_){
        pet_->render();
    }
    SDL_RenderPresent(renderer_);
}

void Game::run()
{
    while(is_running_){

        // 获取每帧开始时间
        auto start_time = SDL_GetTicksNS();

        handleEvent();
        update(1 / 60.0f);
        render();

        // 获取每帧结束时间
        auto end_time = SDL_GetTicksNS();
        auto frame_time = end_time - start_time;    // 计算每帧耗时

        // 正确的帧率限制：如果本帧耗时小于目标间隔，则延迟剩余时间
        if(frame_time < frame_delay_){
            SDL_DelayNS(frame_delay_ - frame_time);  // ns
            dt = static_cast<float>(frame_delay_) / 1.0e9f; // 秒（对齐到目标帧间隔）
        }else{
            dt = static_cast<float>(frame_time) / 1.0e9f; // 秒（真实帧间隔）
        }

        // 累计用于FPS统计
        fps_frame_count_++;
        Uint64 now_ns = end_time;
        Uint64 elapsed_ns = now_ns - fps_last_report_ns_;
        if(elapsed_ns >= 3'000'000'000ULL){ // 每约1秒输出一次
            fps_last_value_ = static_cast<float>(fps_frame_count_) * (1.0e9f / static_cast<float>(elapsed_ns));
            float avg_frame_ms = 1000.0f / (fps_last_value_ > 0.0f ? fps_last_value_ : 1.0f);
            SDL_Log("FPS: %.2f | avg frame: %.3f ms", fps_last_value_, avg_frame_ms);
            fps_last_report_ns_ = now_ns;
            fps_frame_count_ = 0;
        }
    }
}

void Game::clean()
{
    if(pet_){
        pet_->clean();
        delete pet_;
        pet_ = nullptr;
    }

    if(renderer_){
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }

    if(window_){
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }

    TTF_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
}

// -------------------------------------------------------

std::string Game::getTitle(){
    return title_;
}
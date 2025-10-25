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

    // 获取屏幕大小
    tools::UI::getWindowSize((int&)window_size_.x, (int&)window_size_.y);
    SDL_Log("Window size: %d x %d", window_size_.x, window_size_.y);

    // 创建 透明 置顶 无边框 窗口
    Uint32 windowFlages = SDL_WINDOW_TRANSPARENT | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_BORDERLESS;
    SDL_CreateWindowAndRenderer(getTitle().c_str(), window_size_.x, window_size_.y, windowFlages, &window_, &renderer_);
    if(!window_ || !renderer_){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindowAndRenderer Error: %s", SDL_GetError());
    }

    // 获取 HWND 设置窗口扩展样式
    SDL_Window *window = SDL_GetWindowFromID(SDL_GetWindowID(window_)); // 获取SDL_Window指针
    if(window){
        // 获取窗口句柄，windows平台专用
        HWND hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(window), "SDL.window.win32.hwnd", NULL);
        if(hwnd){
            SDL_Log("now we have hwnd: %d", hwnd);
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
    pet_->init();
    pet_->loadAnimations();

    // 注册HitTest回调
    SDL_SetWindowHitTest(window_, PetHitTestCallback, pet_);

    // 设置窗口逻辑分辨率
    SDL_SetRenderLogicalPresentation(renderer_, window_size_.x, window_size_.y, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    // frame delay ns
    frame_delay_ = 1.0e9f / 60.0f;

    // now we are running
    is_running_ = true;
}

void Game::update(float deltaTime)
{
    if(pet_){
        pet_->update(deltaTime);
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
        if(frame_delay_ < frame_time){
            SDL_DelayNS(frame_delay_ - frame_time);  // ns
            dt = frame_delay_ / 1.0e9f; // 秒
        }else{
            dt = frame_time / 1.0e9f; // 秒
        }

        SDL_Log("FPS: %f", 1.0f / dt);
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
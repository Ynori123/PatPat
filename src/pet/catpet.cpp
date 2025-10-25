#include "catpet.h"
#include "../tools/tools.h"
#include "../tools/manifest_loader.h"
#include <algorithm>

// map names in manifest to PetState(no caring about uppercase or lowercase)
static PetState mapNameToState(std::string name){
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    if(name == "idle") return PetState::IDLE;
    if(name == "walk") return PetState::WALK;
    if(name == "click") return PetState::CLICK;
    // more states can be added here
    return PetState::IDLE; // default
}

CatPet::CatPet()
{
    // Nothing to do ?
    currentState_ = PetState::IDLE; // 默认状态
}

CatPet::~CatPet()
{
    // clean up resources in clean()
}

void CatPet::init()
{
    // 初始化位置
    int screenW = 0, screenH = 0;
    tools::UI::getWindowSize(screenW, screenH);
    posX_ = screenW / 2; // 居中
    posY_ = static_cast<int> (screenH * 0.8f); // 屏幕下方

    // Atcually no need, since paths are in json
    // 初始化动画路径
    // animationPaths_.push_back("resources\\sprites\\CatPet\\idle_anim.png"); // IDLE
    // animationPaths_.push_back("resources\\sprites\\CatPet\\walk_anim.png"); // WALK
    // animationPaths_.push_back("resources\\sprites\\CatPet\\click_anim.png"); // CLICK

    // load animations from manifest
    if(!loadAnimations()){
        SDL_Log("CatPet::init: loadAnimations failed");
        // fallback or exit
    }

    // set initial state
    setState(PetState::IDLE);
}

void CatPet::update(float dt)
{
    // 更新当前动画
    auto it = animations_.find(currentState_);
    if(it != animations_.end() && it->second){
        it->second->update(dt);
    } else{
        // 没有当前状态动画
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No animation found for current state in CatPet::update");
        // 尝试使用IDLE动画作为后备
        auto idleIt = animations_.find(PetState::IDLE);
        if(idleIt != animations_.end() && idleIt->second){
            idleIt->second->update(dt);
        }
    }
}

void CatPet::render()
{
    playAnimation(currentState_);
}

void CatPet::handleEvent(SDL_Event &event)
{
    // now only handle mouse click events
    handleEventClick(event);

}

void CatPet::clean()
{
    // clean up animations
    for(auto &kv : animations_){
        if(kv.second){
            kv.second->clean();
        }
    }
    animations_.clear();

    spriteSheet_ = nullptr;
}

bool CatPet::loadAnimations()
{
    if(!renderer_){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer is null in CatPet::loadAnimations");
        return false;
    }

    // first, load manifest
    Manifest mf;
    std::string err;
    const std::string manifestPath = "resources/sprites/CatPet/manifest.json";
    if(!loadManifest(manifestPath, mf, &err)){
        SDL_Log("CatPet::loadAnimations: Failed to load manifest: %s", err.c_str());
        return false;
    }

    bool sizeSet = false;

    // then, run through animations in manifest and load their textures
    for(auto &kv : mf.animations){
        AnimationDesc desc = kv.second;
        normalizeDesc(desc, mf.defaults);

        // get full path
        const std::string fullPath = (mf.basePath.empty() ? desc.path : (mf.basePath + "/" + desc.path));

        // load texture
        SDL_Texture* tex = loadTexture(renderer_, fullPath);
        if(!tex){
            SDL_Log("CatPet::loadAnimations: Failed to load texture: %s", fullPath.c_str());
            continue; // skip this animation
        }

        // extract frames
        float texW = 0, texH = 0; // if cannot get frames, just caculate
        // use SDL_GetTextureSize to get width and height
        SDL_GetTextureSize(tex, &texW, &texH);
        if(desc.frames <= 0){
            if(desc.layout == "grid" && desc.rows > 0 && desc.cols > 0){
                desc.frames = desc.rows * desc.cols;
            } else if(desc.frameWidth > 0){
                desc.frames = static_cast<int>(texW / desc.frameWidth);
            }
        }

        // create frames(list)
        std::vector<AnimationFrame> frames;
        if(!desc.rects.empty()){
            frames = buildFramesFromRects(desc);
        }else{
            frames = buildFramesFromGrid(desc, texW, texH);
        }

        if(frames.empty()){
            SDL_Log("CatPet::loadAnimations: No frames extracted for animation: %s", fullPath.c_str());
            SDL_DestroyTexture(tex);
            continue; // skip this animation
        }

        // Now, create Animation and init it
        auto anim = std::make_unique<Animation>();
        anim->init(tex, frames, desc.loop);

        // then, set size of pet if first animation loaded
        if(!sizeSet){
            setWidthAndHeight(tex, static_cast<int>(frames.size()));
            sizeSet = true;
        }

        // now, store animation in map
        PetState st = mapNameToState(desc.name);
        animations_[st] = std::move(anim);

        return !animations_.empty();
    }
}

void CatPet::setState(PetState state){
    // update state and reset animation
    DesktopPet::setState(state);
    auto it = animations_.find(state);
    if(it != animations_.end() && it->second){
        it->second->resetAnimation();
    }
}

void CatPet::handleEventClick(SDL_Event& event){
    if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN){
        if(event.button.button == SDL_BUTTON_LEFT){
            setState(PetState::CLICK);
            SDL_Log("CatPet::handleEventClick: Cat clicked, switching to CLICK state");
        }
    }
}
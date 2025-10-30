#include "catpet.h"
#include "../tools/tools.h"
#include "../tools/manifest_loader.h"
#include "../tools/random.h"
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
    SDL_Log("CatPet::init screen: %dx%d, initial pos: (%d,%d)", screenW, screenH, posX_, posY_);

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
    // set initial size
    changePetScale();
    // petWidth_ *= viewScale_;
    // petHeight_ *= viewScale_;
    SDL_Log("CatPet::init: pet size: %dx%d", petWidth_, petHeight_);

    // initialize timers
    walkTimer_.start();
    walkTimer_.setInterval(tools::Random::randfloat(walkInterval_min_, walkInterval_max_), true);
}

void CatPet::update(float dt)
{

    setWalkAction(dt);

    // 更新当前动画
    auto it = animations_.find(currentState_);
    if(it != animations_.end() && it->second){

        // if move then update position
        if(getMovementState()){
            // updatePosition(dt);
            walkAround(dt);
            //SDL_Log("CatPet::update: pet movement state=%d pos=(%d,%d)", (int)currentState_, posX_, posY_);
        }
        it->second->update(dt);
        // Just used simple method here
        // Actually need better state machine
        // while animation is finished and not looping, switch back to IDLE
        if(!it->second->isLooping() && it->second->isFinished()){
            setState(PetState::IDLE);
        }
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
    // 周期性输出渲染信息，便于定位不可见问题
    // static Uint64 lastLogNs = 0;
    // Uint64 now = SDL_GetTicksNS();
    // if(now - lastLogNs > 1'000'000'000ULL){
    //     SDL_Log("CatPet::render state=%d pos=(%d,%d) size=%dx%d", (int)currentState_, posX_, posY_, petWidth_, petHeight_);
    //     lastLogNs = now;
    // }

    // render with flipX_
    if(flipX_){
        float sx = 1.0f, sy = 1.0f; // scale of screen(renderer actually)
        SDL_GetRenderScale(renderer_, &sx, &sy);

        // translate to center, scale -1 in X, translate back
        SDL_SetRenderScale(renderer_, -sx, sy);

        // since scale is -1 in X, need to adjust position
        int tempX = posX_;
        posX_ = -(posX_ + petWidth_);

        playAnimation(currentState_);

        // back to normal
        posX_ = tempX;
        SDL_SetRenderScale(renderer_, sx, sy);
    }else{
        playAnimation(currentState_);
    }
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
    // SDL_Log("CatPet::loadAnimations: manifest loaded. basePath='%s', animations=%zu", mf.basePath.c_str(), mf.animations.size());

    bool sizeSet = false;

    // then, run through animations in manifest and load their textures
    for(auto &kv : mf.animations){
        AnimationDescription desc = kv.second;
        normalizeDesc(desc, mf.defaults);

        // get full path
    const std::string fullPath = (mf.basePath.empty() ? desc.path : (mf.basePath + desc.path));

        // load texture
        // SDL_Log("CatPet::loadAnimations: loading animation '%s' from '%s'", desc.name.c_str(), fullPath.c_str());
        SDL_Texture* tex = loadTexture(renderer_, fullPath);
        if(!tex){
            SDL_Log("CatPet::loadAnimations: Failed to load texture: %s", fullPath.c_str());
            continue; // skip this animation
        }

        // extract frames
        float texW = 0, texH = 0; // if cannot get frames, just caculate
        // use SDL_GetTextureSize to get width and height
    SDL_GetTextureSize(tex, &texW, &texH);
    // SDL_Log("CatPet::loadAnimations: texture size for '%s': %.0fx%.0f", desc.name.c_str(), texW, texH);
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
        // SDL_Log("CatPet::loadAnimations: animation '%s' mapped to state=%d, frames=%zu, loop=%d", desc.name.c_str(), (int)st, frames.size(), (int)desc.loop);
        animations_[st] = std::move(anim);  // store animation
        movementStates_[st] = desc.is_movement; // store movement state
    }

    if(animations_.empty()){
        SDL_Log("CatPet::loadAnimations: no animations loaded.");
    }
    return !animations_.empty();
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
            // SDL_Log("CatPet::handleEventClick: Cat clicked, switching to CLICK state");
        }
    }
}

// actual actions
void CatPet::setWalkAction(float dt){
    // 如果触发
    if(walkTimer_.update(dt)){
        // do not break if already walking
        if(currentState_ == PetState::WALK){
            return;
        }
        // reset interval for next walk
        walkTimer_.setInterval(tools::Random::randfloat(walkInterval_min_, walkInterval_max_), true);
        target_positionX_ = tools::Random::randint(300, 800);
        SDL_Log("CatPet::walkAround: Walking to new target position X=%d", target_positionX_);
        setState(PetState::WALK);
    }
}

void CatPet::walkAround(float dt){

    int dx = target_positionX_ - posX_; // distance to target
    if(dx == 0){
        // reached
        setState(PetState::IDLE);
        return;
    }
    bool toRight = (dx > 0);    // direction
    
    // set move speed direction
    float baseSpeed = std::abs(moveSpeed_.x);
    moveSpeed_.x = toRight ? baseSpeed : -baseSpeed;

    // set animation flipping based on direction
    flipX_ = !toRight;

    // move pet
    int step = static_cast<int>(moveSpeed_.x * dt);
    // avoid stop when step is 0 (since there is a float to int conversion)
    if(step == 0){
        step = toRight ? 1 : -1;
    }

    int newX = posX_ + step;

    // check if overshoot
    if((toRight && newX >= target_positionX_) || (!toRight && newX <= target_positionX_)){
        // reached or overshoot
        posX_ = target_positionX_;
        setState(PetState::IDLE);
        SDL_Log("CatPet::walkAround: Reached target position X=%d", target_positionX_);
        return;
    }

    // updatePosition(dt);
    posX_ = newX;
}
#ifndef CATPET_H
#define CATPET_H

#include "core/desktoppet.h"

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

protected:
    virtual void setState(PetState state) override; // 设置状态
    virtual void handleEventClick(SDL_Event& event) override; // 处理点击事件

};

#endif // CATPET_H
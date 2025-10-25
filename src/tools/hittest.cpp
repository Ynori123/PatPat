#include "hittest.h"
#include "core/desktoppet.h"


extern "C" SDL_HitTestResult PetHitTestCallback(SDL_Window* window, const SDL_Point* point_area, void* data){
    
    SDL_Log("GeneralHitTestCallback called");

    DesktopPet* pet = (DesktopPet*)data;
    int petX, petY;
    pet->getPosition(petX, petY);
    SDL_Log("now petX = %d, petY = %d", petX, petY);
    int pw = pet->getWidth(pet->getViewScale());
    int ph = pet->getHeight(pet->getViewScale());
    SDL_Log("get pet width = %d, height = %d", pw, ph);

    // 缩放整个宠物区域
    // 由于注册方式的限制，建议在data中传入宽高时传入已经缩放过的宽高
    // 判断点是否在宠物区域内
    if(point_area->x >= petX && point_area->x <= petX + pw 
        && point_area->y >= petY && point_area->y <= petY + ph){
            return SDL_HITTEST_NORMAL; // 在宠物区域内，正常处理
        }
    else{
        return SDL_HITTEST_TRANSPARENT; // 不在宠物区域内，拖拽处理
    }
}

void GetHitTestRegion(int &x, int &y, int &w, int &h, int scaleX, int scaleY){
    w *= scaleX;
    h *= scaleY;
    SDL_Log("HitTest Region: x=%d, y=%d, w=%d, h=%d", x, y, w, h);
}

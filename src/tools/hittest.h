#pragma once 
#include <SDL3/SDL.h>
#include <vector>

// 自定义透明穿透
#ifndef SDL_HITTEST_TRANSPARENT
#define SDL_HITTEST_TRANSPARENT ((SDL_HitTestResult)739)
#endif


// 回调函数声明
extern "C" SDL_HitTestResult PetHitTestCallback(SDL_Window* window, const SDL_Point* point_area, void* data);

void GetHitTestRegion(int &x, int &y, int &w, int &h, int scaleX = 1, int scaleY = 1);


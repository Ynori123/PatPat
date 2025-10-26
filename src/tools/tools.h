#pragma once

#include <iostream>
#include <vector>
#include <SDL3/SDL.h>
#include <windows.h>


// 工具类
namespace tools{
    // math
    namespace math{

    }
    // UI
    namespace UI{

        // getters
        void getWindowSize(int& w, int& h);
        SDL_Point getClientMousePosition(HWND hwnd);

        // changers, which is real workers
    void ChangeWindowTransparent(HWND hwnd, bool isInArea, bool &transparentState);
    void ChangeTransparentState(HWND hwnd, SDL_Point point, SDL_Rect rect, bool &transparentState);  // 功能整合函数

        // Isfunctions, which is used for checking states
        bool IsPointInRect(SDL_Rect rect, SDL_Point point);

        // checkers, just for debugging
        bool CheckClickThrough(SDL_Window* window, bool is_transparent); // 检查窗口是否点击穿透
        bool CheckIsInAnyRects(std::vector<SDL_Rect>& rects, SDL_Point mouse_point, bool is_tansparent); // 检查鼠标是否在任意一个矩形内
    }

    // to do ...
}
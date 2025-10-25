#pragma once

#include "tools.h"


namespace tools{

    namespace UI{

        // getters

        // 获取当前屏幕大小
        void getWindowSize(int& w, int& h){
            w = GetSystemMetrics(SM_CXSCREEN);
            h = GetSystemMetrics(SM_CYSCREEN);
        }

        // 获取鼠标位置
        SDL_Point getClientMousePosition(HWND hwnd){
            POINT p;
            if(GetCursorPos(&p)){
                ScreenToClient(hwnd, &p);
            }else{
                SDL_Log("GetCursorPos failed");
            }
            return SDL_Point{p.x, p.y};
        }

        // setters

        // changers

        // 改变窗口样式，实现点击穿透
        void ChangeWindowTransparent(HWND hwnd, bool isInArea, bool transparentState){
            if(!hwnd){
                SDL_Log("hwnd is null");
                return;
            }

            // 点击穿透只有在“指定区域外 + 非穿透状态”时才改变（开启穿透状态）
            if(!isInArea){
                if(transparentState){
                    return;
                }else{
                    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
                    exStyle |= WS_EX_TRANSPARENT;
                    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);
                    transparentState = true;
                    SDL_Log("Win32: Now is transparent, mouse is not in area.");
                    return;
                }
            }else{
            // 非点击穿透只有在“指定区域内 + 穿透状态”时才改变（关闭穿透状态）
                if(!transparentState){
                    return;
                }else{
                    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
                    exStyle &= ~WS_EX_TRANSPARENT;
                    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);
                    transparentState = false;
                    SDL_Log("Win32: Now is not transparent, mouse is in area.");
                    return;
                }
            }

        }

        // 整合函数，根据鼠标位置和窗口状态，实现点击穿透
        void ChangeTransparentState(HWND hwnd, SDL_Point point, SDL_Rect rect, bool transparentState){
            bool isInArea = IsPointInRect(rect, point);
            ChangeWindowTransparent(hwnd, isInArea, transparentState);

            return;
        }

        // isFunctions

        // 判断点是否在矩形内
        bool IsPointInRect(SDL_Rect rect, SDL_Point point){
            return SDL_PointInRect(&point, &rect);
        }

        // checkers

        // Just for checking, not the real implementation
        bool CheckClickThrough(SDL_Window* window, bool is_transparent){
            if(!window){
                std::cout << "window is null" << std::endl;
                return false;
            }
            if(!is_transparent){
                return false;
            }
            return true;
        }

        bool CheckIsInAnyRects(std::vector<SDL_Rect>& rects, SDL_Point mouse_point, bool is_tansparent){
            
            std::cout << "Now checking point (" << mouse_point.x << ", " << mouse_point.y << ") in rects." << std::endl;

            if(!is_tansparent){
                return false;
            }

            for(auto& rect : rects){
                return SDL_PointInRect(&mouse_point, &rect);
            }

            return false;
        }

    }
}
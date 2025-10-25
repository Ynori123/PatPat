#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "../core/animation.h"  // needed for AnimationFrame

struct FrameRect {
    int x = 0, y = 0, w = 0, h = 0;
    int durationMS = 100; // ms
};

struct AnimationDesc {
    std::string name;
    std::string path;
    int frames = -1;
    int frameWidth = -1;
    int frameHeight = -1;
    int rows = -1, cols = -1;
    int fps = -1;
    bool loop = true;
    std::string layout = "row"; // "row" or "column"
    std::vector<FrameRect> rects;
};

struct Defaults {
    int frameWidth = 48;
    int frameHeight = 48;
    int fps = 8;
    bool loop = true;
    std::string layout = "row";
};

struct Manifest {
    int version = 1;
    std::string basePath;
    Defaults defaults;
    std::unordered_map<std::string, AnimationDesc> animations;  // name -> desc
};

// load manifest from json file
bool loadManifest(const std::string& jsonPath, Manifest& outManifest, std::string* outErr = nullptr);

// Normalizing manifest date, filling missing values with defults
void normalizeDesc(AnimationDesc& desc, const Defaults& defults);

// Create AnimationFrame form rects
std::vector<AnimationFrame> buildFramesFromRects(const AnimationDesc& d);

// Create AnimationFrame form Grid, needing texture width and height
std::vector<AnimationFrame> buildFramesFromGrid(const AnimationDesc& d, int texW, int texH);

// Simple texture loader
SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& fullpath);
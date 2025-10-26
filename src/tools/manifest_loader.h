#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "../core/animation.h"  // use shared types: AnimationFrame, AnimationDescription, Defaults, Manifest

// load manifest from json file
bool loadManifest(const std::string& jsonPath, Manifest& outManifest, std::string* outErr = nullptr);

// Normalizing manifest data, filling missing values with defaults
void normalizeDesc(AnimationDescription& desc, const Defaults& defaults);

// Create AnimationFrame from rects
std::vector<AnimationFrame> buildFramesFromRects(const AnimationDescription& d);

// Create AnimationFrame from Grid, needing texture width and height
std::vector<AnimationFrame> buildFramesFromGrid(const AnimationDescription& d, int texW, int texH);

// Simple texture loader
SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& fullpath);
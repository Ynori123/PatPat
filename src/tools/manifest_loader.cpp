#include "manifest_loader.h"
#include "minijson.h"

#include <fstream>
#include <sstream>

static std::string readFileText(const std::string& path){
    std::ifstream ifs(path, std::ios::binary);
    if(!ifs.is_open())  return "";
    std::ostringstream oss;
    oss << ifs.rdbuf();
    return oss.str();
}

bool loadManifest(const std::string& jsonPath, Manifest& out, std::string* outErr){
    out = Manifest{};
    std::string text = readFileText(jsonPath);
    if(text.empty()){
        if(outErr) *outErr = "Failed to read file: " + jsonPath;
        return false;
    }
    auto res = minijson::parse(text);
    if(!res.ok || !res.root.isObject()){
        if(outErr) *outErr = "Failed to parse JSON: " + res.error;
        return false;
    }
    const auto& root = res.root;

    // version
    if(auto v = root.get("version"); v && v->isNumber()){
        out.version = static_cast<int>(v->n);
    }

    // basePath
    out.basePath = root.getString("basePath", "");
    if(!out.basePath.empty()){
        char back = out.basePath.back();
        if(back != '/' && back != '\\'){
            out.basePath.push_back('/');
        }
    }

    // defults
    if (auto d = root.getObject("defaults")) {
        if(auto v = d->find("frameWidth"); v != d->end()) {
            out.defaults.frameWidth = static_cast<int>(v->second.n);
        }
        if(auto v = d->find("frameHeight"); v != d->end()) {
            out.defaults.frameHeight = static_cast<int>(v->second.n);
        }
        if(auto v = d->find("fps"); v != d->end()) {
            out.defaults.fps = static_cast<int>(v->second.n);
        }
        if(auto v = d->find("loop"); v != d->end()) {
            out.defaults.loop = v->second.b;
        }
        if(auto v = d->find("layout"); v != d->end() && v->second.isString()) {
            out.defaults.layout = v->second.s;
        }
    }

    // animations
    if(auto aims = root.getObject("animations")){
        for(const auto& kv : *aims){
            const std::string& name = kv.first;
            const minijson::Value& val = kv.second;
            if(!val.isObject()) continue;

            AnimationDesc desc;
            desc.name = name;
            desc.path = val.getString("path", "");
            desc.frames = val.getInt("frames", -1);
            desc.frameWidth = val.getInt("frameWidth", -1);
            desc.frameHeight = val.getInt("frameHeight", -1);
            desc.rows = val.getInt("rows", -1);
            desc.cols = val.getInt("cols", -1);
            desc.fps = val.getInt("fps", -1);
            desc.loop = val.getBool("loop", true);
            desc.layout = val.getString("layout", "row");
        

            if(auto rects = val.getArray("rects")){
                for(const auto& item : *rects){
                    if(!item.isObject()) continue;
                    FrameRect fr;
                    fr.x = item.getInt("x", 0);
                    fr.y = item.getInt("y", 0);
                    fr.w = item.getInt("w", 0);
                    fr.h = item.getInt("h", 0);
                    fr.durationMS = item.getInt("durationMS", 100);
                    desc.rects.push_back(fr);
                }
            }

            out.animations.emplace(desc.name, std::move(desc));
        }
    }

    return !out.animations.empty();
}

void NormalizeDesc(AnimationDesc& d, const Defaults& def){
    if(d.fps <= 0) d.fps = def.fps;
    if(d.frameWidth <= 0) d.frameWidth = def.frameWidth;
    if(d.frameHeight <= 0) d.frameHeight = def.frameHeight;
    if(d.layout.empty()) d.layout = def.layout;
    // loop default to true
}

std::vector<AnimationFrame> buildFramesFromRects(const AnimationDesc& d){
    std::vector<AnimationFrame> frames;
    frames.reserve(d.rects.size());
    int per = 1000 / (d.fps > 0 ? d.fps : 8);
    for(const auto& r : d.rects){
        AnimationFrame f;
        f.souceRect = SDL_Rect{r.x, r.y, r.w, r.h};
        f.duration = (r.durationMS > 0) ? r.durationMS : per;
        frames.push_back(f);
    }
    return frames;
}

std::vector<AnimationFrame> buildFramesFromGrid(const AnimationDesc& d, int texW, int texH){
    std::vector<AnimationFrame> frames;
    if(d.frameWidth <= 0 || d.frameWidth <= 0) return frames;

    int per = 1000 / (d.fps > 0 ? d.fps : 8);
    int count = d.frames;

    if(count <= 0){
        if(d.layout == "grid" && d.rows > 0 && d.cols > 0){
            count = d.rows * d.cols;
        } else{
            // sigle row, just use texture width/height
            count = d.frameWidth > 0 ? (texW / d.frameWidth) : 1;
        }
    }

    frames.reserve(count);
    if(d.layout == "grid" && d.rows > 0 && d.cols > 0){
        int idx = 0;
        for(int r = 0; r < d.rows && idx < count; r++){
            for(int c = 0; c < d.cols && idx < count; c++){
                AnimationFrame f;
                f.souceRect = SDL_Rect{c * d.frameWidth, r * d.frameHeight, d.frameWidth, d.frameHeight};
                f.duration = per;
                frames.push_back(f);
                idx++;
            }
        }
    } else{
        for(int i = 0; i < count; i++){
            AnimationFrame f;
            f.souceRect = SDL_Rect{i * d.frameWidth, 0, d.frameWidth, d.frameHeight};
            f.duration = per;
            frames.push_back(f);
        }
    }

    return frames;
}

SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& fullpath){
    SDL_Texture* t = IMG_LoadTexture(renderer, fullpath.c_str());
    if(!t){
        SDL_Log("Failed to load texture: %s, error: %s", fullpath.c_str(), SDL_GetError());
    }
    return t;
}
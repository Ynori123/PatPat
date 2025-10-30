#pragma once

#include <random>
#include <chrono>
#include <vector>
#include <stdexcept>
#include <numeric>
#include <algorithm>

inline std::mt19937& rng() {
    static thread_local std::mt19937 eng{ std::random_device{}() };
    return eng;
}

namespace tools
{

class Random{
public:

    // set seed outside for reproducibility
    static void setSeed(uint64_t seed){
        uint32_t low = static_cast<uint32_t>(seed);
        uint32_t high = static_cast<uint32_t>(seed >> 32);
        std::seed_seq seq{low, high};
        rng().seed(seq);
    }

    // int [a, b]
    static int randint(int min, int max){
        if(min > max){ std::swap(min, max);}
        std::uniform_int_distribution<int> dist(min, max);
        return dist(rng());
    }

    // float [min., max.)
    static float randfloat(float min, float max){
        if(min > max){std::swap(min, max);}
        std::uniform_real_distribution<float> dist(min, max);
        return dist(rng());
    }

    // return true with probability p [0, 1]
    static bool chance(double p){
        if(p <= 0.0) return false;
        if(p >= 1.0) return true;
        std::bernoulli_distribution dist(p);    // 伯努利分布，随机返回 true/false
        return dist(rng());
    }
};


}


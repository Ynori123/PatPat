#include "Timer.h"
#include <algorithm>

void Timer::start()
{
    running_ = true;
    paused_ = false;
    accumulated_ = Seconds{0.0f};
    startPoint_ = now();
    lastTickPoint_ = startPoint_;
    lapPoint_ = startPoint_;
    lastUpdatePoint_ = startPoint_;
    intervalAccumSec_ = 0.0f;
}

void Timer::stop()
{
    running_ = false;
    paused_ = false;
    accumulated_ = Seconds{0.0f};
    // clear time points are not strictly necessary
    intervalAccumSec_ = 0.0f;
}

void Timer::reset()
{
    accumulated_ = Seconds{0.0f};
    intervalAccumSec_ = 0.0f;
    auto tnow = now();
    if(running_ && !paused_){
        startPoint_ = tnow;
        lastTickPoint_ = tnow;
        lapPoint_ = tnow;
        lastUpdatePoint_ = tnow;
    }
    // if paused or stopped, keep time points unchanged
}

void Timer::pause()
{
    if(running_ && !paused_){
        paused_ = true;
        pausePoint_ = now();
        accumulated_ += std::chrono::duration_cast<Seconds>(pausePoint_ - startPoint_);
    }
}

void Timer::resume()
{
    if(running_ && paused_){
        paused_ = false;
        startPoint_ = now();
        lastTickPoint_ = startPoint_;
        lapPoint_ = startPoint_;
        lastUpdatePoint_ = startPoint_;
    }
}

float Timer::getElapsedTime() const
{
    auto tnow = now();
    return currentElapsedUnsafe(tnow);
}

float Timer::currentElapsedUnsafe(TimePoint tnow) const
{
    if(!running_){
        return accumulated_.count();
    }
    if(paused_){
        return accumulated_.count();
    }
    // otherwise now is running
    Seconds cur = accumulated_ + std::chrono::duration_cast<Seconds>(tnow - startPoint_);
    return cur.count();
}

float Timer::tick()
{
    if(!running_ || paused_){
        return 0.0f;
    }
    auto tnow = now();
    float dt = std::chrono::duration_cast<Seconds>(tnow - lastTickPoint_).count();
    lastTickPoint_ = tnow;
    return std::max(0.0f, dt);
}

float Timer::lap()
{
    auto tnow = now();
    float sec = std::chrono::duration_cast<Seconds>(tnow - lapPoint_).count();
    lapPoint_ = tnow;
    return std::max(0.0f, sec);
}

void Timer::setInterval(float seconds, bool repeat)
{
    intervalSec_ = seconds;
    repeat_ = repeat;
    intervalAccumSec_ = 0.0f;
    lastUpdatePoint_ = now();
}

void Timer::clearInterval()
{
    intervalSec_ = 0.0f;
    intervalAccumSec_ = 0.0f;
}

// drive
bool Timer::update(float dt)
{
    if(!running_ || paused_ || intervalSec_ <= 0.0f){
        return false;
    }

    intervalAccumSec_ += std::max(0.0f, dt);
    bool fired = false;

    // when dt is large enough to cover multiple intervals, 
    // which is to say, Timer will be fired many times
    // we allow that, but only return true once
    while(intervalAccumSec_ + 1e-7f >= intervalSec_){
        intervalAccumSec_ -= intervalSec_;
        fired = true;
        if(!repeat_){
            // no repeat, stop after first fire
            running_ = false;
            intervalAccumSec_ = 0.0f;
            break;
        }
    }
    return fired;
}

bool Timer::update()
{
    if(!running_ || paused_ || intervalSec_ <= 0.0f){
        return false;
    }

    auto tnow = now();
    float dt = std::chrono::duration_cast<Seconds>(tnow - lastUpdatePoint_).count();
    lastUpdatePoint_ = tnow;
    return update(dt);
}

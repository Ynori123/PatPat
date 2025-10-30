#pragma once

#include <chrono>


class Timer {
public:
    Timer() = default;
    ~Timer() = default;

    // base controlers
    void start();   // start or restart the timer
    void stop();    // stop and reset time
    void reset();   // reset timer to 0 but keep running state
    void pause();   // pause and freeze time
    void resume();  // 恢复计时

    // check time
    float getElapsedTime() const; // 返回以秒为单位的时间
    float tick();            // 返回自上次 tick() 以来的时间，并更新 tick 点
    float lap();             // 返回自上次 lap() 以来的时间，并更新 lap 点    

    // states
    bool isRunning() const {return running_;}
    bool isPaused() const {return paused_;}

    // interval mode
    void setInterval(float seconds, bool repeat = true);    // 开启间隔定时器模式
    void clearInterval(); // 关闭间隔定时器模式
    float getInterval() const {return intervalSec_;}
    bool getRepeat() const {return repeat_;}

    // how to drive, use bool can indicate whether interval is reached
    bool update(float dt); // based on game external loop
    bool update();        // based on internal time calculation

private:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using Seconds = std::chrono::duration<float>;

    // tools internal
    TimePoint now() const {return Clock::now();}
    float currentElapsedUnsafe(TimePoint tnow) const;   // 不检查状态，计算从 start 到 tnow 的经过时间

    // run and data
    bool running_ = false;
    bool paused_ = false;
    Seconds accumulated_{0.0f}; // 累计时间
    TimePoint startPoint_{}; // 计时开始时间点
    TimePoint pausePoint_{}; // 暂停时间点

    // tick or lap
    TimePoint lastTickPoint_{}; // reference point for tick()
    TimePoint lapPoint_{};  // reference point for lap()

    // interval mode
    float intervalSec_ = 0.0f; // interval in seconds, <= 0 means disabled
    bool repeat_ = true; // whether to repeat after each interval
    float intervalAccumSec_ = 0.0f; // accumulated time for interval checking
    TimePoint lastUpdatePoint_{}; // for update() without dt, used to calculate dt
};

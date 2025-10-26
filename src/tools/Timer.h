#ifndef TIMER_H
#define TIMER_H

class Timer {
public:
    Timer();
    ~Timer();

    void start();
    void stop();
    void reset();
    float getElapsedTime() const; // 返回以秒为单位的时间
};


#endif // TIMER_H
#pragma once
#include <functional>

class Timer {
public:
    Timer();
    Timer(float interval, bool isRepeating = true, float duration = -1.0f,
        std::function<void()> onUpdate = nullptr, float currentTime = 0.0f);

    bool shouldTrigger(float currentTime);
    void trigger(float currentTime);
    void pause();
    void resume(float currentTime);

    float getNextTrigger() const;
    bool isActive() const;

private:
    float updateInterval;  // Time interval between triggers
    float nextTrigger;     // Absolute time for the next trigger
    float startTime;       // Time when the timer starts
    float endTime;         // Time when the timer should stop (-1 for infinite)

    bool repeat;           // Whether the timer repeats
    bool active;           // Whether the timer is active
    std::function<void()> onUpdateCallback; // Callback function
};

struct TimerComparison {
    bool operator()(const Timer& a, const Timer& b) const;
};

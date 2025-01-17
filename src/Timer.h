#pragma once
#include <functional>
#include <GLFW/glfw3.h>

class Timer {
public:
    Timer();
    Timer(float interval, std::function<void()> onUpdate = nullptr, bool isRepeating = true,
        float duration = -1.0f, float currentTime = glfwGetTime());

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

#include "Timer.h"

Timer::Timer()
    : updateInterval(0.0f), nextTrigger(0.0f),
    startTime(0.0f), endTime(-1.0f),
    repeat(false), active(false),
    onUpdateCallback(nullptr) {
}

Timer::Timer(float interval, std::function<void()> onUpdate, bool isRepeating, float duration, float currentTime)
    : updateInterval(interval),
    startTime(currentTime),
    repeat(isRepeating),
    active(true),
    onUpdateCallback(onUpdate)
{
    nextTrigger = startTime + updateInterval;
    if (duration >= 0.0f)
        endTime = startTime + duration;
    else
        endTime = -1.0f;
}


bool Timer::shouldTrigger(float currentTime) {
    if (!active)
        return false;

    // Check if the timer has expired
    if (endTime >= 0.0f && currentTime >= endTime) {
        active = false;
        return false;
    }
    return currentTime >= nextTrigger;
}

// Callback then update next trigger
void Timer::trigger(float currentTime) {
    if (!active) return;

   
    if (onUpdateCallback) onUpdateCallback();

    if (repeat) {
        nextTrigger += updateInterval; // Move to the next interval
    }
    else {
        active = false; // Deactivate non-repeating timers
    }
}

void Timer::pause() {
    active = false;
}

void Timer::resume(float currentTime) {
    if (endTime >= 0.0f && currentTime >= endTime) {
        active = false; // Don't resume if already expired
        return;
    }
    active = true;
    nextTrigger = currentTime + updateInterval;
}

float Timer::getNextTrigger() const { return nextTrigger; }
bool Timer::isActive() const { return active; }

// Return earlier triggers
bool TimerComparison::operator()(const Timer& a, const Timer& b) const {
    return a.getNextTrigger() > b.getNextTrigger();
}

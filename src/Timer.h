class Timer {
public:
    Timer(float interval, bool isRepeating = true, float duration = -1.0f, std::function<void()> onUpdate = nullptr)
        : updateInterval(interval),
        totalDuration(duration),
        timeSinceLastUpdate(0.0f),
        timeElapsed(0.0f),
        repeat(isRepeating),
        paused(false),
        onUpdateCallback(onUpdate) {
    }

    void update(float deltaTime) {
        if (paused) return;

        timeSinceLastUpdate += deltaTime;
        timeElapsed += deltaTime;

        if (totalDuration > 0.0f && timeElapsed >= totalDuration) {
            paused = true; // Stops the timer
            return;
        }

        if (timeSinceLastUpdate >= updateInterval) {
            if (onUpdateCallback) onUpdateCallback();
            if (repeat) timeSinceLastUpdate = 0.0f;
            else paused = true; // Stops a non-repeating timer
        }
    }

    void pause() { paused = true; }
    void resume() { paused = false; }
    float getElapsedTime() const { return timeElapsed; }

private:
    float updateInterval;
    float totalDuration;
    float timeSinceLastUpdate;
    float timeElapsed;
    bool repeat;
    bool paused;
    std::function<void()> onUpdateCallback;
};

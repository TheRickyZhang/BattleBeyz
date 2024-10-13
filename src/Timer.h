class Timer {
public:
    Timer(float interval, bool isRepeating = true, float duration = -1.0f)
        : updateInterval(interval),
        totalDuration(duration),
        timeSinceLastUpdate(0.0f),
        timeElapsed(0.0f),
        repeat(isRepeating),
        paused(false) {}

    bool shouldUpdate(float deltaTime) {
        if (paused) return false;

        timeSinceLastUpdate += deltaTime;
        timeElapsed += deltaTime;

        if (totalDuration > 0.0f && timeElapsed >= totalDuration) {
            return false;
        }

        if (timeSinceLastUpdate >= updateInterval) {
            if (repeat) timeSinceLastUpdate = 0.0f;
            return true;
        }
        return false;
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
};

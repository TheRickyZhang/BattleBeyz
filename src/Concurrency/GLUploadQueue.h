////////////////////////////////////////////////////////////////////////////////
// GLUploadQueue.h -- Deferred main-thread GL work -- rz -- 2026-08-25
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <functional>
#include <mutex>
#include <queue>

/*
* There is exactly one GL context and it is current on the main thread only, so every
* OpenGL call in the project happens either directly in init/draw or through here.
*
* Workers enqueue an upload closure holding the data they produced; drain() runs those
* closures on the main thread once per frame, under a time budget so a burst of large
* meshes spreads over several frames instead of stalling one.
*/
class GLUploadQueue {
public:
    // Safe from any thread
    void enqueue(std::function<void()> upload);

    // Main thread only. budgetMs <= 0 drains everything regardless of cost.
    void drain(double budgetMs = 4.0);

    size_t pending() const;

private:
    mutable std::mutex mutex;
    std::queue<std::function<void()>> uploads;
};

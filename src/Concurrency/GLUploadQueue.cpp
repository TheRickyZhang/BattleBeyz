////////////////////////////////////////////////////////////////////////////////
// GLUploadQueue.cpp -- Deferred main-thread GL work -- rz -- 2026-08-25
////////////////////////////////////////////////////////////////////////////////

#include <chrono>
#include <utility>

#include "GLUploadQueue.h"

void GLUploadQueue::enqueue(std::function<void()> upload) {
    if (!upload) return;
    std::lock_guard<std::mutex> lock(mutex);
    uploads.push(std::move(upload));
}

void GLUploadQueue::drain(double budgetMs) {
    const auto start = std::chrono::steady_clock::now();

    while (true) {
        std::function<void()> upload;
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (uploads.empty()) return;
            upload = std::move(uploads.front());
            uploads.pop();
        }

        // Run outside the lock so producers are not blocked by a slow upload
        upload();

        if (budgetMs > 0.0) {
            std::chrono::duration<double, std::milli> elapsed = std::chrono::steady_clock::now() - start;
            if (elapsed.count() >= budgetMs) return;
        }
    }
}

size_t GLUploadQueue::pending() const {
    std::lock_guard<std::mutex> lock(mutex);
    return uploads.size();
}

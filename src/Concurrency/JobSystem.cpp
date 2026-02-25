////////////////////////////////////////////////////////////////////////////////
// JobSystem.cpp -- Fixed worker pool for CPU-side work -- rz -- 2026-08-25
////////////////////////////////////////////////////////////////////////////////

#include "JobSystem.h"

JobSystem::JobSystem(unsigned int requestedWorkers) {
    unsigned int workerCount = requestedWorkers;
    if (workerCount == 0) {
        unsigned int cores = std::thread::hardware_concurrency();
        workerCount = cores > 1 ? cores - 1 : 1;
    }

    workers.reserve(workerCount);
    for (unsigned int i = 0; i < workerCount; ++i) {
        workers.emplace_back([this](std::stop_token stopToken) { workerLoop(std::move(stopToken)); });
    }
}

JobSystem::~JobSystem() {
    shutdown();
}

void JobSystem::shutdown() {
    // The stop_token overload of wait() registers a callback that wakes the workers,
    // so requesting stop is enough to break them out of the queue wait.
    for (std::jthread& worker : workers) {
        worker.request_stop();
    }
    for (std::jthread& worker : workers) {
        if (worker.joinable()) worker.join();
    }
    workers.clear();
}

void JobSystem::workerLoop(std::stop_token stopToken) {
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            jobAvailable.wait(lock, stopToken, [this] { return !jobs.empty(); });

            // Predicate false means stop was requested; drop any remaining jobs
            if (jobs.empty()) return;

            job = std::move(jobs.front());
            jobs.pop();
        }
        job();
    }
}

size_t JobSystem::pending() const {
    std::lock_guard<std::mutex> lock(queueMutex);
    return jobs.size();
}

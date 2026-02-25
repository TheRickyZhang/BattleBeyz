////////////////////////////////////////////////////////////////////////////////
// JobSystem.h -- Fixed worker pool for CPU-side work -- rz -- 2026-08-25
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stop_token>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

/*
* A plain worker pool for independent CPU work: parsing .obj files, decoding images.
* Deliberately not a task graph -- nothing here has dependencies between jobs.
*
* Jobs must not call OpenGL. Produce plain data (MeshData, ImageData) and hand the
* upload to GLUploadQueue, which runs it on the thread owning the GL context.
*/
class JobSystem {
public:
    // 0 workers means hardware_concurrency() - 1, leaving a core for the main thread
    explicit JobSystem(unsigned int requestedWorkers = 0);
    ~JobSystem();

    JobSystem(const JobSystem&) = delete;
    JobSystem& operator=(const JobSystem&) = delete;

    template <typename F>
    auto submit(F&& fn) -> std::future<std::invoke_result_t<F>> {
        using Result = std::invoke_result_t<F>;

        // packaged_task is move-only and std::function is not, so share it in
        auto task = std::make_shared<std::packaged_task<Result()>>(std::forward<F>(fn));
        std::future<Result> result = task->get_future();
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            jobs.emplace([task]() { (*task)(); });
        }
        jobAvailable.notify_one();
        return result;
    }

    // Requests stop and joins. Idempotent; the destructor calls it too. Lets the owner
    // retire the workers before destroying anything their jobs might reference.
    void shutdown();

    size_t workerCount() const { return workers.size(); }
    size_t pending() const;

private:
    void workerLoop(std::stop_token stopToken);

    mutable std::mutex queueMutex;
    std::condition_variable_any jobAvailable;
    std::queue<std::function<void()>> jobs;

    // Declared last so the threads are joined before the queue they read from dies
    std::vector<std::jthread> workers;
};

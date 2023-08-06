#pragma once

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/std.h>
#include <functional>
#include <future>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>

#ifdef __clang__
#    error todo: implement this
#endif

namespace Util {

template<class T> class MutexProtected {
public:
    template<class... Args>
        requires(std::is_constructible_v<T, Args...>)
    MutexProtected(Args&&... args)
        : m_t(std::forward<Args>(args)...) { }

    template<class Callback> auto with(Callback&& callback) {
        std::unique_lock lock { m_mutex };
        return callback(m_t);
    }

private:
    T m_t;
    std::mutex m_mutex;
};

template<class T> class ThreadPool {
public:
    using Result = T;
    using TaskCallback = std::function<Result()>;

    explicit ThreadPool(size_t threads = std::thread::hardware_concurrency()) {
        for (size_t s = 0; s < threads; s++) {
            m_threads.push_back(std::jthread([this]() {
                while (true) {
                    // fmt::print("{} {}: Waiting for new tasks\n", std::this_thread::get_id(), dump());
                    // Wait for new tasks
                    std::mutex mutex;
                    std::unique_lock lock { mutex };
                    m_new_task_pushed.wait(lock, [this]() { return m_stopped || m_new_task_count > 0; });

                    // fmt::print("{} {}: Got new task (hopefully!)\n", std::this_thread::get_id(), dump());
                    // Take first task from the queue and run it
                    auto task = m_tasks.with([this](auto& queue) -> std::optional<std::function<void()>> {
                        if (queue.empty()) {
                            return {};
                        }
                        auto task = std::move(queue.front());
                        queue.pop();
                        m_new_task_count--;
                        if (m_new_task_count == 0) {
                            m_new_task_pushed.notify_all();
                        }
                        return task;
                    });

                    if (task) {
                        (*task)();
                    }

                    if (m_stopped && m_new_task_count == 0) {
                        // fmt::print("{} {}: Finished because stopped!\n", std::this_thread::get_id(), dump());
                        return;
                    }
                }
            }));
        }
    }

    ~ThreadPool() {
        stop();
        // This is explicit because otherwise task list is destroyed
        // before workers finish their work which would cause UAF
        for (auto& thread : m_threads) {
            thread.join();
        }
    }

    // Tell the pool that no new tasks will be
    void stop() {
        m_stopped = true;
        m_new_task_pushed.notify_all();
        // fmt::print("Stopping threadpool\n");
    }

    std::future<Result> run(TaskCallback&& callback) {
        auto promise = std::make_shared<std::promise<Result>>();
        // clang-format off
        m_tasks.with([this, callback = std::move(callback), promise](auto& queue) {
            queue.push([callback = std::move(callback), promise]() {
                if constexpr (std::is_same_v<std::invoke_result_t<decltype(callback)>, void>) {
                    callback();
                    promise->set_value();
                } else {
                    promise->set_value(callback());
                }
            });
            m_new_task_count++;
        });
        // clang-format on
        m_new_task_pushed.notify_all();
        return promise->get_future();
    }

    std::string dump() {
        return "";
        // return m_tasks.with([this](auto& q) {
        //     return fmt::format("[Threadpool threads={} tasks={} stopped={} new_tasks={}]", m_threads.size(), q.size(), m_stopped.load(),
        //         m_new_task_count.load());
        // });
    }

private:
    std::vector<std::jthread> m_threads;
    MutexProtected<std::queue<std::function<void()>>> m_tasks;

    std::condition_variable m_new_task_pushed;

    std::atomic<bool> m_stopped = false;
    std::atomic<int> m_new_task_count = 0;
};

}

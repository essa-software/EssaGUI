#include <EssaUtil/Testing.hpp>

#include <EssaUtil/Threading/ThreadPool.hpp>

using namespace std::chrono_literals;

constexpr size_t TotalDuration = 1'000'000'000;

void run(size_t tasks) {
    Util::ThreadPool<float> pool;

    std::vector<std::future<float>> futures;
    futures.reserve(tasks);
    for (size_t s = 0; s < tasks; s++) {
        futures.push_back(pool.run([tasks]() {
            float fac = 1;
            for (size_t i = 2; i < TotalDuration / tasks; i++) {
                fac += 1;
            }
            return fac;
        }));
    }
    // fmt::print("Waiting on {} futures\n", futures.size());
    while (true) {
        bool any_unfinished = false;
        size_t idx = 0;
        for (auto& fut : futures) {
            // fmt::print("Waiting for {}\n", idx);
            auto status = fut.wait_for(100ms);
            if (status == std::future_status::ready) {
                // fmt::print("Future {} finished\n", idx);
            }
            else {
                // fmt::print("Future {} UNfinished yet!\n", idx);
                // pool.dump();
                any_unfinished = true;
            }
            idx++;
        }
        if (!any_unfinished) {
            // fmt::print("Finished everything yay!!!\n");
            break;
        }
    }
}

BENCHMARK(threadpool_10) { run(10); }
BENCHMARK(threadpool_100) { run(100); }
BENCHMARK(threadpool_1000) { run(1000); }
BENCHMARK(threadpool_10000) { run(10000); }
BENCHMARK(threadpool_100000) { run(100000); }

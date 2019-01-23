// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include <ao/core/threading/thread_pool.h>
#include <gtest/gtest.h>

#define FUTURE_SIZE 5

using namespace std::chrono_literals;

namespace ao::test {
    TEST(ThreadPool, DefaultConstructor) {
        // 'Mute' logger
        core::Logger::Init();
        core::Logger::SetMinLevel(core::Logger::Level::fatal);

        core::ThreadPool p;

        ASSERT_FALSE(p.isRunning());
        ASSERT_EQ(0, p.size());
    }

    TEST(ThreadPool, Constructor) {
        core::ThreadPool p(2);

        std::this_thread::sleep_for(10ms);
        ASSERT_TRUE(p.isRunning());
        ASSERT_EQ(2, p.size());
    }

    TEST(ThreadPool, SimpleEnqueue) {
        core::ThreadPool p(2);

        auto result = p.enqueue<std::thread::id>([&]() { return std::make_shared<std::thread::id>(std::this_thread::get_id()); })->wait();

        ASSERT_NE(std::this_thread::get_id(), *result);
    }

    TEST(ThreadPool, MultipleEnqueue) {
        std::vector<std::shared_ptr<core::Future<int>>> futures;
        core::ThreadPool p(FUTURE_SIZE);

        // Enqueue tasks
        for (size_t i = 0; i < FUTURE_SIZE; i++) {
            futures.push_back(p.enqueue<int>([i]() { return std::make_shared<int>(static_cast<int>(i)); }));
        }

        // Wait tasks
        auto values = core::ThreadPool::All<int>(futures)->wait();

        // Check content
        ASSERT_EQ(FUTURE_SIZE, values->size());
        for (size_t i = 0; i < FUTURE_SIZE; i++) {
            ASSERT_NE(std::find_if(values->begin(), values->end(), [i](int& integer) { return i == integer; }), values->end());
        }
    }

    TEST(ThreadPool, MultipleEnqueueWithSequentialProcessing) {
        std::vector<std::shared_ptr<core::Future<int>>> futures;
        core::ThreadPool p(1);

        // Enqueue tasks
        for (size_t i = 0; i < FUTURE_SIZE; i++) {
            futures.push_back(p.enqueue<int>([i]() { return std::make_shared<int>(static_cast<int>(i)); }));
        }

        // Wait tasks
        auto values = core::ThreadPool::All<int>(futures)->wait();

        // Check content
        ASSERT_EQ(FUTURE_SIZE, values->size());
        for (size_t i = 0; i < FUTURE_SIZE; i++) {
            ASSERT_EQ(i, (*values)[i]);
        }
    }

    TEST(ThreadPool, Kill) {
        core::ThreadPool p(1);
        auto duration = 10ms;

        // Enqueue task
        auto start = std::chrono::system_clock::now();
        p.enqueue<int>([&]() {
            std::this_thread::sleep_for(duration);

            return std::make_shared<int>(10);
        });

        // Let worker takes task
        std::this_thread::sleep_for(duration / 5);

        // Kill
        p.kill();

        // Assert task end
        ASSERT_GE(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count(), duration.count());
        ASSERT_FALSE(p.isRunning());
    }

    TEST(ThreadPool, ConcurrentEnqueue) {
        std::vector<std::shared_ptr<core::Future<int>>> futures;
        std::array<std::thread, FUTURE_SIZE> threads;
        std::mutex futures_mutex;
        core::ThreadPool p(FUTURE_SIZE / 2);

        // Enqueue tasks
        for (size_t i = 0; i < FUTURE_SIZE; i++) {
            threads[i] = std::thread([&]() {
                futures_mutex.lock();
                futures.push_back(p.enqueue<int>([]() { return std::make_shared<int>(10); }));
                futures_mutex.unlock();
            });
        }

        // Join threads
        for (size_t i = 0; i < FUTURE_SIZE; i++) {
            threads[i].join();
        }

        // Wait tasks
        auto values = core::ThreadPool::All<int>(futures)->wait();

        // Assert size
        ASSERT_EQ(FUTURE_SIZE, values->size());
    }

    TEST(ThreadPool, AllThen) {
        std::vector<std::shared_ptr<core::Future<int>>> futures;
        core::ThreadPool p(FUTURE_SIZE);
        std::atomic_int called = 0;

        // Create tasks
        for (size_t i = 0; i < FUTURE_SIZE; i++) {
            futures.push_back(p.enqueue<int>([]() { return std::make_shared<int>(10); }));
        }

        // Define then
        core::ThreadPool::Then<int>(futures, [&](std::shared_ptr<int>) { called++; });

        // Wait
        for (auto& future : futures) {
            future->wait();
        }

        // Assert value
        ASSERT_EQ(FUTURE_SIZE, called);
    }
}  // namespace ao::test

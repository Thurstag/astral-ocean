// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include <chrono>

#include <gtest/gtest.h>
#include <ao/core/threading/future.hpp>

#include "../helpers/tests.h"

#define FUTURE_SIZE 5
#define ARRAY_SIZE 2

namespace ao::test {
    TEST(Future, DefaultState) {
        ASSERT_EQ(core::FutureState::kWaiting, core::Future<int>().state());
    }

    TEST(Future, Then) {
        auto _int = std::make_shared<int>(10);
        bool called = false;
        core::Future<int> f;

        // Define callback
        f.then([&](std::shared_ptr<int> integer) {
            ASSERT_EQ(integer.get(), _int.get());

            called = true;
        });

        // Notify
        f.notify(_int);

        // Assert call
        ASSERT_TRUE(called);
        ASSERT_EQ(core::FutureState::kFinished, f.state());
    }

    TEST(Future, ThenAfter) {
        auto _int = std::make_shared<int>(10);
        bool called = false;
        core::Future<int> f;

        // Notify
        f.notify(_int);

        // Define callback
        f.then([&](std::shared_ptr<int> integer) {
            ASSERT_EQ(integer.get(), _int.get());

            called = true;
        });

        // Assert call
        ASSERT_TRUE(called);
        ASSERT_EQ(core::FutureState::kFinished, f.state());
    }

    TEST(Future, AllThen) {
        std::vector<core::Future<int>> futures(FUTURE_SIZE);
        std::array<std::thread, FUTURE_SIZE> threads;
        std::atomic_int called = 0;

        // Define then
        core::Future<int>::Then(futures, [&](std::shared_ptr<int>) { called++; });

        // Notify futures
        for (size_t i = 0; i < FUTURE_SIZE; i++) {
            threads[i] = std::thread([&futures, i]() { futures[i].notify(std::make_shared<int>(10)); });
        }

        // Join threads
        for (size_t i = 0; i < FUTURE_SIZE; i++) {
            threads[i].join();
        }

        ASSERT_EQ(FUTURE_SIZE, called);
    }

    TEST(Future, All) {
        std::vector<core::Future<int>> futures(FUTURE_SIZE);
        std::array<std::thread, FUTURE_SIZE> threads;
        std::array<int, FUTURE_SIZE> integers;

        // Create array of integers
        for (size_t i = 0; i < FUTURE_SIZE; i++) {
            integers[i] = static_cast<int>(i + 2);

            threads[i] = std::thread([&](size_t i, size_t j) { futures[i].notify(std::make_shared<int>(static_cast<int>(j))); }, i, integers[i]);
        }

        // Wait
        auto values = core::Future<int>::All(futures)->wait();

        // Assert array
        ASSERT_EQ(FUTURE_SIZE, values->size());
        for (auto& value : *values) {
            ASSERT_NE(std::find_if(values->begin(), values->end(), [value](int& integer) { return value == integer; }), values->end());
        }

        // Join threads
        for (size_t i = 0; i < FUTURE_SIZE; i++) {
            threads[i].join();
        }
    }

    TEST(Future, Wait) {
        auto _int = std::make_shared<int>(10);
        core::Future<int> f;

        std::thread t([&]() {
            using namespace std::chrono_literals;

            // Sleep
            std::this_thread::sleep_for(2ms);

            // Notify
            f.notify(_int);
        });

        // Wait
        ASSERT_EQ(_int.get(), f.wait().get());

        // Join thread
        t.join();
    }

    TEST(Future, ConcurrentWait) {
        std::array<std::thread, ARRAY_SIZE> threads;
        auto _int = std::make_shared<int>(10);
        core::Future<int> f;

        // Start threads
        for (size_t i = 0; i < ARRAY_SIZE; i++) {
            threads[i] = std::thread([&]() { ASSERT_EQ(_int.get(), f.wait().get()); });
        }

        // Notify
        f.notify(_int);

        // Join threads
        for (size_t i = 0; i < ARRAY_SIZE; i++) {
            threads[i].join();
        }
    }

    TEST(Future, AsyncNotify) {
        auto id = std::this_thread::get_id();
        std::condition_variable cv_called;
        bool called = false;
        core::Future<int> f;

        f.then([&](std::shared_ptr<int>) {
            ASSERT_NE(std::this_thread::get_id(), id);
            called = true;

            cv_called.notify_all();
        });

        std::thread t([&]() { f.notify(std::make_shared<int>(10)); });

        // Assert call
        std::mutex cv_m;
        std::unique_lock<std::mutex> lock(cv_m);
        cv_called.wait(lock, [&]() { return called; });
        ASSERT_EQ(core::FutureState::kFinished, f.state());

        // Join
        t.join();
    }

    TEST(Future, TwiceNotify) {
        auto _int = std::make_shared<int>(10);
        bool called = false;
        core::Future<int> f;

        // Define callback
        f.then([&](std::shared_ptr<int> integer) {
            ASSERT_EQ(integer.get(), _int.get());

            called = true;
        });

        // Notify
        f.notify(_int);

        ASSERT_EXCEPTION<ao::core::Exception>([&]() { f.notify(_int); });
    }

}  // namespace ao::test

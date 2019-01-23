// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "thread_pool.h"

#include <mutex>

ao::core::ThreadPool::ThreadPool(size_t size) : tasks_sem(0) {
    this->resize(size);
}

ao::core::ThreadPool::~ThreadPool() {
    this->resize(0);
}

bool ao::core::ThreadPool::isRunning() {
    for (auto& state : this->worker_states) {
        if (state != ao::core::WorkerState::kIdle) {
            return true;
        }
    }
    return false;
}

size_t ao::core::ThreadPool::size() {
    return this->workers.size();
}

void ao::core::ThreadPool::kill() {
    // Block new tasks
    this->tasks_mutex.lock();
    {
        // Change target state
        this->target_state = ao::core::WorkerState::kIdle;

        // Unlock workers
        for (size_t i = 0; i < this->workers.size(); i++) {
            this->tasks_sem.post();
        }

        // Wait workers
        this->LOGGER << ao::core::Logger::Level::trace << "Wait workers";
        for (size_t i = 0; i < this->workers.size(); i++) {
            this->workers[i].join();
        }

        // Clear queue
        std::queue<std::function<void()>> empty;
        this->tasks.swap(empty);
    }
    this->tasks_mutex.unlock();

    this->LOGGER << ao::core::Logger::Level::trace << "ThreadPool killed";
}

void ao::core::ThreadPool::resize(size_t size) {
    // Kill pool
    if (this->isRunning()) {
        this->kill();
    }

    // Resize vectors
    this->workers.resize(size);
    this->worker_states.resize(size, ao::core::WorkerState::kIdle);

    // Exit
    if (size == 0) {
        return;
    }

    // Update target state
    this->target_state = ao::core::WorkerState::kWaiting;

    // Run workers
    for (size_t i = 0; i < size; i++) {
        this->workers[i] = std::thread(
            [&](int id) {
                // Update state
                worker_states[id] = ao::core::WorkerState::kWaiting;

                LOGGER << core::Logger::Level::debug << fmt::format("Start worker {}", id);

                while (true) {
                    // Wait task
                    tasks_sem.wait();

                    // Stop thread
                    if (target_state != ao::core::WorkerState::kWaiting) {
                        break;
                    }

                    // Dequeue task
                    tasks_mutex.lock();
                    auto task = tasks.front();
                    tasks.pop();
                    tasks_mutex.unlock();

                    // Update state
                    worker_states[id] = ao::core::WorkerState::kRunning;

                    // Execute task
                    try {
                        task();
                    } catch (...) {
                        LOGGER << ao::core::Logger::Level::error << fmt::format("Error during executing task on worker: {}", id);
                    }

                    // Stop thread
                    if (target_state != ao::core::WorkerState::kWaiting) {
                        break;
                    }
                }
                LOGGER << core::Logger::Level::debug << fmt::format("Shutdown worker {}", id);

                // Update state
                worker_states[id] = ao::core::WorkerState::kIdle;
            },
            i);
    }
}

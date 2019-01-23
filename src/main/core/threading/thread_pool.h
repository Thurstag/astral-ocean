// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <functional>
#include <queue>
#include <thread>
#include <vector>

#include <boost/interprocess/sync/interprocess_semaphore.hpp>

#include "../logger/logger.h"
#include "future.hpp"

namespace ao::core {
    /// <summary>
    /// WorkerState enumeration
    /// </summary>
    enum class WorkerState { kIdle, kWaiting, kRunning };

    /// <summary>
    /// ThreadPool class
    /// </summary>
    class ThreadPool {
       public:
        /// <summary>
        /// Constructor
        /// </summary>
        ThreadPool() : ThreadPool(0){};

        /// <summary>
        /// Thread pool size
        /// </summary>
        /// <param name="size">Size</param>
        ThreadPool(size_t size);

        /// <summary>
        /// Destructor
        /// </summary>
        virtual ~ThreadPool();

        /// <summary>
        /// Method to know if it's running
        /// </summary>
        /// <returns>Running or not</returns>
        bool isRunning();

        /// <summary>
        /// Method to get size
        /// </summary>
        /// <returns>Size</returns>
        size_t size();

        /// <summary>
        /// Method to kill workers
        /// </summary>
        void kill();

        /// <summary>
        /// Method to enqueue a task
        /// </summary>
        /// <param name="function">Function</param>
        template<class Return>
        std::shared_ptr<Future<Return>> enqueue(std::function<std::shared_ptr<Return>()> function) {
            auto future = std::make_shared<Future<Return>>();

            // Enqueue task
            this->tasks_mutex.lock();
            this->tasks.push([future, function]() { future->notify(function()); });
            this->tasks_mutex.unlock();

            // Notify task
            this->tasks_sem.post();

            return future;
        }

        /// <summary>
        /// Method to create a future with an existing list
        /// </summary>
        /// <param name="futures">Futures</param>
        /// <returns>Future</returns>
        template<class Return>
        static std::shared_ptr<Future<std::vector<Return>>> All(std::vector<std::shared_ptr<Future<Return>>>& futures) {
            auto future = std::make_shared<Future<std::vector<Return>>>();
            auto mutex = std::make_shared<std::mutex>();
            auto vector = std::make_shared<std::vector<Return>>();
            auto size = futures.size();

            // Attach then
            ThreadPool::Then<Return>(futures, [future, mutex, vector, size](std::shared_ptr<Return>& value) {
                mutex->lock();
                {
                    // Add to vector
                    vector->push_back(*value);

                    // Notify
                    if (vector->size() == size) {
                        future->notify(vector);
                    }
                }
                mutex->unlock();
            });

            return future;
        }

        /// <summary>
        /// Method to attach a callback to all futures
        /// </summary>
        /// <param name="futures">Futures</param>
        /// <param name="callback">Callback</param>
        template<class Return>
        static void Then(std::vector<std::shared_ptr<Future<Return>>>& futures, typename Future<Return>::Callback callback) {
            std::for_each(std::execution::par, futures.begin(), futures.end(),
                          [&](std::shared_ptr<Future<Return>>& future) { future->then(callback); });
        }

       protected:
        Logger LOGGER = Logger::GetInstance<ThreadPool>();

        WorkerState target_state;

        boost::interprocess::interprocess_semaphore tasks_sem;
        std::queue<std::function<void()>> tasks;
        std::mutex tasks_mutex;

        std::vector<std::thread> workers;
        std::vector<WorkerState> worker_states;

        /// <summary>
        /// Method to resize pool
        /// </summary>
        /// <param name="size">Size</param>
        void resize(size_t size);
    };
}  // namespace ao::core

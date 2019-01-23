// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <execution>
#include <mutex>
#include <optional>

#include "../logger/logger.h"

namespace ao::core {
    /// <summary>
    /// FutureState enumeration
    /// </summary>
    enum class FutureState { kWaiting, kFinished };

    /// <summary>
    /// Future class
    /// </summary>
    template<class Return>
    class Future {
       public:
        using Callback = std::function<void(std::shared_ptr<Return>)>;

        /// <summary>
        /// Constructor
        /// </summary>
        Future() : state_(FutureState::kWaiting){};

        /// <summary>
        /// Method to define a callback on future's end
        /// </summary>
        /// <param name="callback">Callback</param>
        virtual void then(Callback callback);

        /// <summary>
        /// Method to wait future's end
        /// </summary>
        virtual std::shared_ptr<Return> wait();

        /// <summary>
        /// Method to notify future's end
        /// </summary>
        virtual void notify(std::shared_ptr<Return> value);

        /// <summary>
        /// Method to get state
        /// </summary>
        /// <returns>State</returns>
        virtual FutureState state();

        /// <summary>
        /// Method to attach a callback to all futures
        /// </summary>
        /// <param name="futures">Futures</param>
        /// <param name="callback">Callback</param>
        static void Then(std::vector<Future<Return>>& futures, Callback callback);

        /// <summary>
        /// Method to create a future with an existing list
        /// </summary>
        /// <param name="futures">Futures</param>
        /// <returns>Future</returns>
        static std::shared_ptr<Future<std::vector<Return>>> All(std::vector<Future<Return>>& futures);

       protected:
        core::Logger LOGGER = core::Logger::GetInstance<Future<Return>>();

        std::condition_variable cv;
        std::mutex notif_mutex;
        FutureState state_;

        std::vector<Callback> callbacks;
        std::shared_ptr<Return> value;
    };

    template<class Return>
    inline void Future<Return>::then(Callback callback) {
        this->notif_mutex.lock();
        {
            // Run callback
            if (this->state_ == FutureState::kFinished) {
                callback(this->value);
                this->notif_mutex.unlock();
                return;
            }

            // Assign
            this->callbacks.push_back(callback);
        }
        this->notif_mutex.unlock();
    }

    template<class Return>
    inline std::shared_ptr<Return> Future<Return>::wait() {
        // Check state
        if (this->state_ == FutureState::kFinished) {
            return this->value;
        }

        // Wait
        std::mutex cv_m;
        std::unique_lock<std::mutex> lock(cv_m);
        this->cv.wait(lock, [&]() { return this->state_ == FutureState::kFinished; });

        return this->value;
    }

    template<class Return>
    inline void Future<Return>::notify(std::shared_ptr<Return> value) {
        this->notif_mutex.lock();
        {
            // Check state
            if (this->state_ == FutureState::kFinished) {
                this->notif_mutex.unlock();
                throw ao::core::Exception("Future is already in Finished state");
            }

            // Update value
            this->value = value;

            // Update state
            this->state_ = FutureState::kFinished;

            // Run callback
            for (auto& callback : this->callbacks) {
                callback(this->value);
            }

            // Notify
            this->cv.notify_all();
        }
        this->notif_mutex.unlock();
    }

    template<class Return>
    inline FutureState Future<Return>::state() {
        return this->state_;
    }

    template<class Return>
    inline void Future<Return>::Then(std::vector<Future<Return>>& futures, Callback callback) {
        std::for_each(std::execution::par, futures.begin(), futures.end(), [&](Future<Return>& future) { future.then(callback); });
    }

    template<class Return>
    inline std::shared_ptr<Future<std::vector<Return>>> Future<Return>::All(std::vector<Future<Return>>& futures) {
        auto future = std::make_shared<Future<std::vector<Return>>>();
        auto mutex = std::make_shared<std::mutex>();
        auto vector = std::make_shared<std::vector<Return>>();
        auto size = futures.size();

        // Attach then
        Future<Return>::Then(futures, [future, mutex, vector, size](std::shared_ptr<Return>& value) {
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
}  // namespace ao::core

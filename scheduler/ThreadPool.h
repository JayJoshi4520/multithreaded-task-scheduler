#pragma once
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
using namespace std;

class ThreadPool {
public:
    explicit ThreadPool(size_t n) : stop_(false) {
        if (n == 0) n = 1;
        for (size_t i=0;i<n;i++) {
            workers_.emplace_back([this]{
                for (;;) {
                    function<void()> job;
                    {
                        unique_lock<mutex> lk(mu_);
                        cv_.wait(lk, [this]{ return stop_ || !q_.empty(); });
                        if (stop_ && q_.empty()) return;
                        job = std::move(q_.front());
                        q_.pop();
                    }
                    job();
                }
            });
        }
    }

    ~ThreadPool() {
        {
            lock_guard<mutex> lk(mu_);
            stop_ = true;
        }
        cv_.notify_all();
        for (auto& t : workers_) if (t.joinable()) t.join();
    }

    void enqueue(function<void()> fn) {
        {
            lock_guard<mutex> lk(mu_);
            q_.push(std::move(fn));
        }
        cv_.notify_one();
    }

private:
    vector<thread> workers_;
    queue<function<void()>> q_;
    mutex mu_;
    condition_variable cv_;
    bool stop_;
};

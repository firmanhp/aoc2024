#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>

namespace util {

class ThreadPool {
public:
  ThreadPool(int num_threads) : is_running_(true) {
    runners_.reserve(num_threads);
    for (int i = 0; i < num_threads; ++i) {
      runners_.emplace_back([this]() { loop(); });
    }
  }

  ~ThreadPool() {
    is_running_ = false;
    work_signal_.notify_all();
    for (auto& runner : runners_) {
      if (runner.joinable())
        runner.join();
    }
  }

  void wait_all() {
    std::unique_lock<std::mutex> lock(pending_mtx_);
    // this may be not the proper usage but hey it works
    work_finish_signal_.wait(lock, [this] {
      return working_threads_ == 0 && pending_queue_.empty();
    });
  }

  void add_job(std::function<void()> job) {
    std::lock_guard<std::mutex> lock(pending_mtx_);
    pending_queue_.push(std::move(job));
    work_signal_.notify_one();
  }

private:
  std::optional<std::function<void()>> pop_job() {
    std::unique_lock<std::mutex> lock(pending_mtx_);
    if (pending_queue_.empty()) {
      work_signal_.wait(
          lock, [this] { return !is_running_ || !pending_queue_.empty(); });
      if (pending_queue_.empty()) {
        // someone forced us to go out
        return std::nullopt;
      }
    }

    auto job = pending_queue_.front();
    pending_queue_.pop();
    return job;
  }

  void loop() {
    while (is_running_) {
      auto job = pop_job();
      if (!job.has_value())
        continue;
      ++working_threads_;
      (*job)();
      --working_threads_;
      work_finish_signal_.notify_all();
    }
  }

  bool is_running_;
  std::atomic<int> working_threads_;
  std::queue<std::function<void()>> pending_queue_;
  mutable std::mutex pending_mtx_;
  std::vector<std::thread> runners_;
  std::condition_variable work_signal_;
  std::condition_variable work_finish_signal_;
};

} // namespace util
#include "thread_pool.h"

ThreadPool::ThreadPool(int num_threads) {
  for (int i = 0; i < num_threads; ++i) {
    threads_.emplace_back([this]() { this->InfiniteLoop(); });
  }
}

ThreadPool::~ThreadPool() {
  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    stop_ = true;
  }
  condition_.notify_all();
  for (auto& thread : threads_) {
    thread.join();
  }
}

void ThreadPool::InfiniteLoop() {
  std::function<void()> current_job;
  while (true) {
    {
      std::unique_lock<std::mutex> lock(queue_mutex_);

      if (queue_.empty() && !stop_) {
        // No more work currently needs to be done.
        // Release the mutex, and wait for more work or destructor.
        condition_.wait(lock, [this]() { return !queue_.empty() || stop_; });
      }

      // mutex (re)acquired
      if (stop_) {
        return;
      } else if (!queue_.empty()) {
        // there is a job ready
        current_job = std::move(queue_.front());
        queue_.pop();
      } else {
        // spurious wakeup
        continue;
      }
      // release mutex
    }

    current_job();
  }
}

void ThreadPool::Push(std::function<void()> job) {
  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    queue_.push(std::move(job));
  }
  condition_.notify_one();
}

size_t ThreadPool::NumJobsQueued() {
  std::lock_guard<std::mutex> lock(queue_mutex_);
  return queue_.size();
}

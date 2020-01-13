#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
 public:
  ThreadPool(int num_threads);
  void Push(std::function<void()> job);
  size_t NumJobsQueued();
  ~ThreadPool();

 private:
  void InfiniteLoop();
  bool stop_ = false;
  std::mutex queue_mutex_;
  std::queue<std::function<void()>> queue_;
  std::vector<std::thread> threads_;
  std::condition_variable condition_;
};

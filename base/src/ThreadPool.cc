#include "ThreadPool.h"

#include <assert.h>

#include <exception>

namespace tinyWeb {
ThreadPool::ThreadPool(const std::string& name)\
  : name_(name), mutex_(), notFull_(mutex_),\
    notEmpty_(mutex_), maxQueueSize_(0),\
    running_(false) {
}

ThreadPool::~ThreadPool() {
  if (running_) {
    stop();
  }
}

void ThreadPool::start(int numThread) {
  assert(threads_.empty() && !running_);
  running_ = true;
  threads_.reserve(numThread);
  for (int i = 0; i < numThread; ++i) {
    char id[32];
    snprintf(id, sizeof id, "%d", i + 1);
    threads_.emplace_back(new Thread(
      std::bind(&ThreadPool::runInThread, this), name_ + id));
    threads_[i]->start();
  }

  if (numThread == 0 && threadInitCallback_) {
    threadInitCallback_();
  }
}

void ThreadPool::stop() {
  {
    MutexLockGuard lock(mutex_);
    running_ = false;
    notFull_.notifyAll();
    notEmpty_.notifyAll();
  }

  for (auto& thread : threads_) {
    thread->join();
  }
}

void ThreadPool::addTask(TaskCallback task) {
  if (threads_.empty()) {
    task();
  } else {
    {
      MutexLockGuard lock(mutex_);
      while (isFull() && running_) {
        notFull_.wait();
      }
      if (!running_) return;
      assert(!isFull());

      tasks_.push_back(std::move(task));
    }
    notEmpty_.notify();
  }
}

ThreadPool::TaskCallback ThreadPool::getTask() {
  TaskCallback task;
  {
    MutexLockGuard lock(mutex_);
    while (tasks_.empty() && running_) {
      notEmpty_.wait();
    }
    if (!running_)  return task;
    assert(!tasks_.empty());
    
    task = std::move(tasks_.front());
    tasks_.pop_front();
  }
  if (maxQueueSize_ > 0) notFull_.notify();
  
  return task;
}

void ThreadPool::runInThread() {
  try {
    if (threadInitCallback_) {
      threadInitCallback_();
    }

    while (running_) {
      TaskCallback task(getTask());
      if (task) {
        task();
      }
    }
  } catch (const std::exception& e) {
    fprintf(stderr, "exception caught in ThreadPool: %s\n", name_.c_str());
    fprintf(stderr, "readson: %s\n", e.what());
    abort();
  } catch (...) {
    fprintf(stderr, "unknow exception caught int ThreadPool %s\n", name_.c_str());
    throw;
  }
}

}   // namespace tinyWeb
#pragma once

#include <vector>
#include <memory>
#include <string>

#include "noncopyable.hpp"
#include "LogStream.h"
#include "Thread.h"
#include "CountDownLatch.h"
#include "MutexLock.hpp"
#include "Condition.hpp"
#include "Singleton.hpp"

namespace tinyWeb {

class AsyncLogging : public noncopyable {
 public:
  AsyncLogging(const std::string basename, \
              size_t rollingSize, \
              size_t flushInterval = 3);

  ~AsyncLogging();
  
  void start() {
    assert(!running_);
    assert(currentBuffer_ && !currentBuffer_->length());
    assert(nextBuffer_ && !nextBuffer_->length());

    running_ = true;
    backThread_.start();
    latch_.wait();
  }

  void stop() {
    assert(running_);
    running_ = false;
    cond_.notify();
    backThread_.join();
  }

  void append(const char* logline, size_t len);

 private:
  void threadFunc();

  typedef detail::FixedBuffer<detail::kLargeBufferSize> Buffer;
  typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
  typedef BufferVector::value_type BufferPtr;

  const std::string basename_;
  const size_t rollingSize_;
  const size_t flushInterval_;
  
  Thread backThread_;
  CountDownLatch latch_;

  MutexLock mutex_;
  Condition cond_;  // Guard by mutex_
  BufferPtr currentBuffer_;   // Guard by mutex_
  BufferPtr nextBuffer_;    // Guard by mutex_
  BufferVector buffers_;  // Guard by mutex_
  bool running_;
};

// void asynOutput(const char* logline, size_t len) {
//  Singleton<AsyncLogging>::instance()->append(logline, len);
// }

}   // namespace tinyWeb
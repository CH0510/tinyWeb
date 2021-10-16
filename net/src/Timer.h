#pragma once

#include <functional>

#include "../../base/src/noncopyable.hpp"
#include "../../base/src/Timestamp.h"
#include "../../base/src/Atomic.hpp"
#include "../../base/src/Types.h"

namespace tinyWeb {
namespace net {

class TimerQueue;

class Timer : public noncopyable {
 public:
  Timer(TimerQueue* timerQueue, TimerCallback cb, \
        Timestamp timePoint, double interval = 0.0);
  ~Timer() = default;

  Timestamp expireTime() const { return expireTime_; }
  bool repeated() const { return repeated_; }
  int64_t sequence() const { return sequence_; }

  void run() {
    expireCallback_();
  }

  void restart(Timestamp now);  // 重启定时器

 private:
  TimerQueue* ownerQueue_;
  TimerCallback expireCallback_;
  Timestamp expireTime_;
  double interval_;
  bool repeated_;
  int64_t sequence_;

  static AtomicInt64 numCreated_;
};
}  // namespace net
}  // namespace tinyWeb
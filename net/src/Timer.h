#pragma once

#include <functional>

#include "../../base/src/noncopyable.hpp"
#include "../../base/src/Timestamp.h"
#include "../../base/src/Atomic.hpp"
#include "../../base/src/Types.h"

namespace tinyWeb {
namespace net {

class TimerQueue;

// 内部类，记录定时器的基本信息
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
  // 重启定时器
  // 使用者需保证由周期定时器发起
  void restart(Timestamp now);

 private:
  // 所属的定时队列
  TimerQueue* ownerQueue_;
  // 超时回调
  TimerCallback expireCallback_;
  // 超时时间，是一个time point
  Timestamp expireTime_;
  // 定时器的周期，0.0表示是一次性定时器
  double interval_;
  // 是否是周期定时器
  bool repeated_;
  // 定时器的序列号
  // 因为无法识别地址相同的两个定时器
  int64_t sequence_;

  static AtomicInt64 numCreated_;
};
}  // namespace net
}  // namespace tinyWeb
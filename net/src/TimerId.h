#pragma once

#include <stdint.h>

namespace tinyWeb {
namespace net {
class Timer;

// 提供给客户端使用的定时器标识
// 用于取消定时器，多次取消不会出现问题
// 例：
// EventLoop loop;
// TimerId timerId = loop.runAfter(cb, 3);  // 3s后运行cb
// loop->cancel(timerId);  // 取消上述创建的定时器
// loop->cancel(timerId);  // 多次取消是无害的
// loop.loop();
class TimerId {
 public:
  TimerId(int64_t sequence, Timer* timer) \
    : sequence_(sequence), timer_(timer) {
  }

  ~TimerId() = default;
  // 返回内部定时器对应的序列号
  int64_t sequence() const { return sequence_; }

 private:
  friend class TimerQueue;

  const int64_t sequence_;
  // 供TimerQueue获取一直关联的Timer对象
  Timer* timer_;
};
}  // namespace net
}  // namespace tinyWeb
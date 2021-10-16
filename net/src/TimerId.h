#pragma once

#include <stdint.h>

namespace tinyWeb {
namespace net {
class Timer;

class TimerId {
 public:
  TimerId(int64_t sequence, Timer* timer) \
    : sequence_(sequence), timer_(timer) {
  }

  ~TimerId() = default;
  
  int64_t sequence() const { return sequence_; }

 private:
  friend class TimerQueue;

  const int64_t sequence_;
  Timer* timer_;
};
}  // namespace net
}  // namespace tinyWeb
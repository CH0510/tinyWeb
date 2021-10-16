#include "Timer.h"

#include <assert.h>
#include "TimerQueue.h"
#include "TimerId.h"

namespace tinyWeb {
namespace net {

AtomicInt64 Timer::numCreated_;

Timer::Timer(TimerQueue* timerQueue, TimerCallback cb, \
             Timestamp timePoint, double interval) \
  : ownerQueue_(timerQueue), \
    expireCallback_(cb), \
    expireTime_(timePoint), \
    interval_(interval), \
    repeated_(interval > 0.0), \
    sequence_(numCreated_.incrementAndGet()) {
}

void Timer::restart(Timestamp now) {
  assert(interval_ > 0.0 && repeated_ == true);
  expireTime_ = addTime(Timestamp::now(), interval_);
}

}  // namespace net
}  // namespace tinyWeb
#pragma once

#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

#include "../../base/src/noncopyable.hpp"
#include "../../base/src/Timestamp.h"
#include "../../base/src/Types.h"

namespace tinyWeb {
namespace net {

class EventLoop;
class Channel;
class Timer;
class TimerId;
class TimerQueue : public noncopyable {
 public:
  explicit TimerQueue(EventLoop* loop);
  ~TimerQueue();

  // 增加定时器到定时队列
  // 线程安全的
  TimerId addTimer(TimerCallback cb, \
        Timestamp timePoint, double interval);

  // 删除定时器
  // 线程安全的
  void cancel(TimerId timer);

 private:
  // priority_queue没有提供高效的删除操作
  // 使用map会导致不能存在相同的Timestamp的定时器
  // TODO(chenhao980510@163.com) ： 使用索引最小堆来进行替换
  // 没使用unique_ptr管理Timer实例，因为这样导致在set容器中
  // 查找目标定时的时间复杂度会退化（没有办法创建两个一样的uniqe_ptr）
  typedef std::pair<Timestamp, Timer*> Entry;
  typedef std::set<Entry> TimerList;

  // 以下函数都是线程不安全的，必须在IO线程中调用
  void addTimerInLoop(Timer* timer);
  // 插入定时器
  // 其会返回新的定时器是否是最早超时的，以指示是否需要调整
  bool insert(Timer* timer);
  // 在IO线程中移除指定的定时器
  void cancelInLoop(TimerId timer);
  // 从定时队列中删除定时器，并返回是否需要调整定时器
  bool delTimer(Timer* timer);
  // 如果插入或删除导致最低定时有变化将调用此函数
  // 调整定时器的到期时间
  void adjustTimer();
  // 获取所有过期的定时器
  std::vector<Timer*> getAllExpired(Timestamp now);
  // 重置当前过期定时器中的循环定时器
  void reset(std::vector<Timer*>&, Timestamp);
  // 定时器超时回调函数
  void handleRead();

  EventLoop* ownerLoop_;
  const int timerFd_;
  std::unique_ptr<Channel> timerChannel_;
  TimerList timerQueue_;
  // 通过TimerId快速查找到对应的Timer
  std::unordered_map<int64_t, Timer*> activeTimer_;
  // 调用cancel而待删除的定时器已超时走则入此队列
  // 在该队列中的定时器将不会重启
  std::set<Timer*> canceledSet_;
  bool callingTimerCallback_;
};
}  // namespace net
}  // namespace tinyWeb
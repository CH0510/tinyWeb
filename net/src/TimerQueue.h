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

// 内部类，负责维护定时器队列
// 内部采用红黑树的方法来实现管理队列时的
// 时间复杂度为O(logN)，但是空间局部性不是很好
// 因为std::priority_queue没有提供高效删除元素的方法。
// TODO(chenhao980510@163.com):实现一个索引优先队列用于底层定时队列的管理
class TimerQueue : public noncopyable {
 public:
  explicit TimerQueue(EventLoop* loop);
  ~TimerQueue();

  // 向定时队列中插入定时器
  // 线程安全的
  // 参数：
  //   cb:超时回调函数
  //   timePoint: 超时时刻
  //   iterval：定时器的周期，0.0表示一次性定时器
  // 返回值：
  //   返给用户的与当前Timer关联的TimerId实例
  TimerId addTimer(TimerCallback cb, \
        Timestamp timePoint, double interval);

  // 从定时队列中删除与timer关联的定时器
  // 线程安全的
  // 需注意一个已经超时的定时器被取走到超时队列中后就一定会被执行
  void cancel(TimerId timer);

 private:
  // priority_queue没有提供高效的删除操作
  // 使用map会导致不能存在相同的Timestamp的定时器
  // TODO(chenhao980510@163.com) ： 使用索引优先队列来进行替换
  // 没使用unique_ptr管理Timer实例，因为这样导致在set容器中
  // 查找目标定时的时间复杂度会退化（没有办法创建两个一样的uniqe_ptr）
  typedef std::pair<Timestamp, Timer*> Entry;
  typedef std::set<Entry> TimerList;

  // 以下函数都是线程不安全的，必须在IO线程中调用
  // 向定时队列插入定时器的内部实现
  void addTimerInLoop(Timer* timer);
  // 插入定时器到定时器队列
  // 返回值以指示是否需要对超时时间进行调整
  bool insert(Timer* timer);
  // 移除指定的定时器内部实现
  void cancelInLoop(TimerId timer);
  // 从定时队列中删除定时器
  // 返回值指示是否需要调整超时时间
  bool delTimer(Timer* timer);
  // 如果插入或删除导致超时事件有变化将调用此函数
  // 调整定时器的到期时间
  void adjustTimer();
  // 获取所有过期的定时器
  std::vector<Timer*> getAllExpired(Timestamp now);
  // 重置当前过期定时器中的循环定时器
  // 返回值用于指示是否需要进行调整超时时间
  bool reset(std::vector<Timer*>&, Timestamp);
  // 定时器超时回调函数
  void handleRead();

  EventLoop* ownerLoop_;
  const int timerFd_;
  std::unique_ptr<Channel> timerChannel_;
  TimerList timerQueue_;      // 定时器队列
  // 通过TimerId快速查找到对应的Timer
  // 其与timerQueue_应当包含相同的定时器
  std::unordered_map<int64_t, Timer*> activeTimer_;
  // 调用cancel而待删除的定时器已超时走则入此队列
  // 在该队列中的定时器将不会重启
  std::set<Timer*> canceledSet_;
  bool callingTimerCallback_;
};
}  // namespace net
}  // namespace tinyWeb
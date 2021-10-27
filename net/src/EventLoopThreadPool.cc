#include "EventLoopThreadPool.h"

namespace tinyWeb {
namespace net {

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, \
                                         const StringArg& name) \
  : baseLoop_(baseLoop), \
    name_(name.c_str()), \
    numThreads_(0), \
    started_(false), \
    next_(0) {
}

void EventLoopThreadPool::start(const ThreadInitCallback& cb) {
  assert(started_ == false);
  baseLoop_->assertInLoopThread();

  threads_.reserve(numThreads_);
  loops_.reserve(numThreads_);

  for (int i = 0; i < static_cast<int>(numThreads_); ++i) {
    std::string threadName = name_ + std::to_string(i);
    threads_.emplace_back(new EventLoopThread(cb, threadName));
    loops_.push_back(threads_[i]->startInLoop());
  }
  started_ = true;

  assert(threads_.size() == numThreads_);
  assert(loops_.size() == numThreads_);
}

EventLoop* EventLoopThreadPool::getNextLoop() {
  baseLoop_->assertInLoopThread();
  
  EventLoop* loop = baseLoop_;
  if (loops_.empty() == false) {
    loop = loops_[next_++];
    if (next_ == static_cast<int>(loops_.size())) {
      next_ = 0;
    }
  }

  return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops() const {
  assert(started_);

  if (loops_.empty()) {
    return std::vector<EventLoop*>{baseLoop_};
  } else {
    return loops_;
  }
}

}  // namespace net
}  // namespace tinyWeb
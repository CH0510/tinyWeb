// @Author Hao Chen
// @Email chenhao980510@163.com
#include "Thread.h"

#include <sys/prctl.h>

#include <utility>

namespace tinyWeb {
namespace detail {

// 用于主线程的初始化，并设置fork()系统调用前后的相关工作
void afterFork() {
  CurrentThread::t_cachedTid = 0;
  CurrentThread::t_threadName = "main";
  CurrentThread::tid();
}

class ThreadNameInitializer {
 public:
  ThreadNameInitializer() {
    CurrentThread::t_threadName = "main";
    CurrentThread::tid();
    MCHECK(pthread_atfork(NULL, NULL, afterFork));
  }
};
// 先于main函数运行，设置主线程的线程名
ThreadNameInitializer init;

// 为了使线程的生命期与 Thread 变量的生命期解耦
struct ThreadData {
  typedef Thread::ThreadFunc ThreadFunc;
  ThreadFunc func_;
  std::string name_;
  pid_t* tid_;
  CountDownLatch* latch_;

  ThreadData(ThreadFunc&& func_, const std::string& name, \
            pid_t* tid, CountDownLatch* latch) \
    : func_(std::forward<ThreadFunc>(func_)), \
      name_(name), tid_(tid), latch_(latch)
    { }

  void runInThread() {
    *tid_ = CurrentThread::tid();
    tid_ = NULL;
    latch_->countDown();
    latch_ = NULL;
    CurrentThread::t_threadName = name_.empty() ? \
                                    "tinyWeb-Thread" : \
                                    name_.c_str();
    // 方便调试，可用pthread_setname_np()代替,
    // 参考https://man7.org/linux/man-pages/man3/pthread_setname_np.3.html
    MCHECK(::prctl(PR_SET_NAME, CurrentThread::t_threadName));
    try {
        func_();
        CurrentThread::t_threadName = "finished";
    } catch (const std::exception& ex) {
        CurrentThread::t_threadName = "crashed";
        fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
        abort();
    } catch (...) {
        CurrentThread::t_threadName = "unknow";
        fprintf(stderr, "unknow exception caught in Thread %s\n", \
                name_.c_str());
        throw;
    }
  }
};

}  // namespace detail

AtomicInt32 Thread::numCreated_;

void* startThread(void* obj) {
  detail::ThreadData* data = static_cast<detail::ThreadData*>(obj);
  data->runInThread();
  delete data;
  return NULL;
}

void Thread::setDefaultName() {
  int num = numCreated_.incrementAndGet();
  if (name_.empty()) {
    char buf[32];
    snprintf(buf, sizeof(buf), "Thread%d", num);
    name_ = buf;
  }
}

Thread::Thread(ThreadFunc threadFunc, const std::string& name) \
  : threadFunc_(std::move(threadFunc)), name_(name), \
    tid_(0), pthreadId_(0), latch_(1), started_(false), \
    joined_(false) {
  setDefaultName();
}

Thread::~Thread() {
  if (started_ && !joined_) {
    MCHECK(pthread_detach(pthreadId_));
  }
}

int Thread::join() {
  assert(started_);
  assert(!joined_);
  joined_ = true;
  return pthread_join(pthreadId_, NULL);
}

void Thread::start() {
  assert(started_ == false);
  started_ = true;
  detail::ThreadData* data = new detail::ThreadData( \
                                        std::move(threadFunc_), \
                                        name_, \
                                        &tid_, \
                                        &latch_);
  if (pthread_create(&pthreadId_, NULL, startThread, data)) {
    started_ = false;
    delete data;
    exit(1);
  } else {
    // 等待线程创建完毕后才退出
    latch_.wait();
    assert(tid_ > 0);
  }
}

}  // namespace tinyWeb

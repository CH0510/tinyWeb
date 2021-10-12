# ThreadPool设计细节
- 将内部任务队列的大小抽取出来，使得既可以做成无界或有界队列。
- 在运行时才进行线程的创建，避免创建ThreadPool却不使用的情况，浪费线程与内存资源。
- 设置一个标志为running_以方便通知所有线程退出

## 错误一
    测试代码中  pool.addTask(std::bind(printString, std::string(buf)));如果不对buf转化为std::string，由于bind使用的参数是右值引用，将会推导出数组类型，进而将buf的地址作为参数传递给任务。这样就导致共享了主线程的局部变量，导致最终答应的任务序号大部分相同
20211003 04:03:24.347443Z 1386961 WRAN  Done - ThreadPool_test.cc:34
20211003 04:03:24.347445Z 1386961 DEBUG assertLocked holder = 1386961, tid = 1386961 - MutexLock.hpp:37
20211003 04:03:24.347447Z 1386961 DEBUG assertLocked holder = 1386961, tid = 1386961 - MutexLock.hpp:37
20211003 04:03:24.347471Z 1386964 INFO  task 99 - ThreadPool_test.cc:16
20211003 04:03:24.347503Z 1386963 INFO  task 99 - ThreadPool_test.cc:16
20211003 04:03:24.348891Z 1386965 INFO  task 99 - ThreadPool_test.cc:16
20211003 04:03:24.349034Z 1386963 INFO  task 99 - ThreadPool_test.cc:16
20211003 04:03:24.349069Z 1386962 INFO  task 99 - ThreadPool_test.cc:16
20211003 04:03:24.349058Z 1386966 INFO  task 99 - ThreadPool_test.cc:16
20211003 04:03:24.349042Z 1386964 INFO  task 99 - ThreadPool_test.cc:16
20211003 04:03:24.350057Z 1386965 INFO  task 99 - ThreadPool_test.cc:16
20211003 04:03:24.351049Z 1386963 INFO  task 99 - ThreadPool_test.cc:16
20211003 04:03:24.351076Z 1386964 INFO  task 99 - ThreadPool_test.cc:16
20211003 04:03:24.351077Z 1386962 INFO  task 99 - ThreadPool_test.cc:16
20211003 04:03:24.351366Z 1386965 INFO  task 99 - ThreadPool_test.cc:16
20211003 04:03:24.351866Z 1386966 INFO  task 99 - ThreadPool_test.cc:16
20211003 04:03:24.352770Z 1386963 INFO  task 99 - ThreadPool_test.cc:16
20211003 04:03:24.352788Z 1386964 INFO  task 99 - ThreadPool_test.cc:16
20211003 04:03:24.353089Z 1386966 INFO  task 99 - ThreadPool_test.cc:16
20211003 04:03:24.353129Z 1386965 INFO  task 99 - ThreadPool_test.cc:16
20211003 04:03:24.353091Z 1386962 INFO  task 99 - ThreadPool_test.cc:16
20211003 04:03:24.353988Z 1386963 INFO  task 99 - ThreadPool_test.cc:16

解决方法是初始化一个临时的string对象：pool.addTask(std::bind(printString, std::string(buf)));
20211003 04:05:01.175754Z 1387302 INFO  task 81 - ThreadPool_test.cc:16
20211003 04:05:01.175768Z 1387299 INFO  task 82 - ThreadPool_test.cc:16
20211003 04:05:01.175815Z 1387303 INFO  task 83 - ThreadPool_test.cc:16
20211003 04:05:01.175967Z 1387300 INFO  task 84 - ThreadPool_test.cc:16
20211003 04:05:01.176714Z 1387301 INFO  task 85 - ThreadPool_test.cc:16
20211003 04:05:01.177728Z 1387300 INFO  task 87 - ThreadPool_test.cc:16
20211003 04:05:01.177727Z 1387299 INFO  task 86 - ThreadPool_test.cc:16
20211003 04:05:01.177736Z 1387302 INFO  task 88 - ThreadPool_test.cc:16
20211003 04:05:01.177813Z 1387303 INFO  task 89 - ThreadPool_test.cc:16
20211003 04:05:01.178764Z 1387301 INFO  task 90 - ThreadPool_test.cc:16
20211003 04:05:01.178858Z 1387299 INFO  task 91 - ThreadPool_test.cc:16
20211003 04:05:01.178931Z 1387302 INFO  task 92 - ThreadPool_test.cc:16
20211003 04:05:01.178943Z 1387303 INFO  task 93 - ThreadPool_test.cc:16
20211003 04:05:01.178966Z 1387300 INFO  task 94 - ThreadPool_test.cc:16
20211003 04:05:01.180726Z 1387302 INFO  task 95 - ThreadPool_test.cc:16
20211003 04:05:01.180749Z 1387301 INFO  task 96 - ThreadPool_test.cc:16
20211003 04:05:01.180758Z 1387303 INFO  task 97 - ThreadPool_test.cc:16
20211003 04:05:01.180773Z 1387299 INFO  task 98 - ThreadPool_test.cc:16
20211003 04:05:01.180807Z 1387300 INFO  task 99 - ThreadPool_test.cc:16

## 错误二
  ThreadPool的isFull会检查锁是否被当前线程获取:
  
    bool isFull() const {
      mutex_.assertLocked();
      return maxQueueSize_ > 0 && tasks_.size() >= maxQueueSize_;  //只在有界队列时进行检查
    }

但是但运行时发生错误:
20211003 04:05:01.183049Z 1387298 WRAN  Test ThreadPool with max queue size = 1 - ThreadPool_test.cc:21
20211003 04:05:01.183866Z 1387298 WRAN  Adding - ThreadPool_test.cc:26
20211003 04:05:01.183892Z 1387298 DEBUG assertLocked holder = 1387298, tid = 1387298 - MutexLock.hpp:37
20211003 04:05:01.183897Z 1387298 DEBUG assertLocked holder = 1387298, tid = 1387298 - MutexLock.hpp:37
20211003 04:05:01.183915Z 1387298 DEBUG assertLocked holder = 1387298, tid = 1387298 - MutexLock.hpp:37
tid = 1387304
20211003 04:05:01.184000Z 1387298 DEBUG assertLocked holder = 0, tid = 1387298 - MutexLock.hpp:37
ThreadPool_test: /home/chenhao/tinyWeb/base/src/MutexLock.hpp:38: void tinyWeb::MutexLock::assertLocked() const: Assertion `isLockedByThisThread()' failed.
已放弃 (核心已转储)

发现日志显示拥有锁的与当前线程并不是一个，但是检查代码发现，使用isFull的代码的线程肯定先获得锁才能检查：
void ThreadPool::addTask(TaskCallback task) {
  if (threads_.empty()) {
    task();
  } else {
    {
      MutexLockGuard lock(mutex_);
      while (isFull() && running_) {
        notFull_.wait();
      }
      if (!running_) return;
      assert(!isFull());

      tasks_.push_back(std::move(task));
    }
    notEmpty_.notify();
  }
}

- 运行第一次：
20211003 04:28:31.078220Z 1390582 WRAN  Adding - ThreadPool_test.cc:26
20211003 04:28:31.078237Z 1390582 DEBUG lock holder = 1390582, tid = 1390582 - MutexLock.hpp:45
20211003 04:28:31.078239Z 1390582 DEBUG assertLocked holder = 1390582, tid = 1390582 - MutexLock.hpp:37
20211003 04:28:31.078241Z 1390582 DEBUG assertLocked holder = 1390582, tid = 1390582 - MutexLock.hpp:37
20211003 04:28:31.078256Z 1390582 DEBUG lock holder = 1390582, tid = 1390582 - MutexLock.hpp:45
20211003 04:28:31.078259Z 1390582 DEBUG assertLocked holder = 1390582, tid = 1390582 - MutexLock.hpp:37
tid = 1390588
20211003 04:28:31.078333Z 1390588 DEBUG lock holder = 1390588, tid = 1390588 - MutexLock.hpp:45
20211003 04:28:31.078358Z 1390582 DEBUG assertLocked holder = 1390588, tid = 1390582 - MutexLock.hpp:37
ThreadPool_test: /home/chenhao/tinyWeb/base/src/MutexLock.hpp:38: void tinyWeb::MutexLock::assertLocked() const: Assertion `isLockedByThisThread()' failed.
已放弃 (核心已转储)

- 运行第二次
20211003 04:31:43.266221Z 1390821 WRAN  Adding - ThreadPool_test.cc:26
20211003 04:31:43.266244Z 1390821 DEBUG lock holder = 1390821, tid = 1390821 - MutexLock.hpp:45
20211003 04:31:43.266248Z 1390821 DEBUG assertLocked holder = 1390821, tid = 1390821 - MutexLock.hpp:37
20211003 04:31:43.266251Z 1390821 DEBUG assertLocked holder = 1390821, tid = 1390821 - MutexLock.hpp:37
tid = 1390827
20211003 04:31:43.266280Z 1390827 DEBUG lock holder = 1390827, tid = 1390827 - MutexLock.hpp:45
20211003 04:31:43.266292Z 1390821 DEBUG lock holder = 1390821, tid = 1390821 - MutexLock.hpp:45
20211003 04:31:43.266295Z 1390821 DEBUG assertLocked holder = 1390821, tid = 1390821 - MutexLock.hpp:37
20211003 04:31:43.266314Z 1390821 DEBUG assertLocked holder = 1390821, tid = 1390821 - MutexLock.hpp:37
20211003 04:31:43.266349Z 1390821 DEBUG lock holder = 1390821, tid = 1390821 - MutexLock.hpp:45
20211003 04:31:43.266366Z 1390821 DEBUG assertLocked holder = 1390821, tid = 1390821 - MutexLock.hpp:37
tid = 1390828
20211003 04:31:43.266440Z 1390821 DEBUG assertLocked holder = 0, tid = 1390821 - MutexLock.hpp:37
ThreadPool_test: /home/chenhao/tinyWeb/base/src/MutexLock.hpp:38: void tinyWeb::MutexLock::assertLocked() const: Assertion `isLockedByThisThread()' failed.
已放弃 (核心已转储)

结合两次的运行日志，发现问题在于在addTask循环之后的 assert中的isFull会出错

- 问题：结合MutexLock和Condition的代码可以总结出问题在于:
  void ThreadPool::addTask(TaskCallback task) {
    if (threads_.empty()) {
      task();
    } else {
      {
        MutexLockGuard lock(mutex_);
        while (isFull() && running_) {
          notFull_.wait();
        }
        if (!running_) return;
        assert(!isFull());    //产生错误的地方

        tasks_.push_back(std::move(task));
      }
      notEmpty_.notify();
    }
  }

由于Condition::wait()只是单纯的调用pthread_cond_wait，他会将锁释放，然后等待通知，但是他并未清空holder，也没有在收到notify()后恢复holder,考虑如下的情况：
- 主线程添加一个任务后导致队列满了，因此调用notFull.wait()，这将会释放锁此时holder等于主线程id
- 子线程或的锁后holder变成了子线程的id，然后消费任务队列中的任务，调用notify通知了主线程
- 主线程从notFull.wait()中回复，但是此时holder不再是主线程的tid，进而调用assert(!isFull());->assertLocked()会失败。

那么解决的方法就在于调用Condition::wait()之前消除holder,在收到notify()之后，即将从Condition::wait()中返回是恢复holder为当前的线程tid。所以在MutexLock中引入了UnassignGuard辅助类。分别在其构造和析构函数中将holder清除和恢复。并用于Condition::wait()调用pthread_cond_wait之前。
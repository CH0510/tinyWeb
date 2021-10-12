#include "src/ThreadPool.h"
#include "src/CountDownLatch.h"
#include "src/CurrentThread.h"
#include "src/Logging.h"

#include <stdio.h>
#include <unistd.h>

using namespace tinyWeb;

void print() {
  printf("tid = %d\n", CurrentThread::tid());
}

void printString(const std::string& str) {
  LOG_INFO << str;
  usleep(1000);
}

void test(int maxSize) {
  LOG_WARN << "Test ThreadPool with max queue size = " << maxSize;
  ThreadPool pool("MainThreadPool");
  pool.setMaxQueueSize(maxSize);
  pool.start(5);

  LOG_WARN << "Adding";
  pool.addTask(print);
  pool.addTask(print);
  for (int i = 0; i < 100; i++) {
    char buf[32] = {0};
    snprintf(buf, sizeof buf, "task %d", i);
    pool.addTask(std::bind(printString, std::string(buf)));
  }
  LOG_WARN << "Done";

  CountDownLatch latch(1);
  pool.addTask(std::bind(&CountDownLatch::countDown, &latch));
  latch.wait();
  pool.stop();
}

void longTask(int num) {
  LOG_INFO << "longTask" << num;
  usleep(3000000);
}

void test2() {
  LOG_WARN << "Test ThreadPool by stop early";
  ThreadPool pool("ThreadPool");
  pool.setMaxQueueSize(5);
  pool.start(3);

  Thread thread1([&pool]() {
    for (int i = 0; i < 20; ++i) {
      pool.addTask(std::bind(longTask, i));
    }
  }, "thread1");
  thread1.start();

  usleep(5000000);
  LOG_WARN << "stop pool";
  pool.stop();

  thread1.join();
  pool.addTask(print);
  LOG_WARN << "test2 Down";
}

int main() {
  test(0);
  test(1);
  test(5);
  test(10);
  test(50);

  test2();
}
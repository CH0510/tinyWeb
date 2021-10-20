#include <stdio.h>
#include <unistd.h>

#include <functional>

#include "src/EventLoop.h"
#include "../base/src/Thread.h"
#include "src/TimerId.h"
#include "src/EventLoopThread.h"
using namespace tinyWeb;
using namespace tinyWeb::net;

int cnt = 0;
EventLoop* g_loop;

void printTid() {
  printf("pid = %d, tid = %d\n", getpid(), CurrentThread::tid());
  printf("now %s\n", Timestamp::now().toString().c_str());
}

void print(const char* msg) {
  printf("msg %s %s\n", Timestamp::now().toString().c_str(), msg);
  if (++cnt == 20) {
    g_loop->quit();
  }
}

void cancel(TimerId timer) {
  g_loop->cancel(timer);
  printf("cancelled at %s\n", Timestamp::now().toString().c_str());
}

int main() {
  printTid();
  sleep(1);
  {
    EventLoop loop;
    g_loop = &loop;

    printf("main\n");
    loop.runAfter(std::bind(print, "once1"), 1);
    loop.runAfter(std::bind(print, "once1.5"), 1.5);
    loop.runAfter(std::bind(print, "once2.5"), 2.5);
    loop.runAfter(std::bind(print, "once3.5"), 3.5);
    TimerId t45 = loop.runAfter(std::bind(print, "once4.5"), 4.5);
    loop.runAfter(std::bind(cancel, t45), 4.2);
    loop.runAfter(std::bind(cancel, t45), 4.8);
    loop.runEvery(std::bind(print, "every2"), 2, 2);
    TimerId t3 = loop.runEvery(std::bind(print, "every3"), 3, 3);
    loop.runAfter(std::bind(cancel, t3), 9.001);

    loop.loop();
    print("main loop exists");
  }
  sleep(1);
  {
    EventLoopThread loopThread;
    EventLoop* loop = loopThread.startInLoop();
    loop->runAfter(printTid, 2);
    sleep(3);
    print("thread loop exits");
  }
}
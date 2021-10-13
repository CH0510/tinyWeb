#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/timerfd.h>

#include <functional>

#include "src/EventLoop.h"
#include "src/Channel.h"
#include "../base/src/Timestamp.h"
#include "../base/src/Thread.h"

tinyWeb::net::EventLoop* g_loop;
int timerfd;

void timeout(tinyWeb::Timestamp receiveTimer) {
  printf("Timeout!\n");
  uint64_t howmany;
  ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
  (void)n;
  g_loop->quit();
}

void threadFunc1() {
  printf("test wake up EventLoop!\n");
  g_loop->wakeup();
}

void print() {
  printf("pid = %d, tid = %d\n", \
          getpid(), tinyWeb::CurrentThread::tid());
}

void print2(int x) {
  printf("pid = %d, tid = %d with x = %d\n", \
         getpid(), tinyWeb::CurrentThread::tid(), x);
}

void threadFunc2() {
  printf("test runInLoop!\n");
  g_loop->runInLoop(print);
  g_loop->runInLoop(std::bind(print2, 10));
}

int main(int argc, char** argv) {
  tinyWeb::net::EventLoop loop;
  g_loop = &loop;

  timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  tinyWeb::net::Channel channel(&loop, timerfd);
  channel.setReadCallback(timeout);
  channel.enableReading();

  struct itimerspec howlong;
  bzero(&howlong, sizeof howlong);
  howlong.it_value.tv_sec = 2;
  ::timerfd_settime(timerfd, 0, &howlong, NULL);
  tinyWeb::Thread t1(threadFunc1);
  tinyWeb::Thread t2(threadFunc2);
  t1.start();
  t2.start();
  loop.loop();
  channel.disableAll();
  channel.remove();
  ::close(timerfd);

  return 0;
}
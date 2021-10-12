#include "src/BlockingQueue.hpp"
#include "src/CountDownLatch.h"
#include "src/Thread.h"

#include <memory>
#include <string>
#include <vector>
#include <cstdio>
#include <unistd.h>

using namespace tinyWeb;

class Test {
public:
    Test(int numThreads) : latch_(numThreads) {
        threads_.reserve(numThreads);
        for (int i = 0; i < numThreads; ++i) {
            char name[32];
            memset(name, 0, sizeof(name));
            snprintf(name, sizeof(name), "work thread %d", i);
            threads_.emplace_back(new Thread(std::bind(&Test::threadFunc, this), name));
        }

        for (std::unique_ptr<Thread>& thread : threads_) {
            thread->start();
        }
    }

    void run(int times) {
        printf("waiting for count down latch\n");
        latch_.wait();
        printf("all threads started\n");
        for (int i = 0; i < times; i++) {
            char buf[32];
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "hello %d", i);
            queue_.addTask(buf);
            printf("tid = %d, add task = %s, size = %zd\n",\
                    CurrentThread::tid(), buf, queue_.size());
        }
    }

    void joinAll() {
        for (size_t i = 0; i < threads_.size(); i++) {
            queue_.addTask("quit");
        }

        for (size_t i = 0; i < threads_.size(); i++) {
            threads_[i]->join();
        }
    }

private:
    void threadFunc() {
        printf("tid = %d, %s started\n", CurrentThread::tid(),\
                CurrentThread::name());
        
        latch_.countDown();
        bool running = true;
        while (running) {
            std::string task(queue_.getTask());
            printf("tid = %d, get data = %s, size = %zd\n",\
                    CurrentThread::tid(), task.c_str(), queue_.size());
            running =  task != "quit";
        }

        printf("tid = %d, %s stop\n", CurrentThread::tid(), CurrentThread::name());
    }

    BlockingQueue<std::string> queue_;
    CountDownLatch latch_;
    std::vector<std::unique_ptr<Thread>> threads_;
};

void testMove() {
    BlockingQueue<std::unique_ptr<int>> queue;
    queue.addTask(std::unique_ptr<int>(new int(42)));
    std::unique_ptr<int> x = queue.getTask();
    printf("getTask %d\n", *x);
    *x = 123;
    queue.addTask(std::move(x));
    std::unique_ptr<int> y = queue.getTask();
    printf("getTask %d\n", *y);
}

int main(int argc, char* argv[]) {
    printf("pid = %d, tid = %d\n", getpid(), CurrentThread::tid());
    Test t(5);
    t.run(100);
    t.joinAll();

    testMove();

    printf("numbers of created threads: %d\n", Thread::numCreated());

    return 0;
}
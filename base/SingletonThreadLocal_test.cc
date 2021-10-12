#include "src/ThreadLocal.hpp"
#include "src/Thread.h"
#include "src/CurrentThread.h"
#include "src/Singleton.hpp"
#include <cstdio>

using namespace tinyWeb;

class Test {
public:
    Test() {
        printf("tid = %d, constructing %p\n", CurrentThread::tid(), this);
    }

    Test(const std::string& name) {
        printf("tid = %d, constructing %p with %s\n", CurrentThread::tid(), this, name.c_str());
    }

    ~Test() {
        printf("tid = %d, destructing %p %s\n", CurrentThread::tid(), this, name_.c_str());
    }

    const std::string& name() const { return name_; }
    void setName(const std::string& value) { name_ = value; }

private:
    std::string name_;   
};

ThreadLocal<Test>* testObj1 = Singleton<ThreadLocal<Test>>::instance();
ThreadLocal<Test>* testObj2 = Singleton<ThreadLocal<Test>>::instance();

void print() {
    printf("tid = %d, obj1 %p value = %s\n",\
            CurrentThread::tid(), testObj1->value(),\
            testObj1->value()->name().c_str());
    printf("tid = %d, obj2 %p value = %s\n",\
            CurrentThread::tid(), testObj2->value(),\
            testObj2->value()->name().c_str());
}

void threadFunc() {
    print();
    testObj1->value()->setName("change 1");
    testObj2->value()->setName("change 42");
    print();
}

int main(int argc, char* argv[]) {
    testObj1->value("main once");
    print();
    Thread t1(threadFunc);
    t1.start();
    t1.join();
    testObj2->value()->setName("main two");
    print();

    return 0;
}
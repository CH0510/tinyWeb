#include <string>

#include "src/Singleton.hpp"
#include "src/Thread.h"
#include "src/noncopyable.hpp"

using namespace tinyWeb;

class Test : noncopyable {
public:
    Test() {
        printf("tid=%d, constructing %p\n", CurrentThread::tid(), this);
    }

    ~Test() {
        printf("tid=%d, destructing %p %s\n", CurrentThread::tid(), this, name_.c_str());
    }

    const std::string& name() const { return name_; }
    void setName(const std::string& name) { name_ = name; }

private:
    std::string name_;
};

void threadFunc() {
    printf("tid=%d, %p name=%s\n",
            CurrentThread::tid(),
            Singleton<Test>::instance(),
            Singleton<Test>::instance()->name().c_str());
    Singleton<Test>::instance()->setName("only one, changed");
}

int main(int argc, char* argv[]) {
    Singleton<Test>::instance()->setName("only one");
    Thread t1(threadFunc);
    t1.start();
    t1.join();
    printf("tid=%d, %p name=%s\n",
            CurrentThread::tid(),
            Singleton<Test>::instance(),
            Singleton<Test>::instance()->name().c_str());

    return 0;
}
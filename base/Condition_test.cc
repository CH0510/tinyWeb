#include "src/MutexLock.hpp"
#include "src/Condition.hpp"
#include <list>
#include <unistd.h>

using namespace tinyWeb;

std::list<int> g_list;
MutexLock g_mutex;
Condition g_cond(g_mutex);

//生产者
void* producer(void* arg) {
    int index = 0;
    const int total = 10 * 1000;
    while (1)
    {
        {
            MutexLockGuard lock(g_mutex);
            g_list.push_back(index);
        }
        index = (index + 1) % total;
        printf("producer push_back: %d\n", index);
    }
}

//消费者
void* consumer(void* arg) {
    while (1) {
        int index;
        {
            MutexLockGuard lock(g_mutex);
            while (g_list.empty()) {
                g_cond.wait();
            }
            index = g_list.front();
            g_list.pop_front();
        }
        printf("consumer pop_front: %d\n", index);
    }
}

int main() {
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, producer, NULL);
    pthread_create(&thread2, NULL, consumer, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}
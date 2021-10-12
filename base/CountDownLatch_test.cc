#include "src/CountDownLatch.h"
#include <pthread.h>
#include <cstdio>

using namespace tinyWeb;

CountDownLatch g_latch(2);

void* threadFunc(void*) {
    printf("tid = %d, before usleep\n", CurrentThread::tid());
    usleep(100);
    printf("tid = %d, after usleep\n", CurrentThread::tid());
    g_latch.countDown();

    return NULL;
}


int main(int argc, char* argv[]) {
    printf("pid = %d, tid = %d\n", getpid(), CurrentThread::tid());
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, threadFunc, NULL);
    pthread_create(&thread2, NULL, threadFunc, NULL);

    g_latch.wait();
    printf("after 2 thread exit\n");

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    return 0;
}
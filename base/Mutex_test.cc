#include "src/MutexLock.hpp"
#include <unistd.h>

using namespace tinyWeb;

MutexLock g_mutex;
int g_count = 0;
const int total_count = 10 * 1000;

void* threadFunc1(void* argc) {
    for (int i = 0; i < total_count; i++) {
        printf("[Thread1] : %d\n", g_count);
        MutexLockGuard lock(g_mutex);
        int cur = g_count;
        cur++;
        g_count = cur;
    }

    return NULL;
}

void* threadFunc2(void* argc) {
    for (int i = 0; i < total_count; i++) {
        printf("[Thread2] : %d\n", g_count);
        MutexLockGuard lock(g_mutex);
        int cur = g_count;
        cur++;
        usleep(10);
        g_count = cur;
    }

    return NULL;
}

int main(int argc, char **argv) {
    pthread_t thread1, thread2;
    pthread_create(&thread1,NULL, threadFunc1, NULL);
    pthread_create(&thread2,NULL, threadFunc2, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}
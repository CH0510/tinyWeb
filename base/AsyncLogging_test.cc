#include "src/AsyncLogging.h"
#include "src/Logging.h"
#include "src/Timestamp.h"

#include <stdio.h>
#include <sys/resource.h>
#include <unistd.h>

#include <string>

using namespace tinyWeb;

AsyncLogging* g_AsyncLogging = NULL;

void asynOutput(const char* logline, size_t len) {
  g_AsyncLogging->append(logline, len);
}

off_t kRollSize = 500 * 1000 * 1000;

void bench(bool longLog) {
  setOutput(asynOutput);

  int cnt = 0;
  const int kBatch = 1000;
  std::string empty = "";
  std::string longStr(3000, 'X');
  longStr += " ";

  for (int t = 0; t < 30; ++t) {
    Timestamp start = Timestamp::now();
    for (int i = 0; i < kBatch; ++i) {
      LOG_INFO << "Hello 0123456789" << " abcdefghijklmnopqrstuvwxyz "
               << (longLog ? longStr : empty)
               << cnt;
      ++cnt;
    }
    
    Timestamp end = Timestamp::now();
    printf("%f us\n", timeDifference(end, start) * 100000 / kBatch);
    struct timespec ts = {0, 500 * 1000 * 1000};
    nanosleep(&ts, NULL);
  }
}

int main(int argc, char* argv[]) {
  {
    // set virtual memory to 2GB
    size_t kOneG = 1000 * 1024 * 1024;
    rlimit rl = { 2 * kOneG, 2 * kOneG };
    setrlimit(RLIMIT_AS, &rl);
  }

  printf("pid = %d\n", getpid());
  char name[256] = {0};
  strncpy(name, argv[0], sizeof name - 1);
  AsyncLogging log(::basename(name), kRollSize);
  log.start();
  g_AsyncLogging = &log;

  bool longLog = argc > 1;
  bench(longLog);

  return 0;
}
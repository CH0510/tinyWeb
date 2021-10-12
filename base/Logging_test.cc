#include "src/Logging.h"

#include <stdio.h>
#include <unistd.h>
#include <string>
#include "src/Logging.h"
#include "src/LogFile.h"

#include <stdio.h>
#include <unistd.h>

#include "src/CountDownLatch.h"
#include "src/Thread.h"

using namespace tinyWeb;
using namespace std;

int g_total;
FILE* g_file;
std::unique_ptr<LogFile> g_logFile;

void dummyOutput(const char* msg, int len)
{
  g_total += len;
  if (g_file)
  {
    fwrite(msg, 1, len, g_file);
  }
  else if (g_logFile)
  {
    g_logFile->append(msg, len);
  }
}

void bench(const char* type)
{
  setOutput(dummyOutput);
  Timestamp start(Timestamp::now());
  g_total = 0;

  int n = 1000*1000;
  const bool kLongLog = false;
  string empty = " ";
  string longStr(3000, 'X');
  longStr += " ";
  for (int i = 0; i < n; ++i)
  {
    LOG_INFO << "Hello 0123456789" << " abcdefghijklmnopqrstuvwxyz"
             << (kLongLog ? longStr : empty)
             << i;
  }
  Timestamp end(Timestamp::now());
  double seconds = timeDifference(end, start);
  printf("%12s: %f seconds, %d bytes, %10.2f msg/s, %.2f MiB/s\n",
         type, seconds, g_total, n / seconds, g_total / seconds / (1024 * 1024));
}

CountDownLatch g_latch(1);

void logInThread()
{
  g_latch.wait();
  LOG_INFO << "logInThread";
  usleep(1000);
}

void Thread_test() {
  Thread t1(logInThread);
  Thread t2(logInThread);
  Thread t3(logInThread);
  Thread t4(logInThread);
  Thread t5(logInThread);
  t1.start();
  t2.start();
  t3.start();
  t4.start();
  t5.start();

  g_latch.countDown();
  t1.join();
  t2.join();
  t3.join();
  t4.join();
  t5.join();
}

int main()
{
  getppid(); // for ltrace and strace

  Thread_test();

  LOG_DEBUG << "debug";
  LOG_INFO << "Hello";
  LOG_WARN << "World";
  LOG_ERROR << "Error";
  LOG_INFO << sizeof(Logger);
  LOG_INFO << sizeof(LogStream);
  LOG_INFO << sizeof(LogStream::Buffer);

  sleep(1);
  bench("nop");

  char buffer[64*1024];

  g_file = fopen("/dev/null", "w");
  setbuffer(g_file, buffer, sizeof buffer);
  bench("/dev/null");
  fclose(g_file);

  g_file = fopen("/tmp/log", "w");
  setbuffer(g_file, buffer, sizeof buffer);
  bench("/tmp/log");
  fclose(g_file);

  g_file = NULL;
  g_logFile.reset(new LogFile("test_log_st", 500*1000*1000, false));
  bench("test_log_st");

  g_logFile.reset(new LogFile("test_log_mt", 500*1000*1000, true));
  bench("test_log_mt");
  g_logFile.reset();
}


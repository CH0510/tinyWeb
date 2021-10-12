#include "LogFile.h"

#include <unistd.h>
#include <assert.h>
#include <time.h>

namespace tinyWeb {

namespace detail {
std::string hostname() {
  char buf[256] = {0};
  if (::gethostname(buf, sizeof(buf)) == 0) {
    return buf;
  } else {
    return "unknow host";
  }
}

}   // namespace detail

LogFile::LogFile(const std::string& basename, \
                  size_t rollSize, \
                  bool threadSafe, \
                  size_t flushEveryN, \
                  size_t flushInterval) \
  : basename_(basename), rollSize_(rollSize), \
    checkEveryN_(flushEveryN), flushInterval_(flushInterval), \
    count_(0), lastRollingTime_(0), lastFlush_(0), \
    startOfPeriod_(0), mutex_(threadSafe ? new MutexLock : NULL) {
  assert(basename_.find('/') == std::string::npos);
  rollingFile();
}

void LogFile::append(const char* logline, size_t len) {
  if (mutex_) {
    MutexLockGuard lock(*mutex_);
    append_unlock(logline, len);
  } else {
    append_unlock(logline, len);
  }
}

void LogFile::flush() {
  if (mutex_) {
    MutexLockGuard lock(*mutex_);
    file_->flush();
  } else {
    file_->flush();
  }
}

void LogFile::rollingFile() {
  time_t now = 0;
  std::string filename = getFileName(basename_, &now);
  lastRollingTime_ = now;
  lastFlush_ = now;
  startOfPeriod_ = now / kRollPerSeconds * kRollPerSeconds;
  file_.reset(new AppendFile(filename));
}

void LogFile::append_unlock(const char* logline, size_t len) {
  file_->append(logline, len);

  //判断是否滚动日志
  if (file_->writenBytes() > rollSize_) {   // 写到一定大小之后就更新
    rollingFile();
  } else {
    ++count_;
    if (count_ >= checkEveryN_) {   // 只有当写完相当数量的日志采取检查是否超时，保证效率
      count_ = 0;
      time_t now = ::time(NULL);
      time_t thisPeriod = now / kRollPerSeconds * kRollPerSeconds;
      if (thisPeriod != startOfPeriod_) {   // 新的一天则滚动新的日志文件
        rollingFile();
      } else if (now - lastFlush_ > static_cast<time_t>(flushInterval_)) {
        // 超过三秒则向目的地刷新日志
        lastFlush_ = now;
        file_->flush();
      }
    }
  }
}

std::string LogFile::getFileName(const std::string& basename, time_t* t) {
  std::string filename;
  filename.reserve(basename.size() + 64);
  // 日志的文件名
  filename = basename;

  // 日志文件的时间
  *t = time(NULL);
  struct tm tm_time;
  gmtime_r(t, &tm_time);
  char timebuf[32] = {0};
  strftime(timebuf, sizeof(timebuf), ".%Y%m%d-%H%M%S.", &tm_time);
  filename += timebuf;

  // 日志所属的主机名
  filename += detail::hostname();

  // 日志所属进程的pid
  char pidbuf[32] = {0};
  snprintf(pidbuf, sizeof(pidbuf), ".%d", getpid());
  filename += pidbuf;

  return filename;
}

}   // namespace tinyWeb

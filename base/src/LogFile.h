#pragma once

#include <string>
#include <memory>

#include "noncopyable.hpp"
#include "FileUtil.h"
#include "Timestamp.h"
#include "StringPiece.hpp"
#include "MutexLock.hpp"

namespace tinyWeb {

class LogFile : public noncopyable {
 public:
  LogFile(const std::string& basename, \
          size_t rollSize, \
          bool threadSafe = true, \
          size_t checkEveryN_ = 1024, \
          size_t flushInterval = 3);
  void append(const char* logline, size_t len);
  void flush();
  void rollingFile();

 private:
  void append_unlock(const char* logline, size_t len);

  static std::string getFileName(const std::string& basename, time_t* t);

  const std::string basename_;
  const size_t rollSize_;    // 日志文件的大小
  const size_t checkEveryN_;    // 刷新到文件的记录条数，默认1024
  const size_t flushInterval_;  // 刷新到文件的间隔事件，默认3s

  size_t count_;   // 已经添加的日志条数
  time_t lastRollingTime_;  // 上一次滚动的时间
  time_t lastFlush_;   // 上次刷新到日志文件的时间
  time_t startOfPeriod_;  // 当前日期（每天的初始时刻）

  std::unique_ptr<AppendFile> file_;
  std::unique_ptr<MutexLock> mutex_;

  static const size_t kRollPerSeconds = 60 * 60 * 24;
};
}   // namespace tinyWeb

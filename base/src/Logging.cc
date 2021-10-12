#include "Logging.h"

#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "CurrentThread.h"
#include "StringPiece.hpp"

namespace tinyWeb {
__thread char t_errorbuf[512];
__thread char t_time[64];
__thread time_t t_lastSecond;   // 1秒内的多次日志无需重复调用系统函数

Appender::Appender() {
  output_func_ = std::bind(&Appender::defaultOutput, this, \
                    std::placeholders::_1, std::placeholders::_2);
  flush_func_ = std::bind(&Appender::defaultFlush, this);
}

LogLevel initLogLevel() {
  if (::getenv("TINYWEB_LOG_DEBUG")) {
    return LogLevel::DEBUG;
  } else {
    return LogLevel::INFO;
  }
}

LogLevel g_logLevel = initLogLevel();

const char* LogLevelName[LogLevel::NUM_LOG_LEVELS] = {
  "DEBUG ",
  "INFO  ",
  "WRAN  ",
  "ERROR ",
  "FATAL "
};

const char* strerror_tl(int saveErrno) {
  return strerror_r(saveErrno, t_errorbuf, sizeof(t_errorbuf));
}

inline LogStream& operator<<(LogStream& s, const Logger::SourceFile& v) {
  s.append(v.data_, v.len_);
  return s;
}

// 日期  时间  微秒  线程tid  日志级别
// (errno即对应的信息) (函数名)   日志信息  源文件名：行号
Logger::Impl::Impl(LogLevel level, int savedErrno, const SourceFile& basename, \
  int line) : time_(Timestamp::now()), level_(level), line_(line), \
  basename_(basename) {
  formatTime();
  CurrentThread::tid();
  stream_ << StringPiece(CurrentThread::tidString(), \
             CurrentThread::tidStringLength());
  stream_ << StringPiece(LogLevelName[level], 6);
  if (savedErrno != 0) {
    stream_ << strerror_tl(savedErrno) << " (errno=" << savedErrno << ") ";
  }
}

void Logger::Impl::formatTime() {
  int64_t microSecondsSinceEpoch = time_.microSecondsSinceEpoch();
  time_t seconds = static_cast<time_t>(microSecondsSinceEpoch /
                                       Timestamp::kMcroSecondsPerSecond);
  int microseconds = static_cast<int>(microSecondsSinceEpoch %
                                      Timestamp::kMcroSecondsPerSecond);
  // 在同一秒的日志无需重新初始化出微秒之外的部分
  if (seconds != t_lastSecond) {
    t_lastSecond = seconds;
    struct tm tm_time;
    ::gmtime_r(&seconds, &tm_time);

    int len  = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d", \
        tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday, \
        tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    assert(len == 17); (void)len;
  }

  char microsecondsBuf[64] = {0};
  snprintf(microsecondsBuf, sizeof(microsecondsBuf), ".%06dZ ", microseconds);
  stream_ << StringPiece(t_time, 17) << microsecondsBuf;
}

void Logger::Impl::finish() {
  stream_ << " - " << basename_ << ":" << line_ << "\n";
}

Logger::Logger(SourceFile file, int line) \
  : impl_(INFO, 0, file, line) {
}

Logger::Logger(SourceFile file, int line, LogLevel level) \
  : impl_(level, 0, file, line) {
}

Logger::Logger(SourceFile file, int line, LogLevel level, const char* func) \
  : impl_(level, 0, file, line) {
  impl_.stream() << func << " ";
}

Logger::Logger(SourceFile file, int line, bool toAbort) \
  : impl_(toAbort ? LogLevel::ERROR : LogLevel::FATAL, errno, file, line) {
}

Logger::~Logger() {
  impl_.finish();
  const LogStream::Buffer& buf(stream().buffer());
  Singleton<Appender>::instance()->output(buf.data(), buf.length());
  if (impl_.logLevel() == LogLevel::FATAL) {
    Singleton<Appender>::instance()->flush();
    ::abort();
  }
}

}   // namespace tinyWeb

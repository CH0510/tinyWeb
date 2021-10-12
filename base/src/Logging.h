#pragma once

#include <stdio.h>
#include <assert.h>

#include <functional>

#include "noncopyable.hpp"
#include "Timestamp.h"
#include "LogStream.h"
#include "Singleton.hpp"

namespace tinyWeb {
// 输出信息目的的封装，默认输出到终端
class Appender : public noncopyable {
 public:
  typedef std::function<void (const char*, size_t)> OutputFunc;
  typedef std::function<void ()> FlushFunc;

  Appender();

  void setOutput(OutputFunc output_func) {
    output_func_ = output_func;
  }

  void setFlush(FlushFunc flush_func) {
    flush_func_ = flush_func;
  }

  void output(const char* data, size_t len) {
    output_func_(data, len);
  }

  void flush() {
    flush_func_();
  }

 private:
  void defaultOutput(const char* data, size_t len) {
    size_t n = fwrite(data, 1, len, stdout);
    assert(n == len); (void)n;
  }

  void defaultFlush() {
    fflush(stdout);
  }

  OutputFunc output_func_;
  FlushFunc flush_func_;
};

inline void setOutput(Appender::OutputFunc output_func) {
  Singleton<Appender>::instance()->setOutput(output_func);
}

inline void setFlush(Appender::FlushFunc flush_func) {
  Singleton<Appender>::instance()->setFlush(flush_func);
}

// 日志的级别
enum LogLevel {
  DEBUG = 0,
  INFO,
  WARN,
  ERROR,
  FATAL,
  NUM_LOG_LEVELS
};

extern LogLevel g_logLevel;

inline LogLevel logLevel() {
  return g_logLevel;
}

inline void setLogLevel(LogLevel level) {
  g_logLevel = level;
}

class Logger : public noncopyable {
 public:
    //  负责格式化源文件
    struct SourceFile {
      const char* data_;
      size_t len_;

      template <int N>
      SourceFile(const char (&arr)[N]) \
        : data_(arr), len_(N - 1) {
        const char* slash = strrchr(data_, '/');
        if (slash) {
          data_ = slash + 1;
          len_ -= data_ - arr;
        }
      }

      explicit SourceFile(const char* filename) \
        : data_(filename) {
        const char* slash = strrchr(filename, '/');
        if (slash) {
          data_ = slash + 1;
        }
        len_ = strlen(data_);
      }
    };

  Logger(SourceFile file, int line);
  Logger(SourceFile file, int line, LogLevel level);
  Logger(SourceFile file, int line, LogLevel level, const char* func);
  Logger(SourceFile file, int line, bool toAbort);
  ~Logger();

  LogStream& stream() { return impl_.stream(); }

 private:
  class Impl : public noncopyable {
   public:
    //  内部实现类，负责日志数据的格式化
    Impl(LogLevel level, int old_errno, const SourceFile& file, int line);

    LogStream& stream() { return stream_; }
    LogLevel logLevel() const { return level_; }
    void setLogLevel(LogLevel level) {
      level_ = level;
    }

    void formatTime();
    void finish();

   private:
    Timestamp time_;
    LogStream stream_;
    LogLevel level_;
    int line_;
    SourceFile basename_;
  };

  Impl impl_;
};

// 宏定义，方便用户使用
#define LOG_DEBUG if (logLevel() <= LogLevel::DEBUG) \
  Logger(__FILE__, __LINE__, LogLevel::DEBUG, __func__).stream()
#define LOG_INFO if (logLevel() <= LogLevel::INFO) \
  Logger(__FILE__, __LINE__).stream()
#define LOG_WARN Logger(__FILE__, __LINE__, LogLevel::WARN).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, LogLevel::ERROR).stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, LogLevel::FATAL).stream()
#define LOG_SYSERROR Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL Logger(__FILE__, __LINE__, true).stream()

const char* strerror_tl(int saveErrno);

}   // namespace tinyWeb

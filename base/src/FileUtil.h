#pragma once

#include "noncopyable.hpp"
#include "StringPiece.hpp"

namespace tinyWeb {

class AppendFile {
 public:
  AppendFile(StringArg filename);

  ~AppendFile();

  void append(const char* logline, size_t len);

  void flush();

  size_t writenBytes() const { return writtenBytes_; }

 private:
  size_t write(const char* logline, size_t len);

  char buffer_[64 * 1024];
  FILE* fp_;
  size_t writtenBytes_;
};

}  // namespace tinyWeb

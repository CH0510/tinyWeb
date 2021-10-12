#pragma once
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include <string>

#include "StringPiece.hpp"
#include "noncopyable.hpp"
#include "Types.h"

namespace tinyWeb {

namespace detail {

const size_t kSmallBufferSize = 4 * 1024;
const size_t kLargeBufferSize = 4 * 1024 * 1024;

template <size_t SIZE>
class FixedBuffer : public noncopyable {
 public:
  FixedBuffer() : cur_(buf_) { }

  const char* data() const { return buf_; }
  char* current() { return cur_; }
  const char* end() const { return buf_ + SIZE; }
  size_t length() const { return cur_ - buf_; }
  size_t avail() const { return end() - cur_; }
  void add(size_t len) { cur_ += len; }
  const char* debugString() {
    *cur_ = '\0';
    return buf_;
  }

  void reset() { cur_ = buf_; }
  void bzero()  { ::memset(buf_, 0, sizeof(buf_)); }

  void append(const char* __restrict__ data, size_t len) {
    if (avail() > len) {
      ::memcpy(cur_, data, len);
      cur_ += len;
    }
  }

  std::string toString() const { return std::string(buf_, length()); }
  StringPiece toStringPiece() const { return StringPiece(buf_, length());}

 private:
  char buf_[SIZE];
  char* cur_;
};
}   // namespace detail

class LogStream : public noncopyable {
 public:
  typedef detail::FixedBuffer<detail::kSmallBufferSize> Buffer;
  typedef LogStream self;

  const Buffer& buffer() const { return buffer_; }
  void resetBuffer() { buffer_.reset(); }
  void append(const char* buf, size_t length) { buffer_.append(buf, length); }

  self& operator<<(bool v) {
    buffer_.append(v ? "1" : "0" , 1);
    return *this;
  }

  self& operator<<(char c) {
    buffer_.append(&c, 1);
    return *this;
  }

  self& operator<<(int16_t);
  self& operator<<(uint16_t);
  self& operator<<(int32_t);
  self& operator<<(uint32_t);
  self& operator<<(int64_t);
  self& operator<<(uint64_t);
  self& operator<<(long long);
  self& operator<<(unsigned long long);

  self& operator<<(const void*);

  self& operator<<(float v) {
    return operator<<(static_cast<double>(v));
  }

  self& operator<<(double);

  self& operator<<(const char* str) {
    if (str) {
      buffer_.append(str, strlen(str));
    } else {
      buffer_.append("(null)", 6);
    }

    return *this;
  }

  self& operator<<(const unsigned char* str) {
    return operator<<(reinterpret_cast<const char*>(str));
  }

  self& operator<<(const std::string& str) {
    buffer_.append(str.c_str(), str.size());
    return *this;
  }

  self& operator<<(StringPiece str) {
    buffer_.append(str.data(), str.size());
    return *this;
  }

  self& operator<<(const Buffer& v) {
    buffer_.append(v.data(), v.length());
    return *this;
  }

 private:
  void staticCheck();

  template <typename T>
  void formatInteger(T);

  Buffer buffer_;

  static const int kMaxBufferSize = 32;
};

}   // namespace tinyWeb

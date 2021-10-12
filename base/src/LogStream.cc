#include "LogStream.h"

#include <stdint.h>

#include <algorithm>
#include <limits>

namespace tinyWeb {
namespace detail {

const char digits[] = "9876543210123456789";
const char* zero = digits + 9;
static_assert(sizeof(digits) == 20, "wrong number of digits");

const char digitsHex[] = "0123456789ABCDEF";
static_assert(sizeof(digitsHex) == 17, "wrong number of digits");

// Efficient Integer to String Conversions, by Matthew Wilson.
// copy from muduo
template<typename T>
size_t convert(char buf[], T value) {
  T i = value;
  char* p = buf;

  do {
    int lsd = static_cast<int>(i % 10);
    i /= 10;
    *p++ = zero[lsd];
  } while (i != 0);

  if (value < 0) {
    *p++ = '-';
  }
  *p = '\0';
  std::reverse(buf, p);

  return p - buf;
}

size_t convertHex(char buf[], uintptr_t value) {
  uintptr_t i = value;
  char* p = buf;

  do {
    int lsd = static_cast<int>(i % 16);
    i /= 16;
    *p++ = digitsHex[lsd];
  } while (i != 0);

  *p = '\0';
  std::reverse(buf, p);

  return p - buf;
}

template class FixedBuffer<kSmallBufferSize>;
template class FixedBuffer<kLargeBufferSize>;
}  // namespace detail

void LogStream::staticCheck() {
  static_assert(kMaxBufferSize - 10 > \
                std::numeric_limits<double>::digits10, \
                "kMaxBufferSize is large enough");
  static_assert(kMaxBufferSize - 10 > \
                std::numeric_limits<long double>::digits10, \
                "kMaxBufferSize is large enough");
  static_assert(kMaxBufferSize - 10 > \
                std::numeric_limits<int64_t>::digits10, \
                "kMaxBufferSize is large enough");
  static_assert(kMaxBufferSize - 10 > \
                std::numeric_limits<long long>::digits10, \
                "kMaxBufferSize is large enough");
}

template <typename T>
void LogStream::formatInteger(T v) {
  if (buffer_.avail() >= kMaxBufferSize) {
    size_t len = detail::convert(buffer_.current(), v);
    buffer_.add(len);
  }
}

LogStream& LogStream::operator<<(int16_t v) {
  return operator<<(static_cast<int>(v));
}

LogStream& LogStream::operator<<(uint16_t v) {
  return operator<<(static_cast<uint32_t>(v));
}

LogStream& LogStream::operator<<(int32_t v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(uint32_t v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(int64_t v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(uint64_t v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(long long v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned long long v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(const void* p) {
  uintptr_t v = reinterpret_cast<uintptr_t>(p);
  if (buffer_.avail() >= kMaxBufferSize) {
    char* buf = buffer_.current();
    buf[0] = '0';
    buf[1] = 'x';
    size_t len = detail::convertHex(buf + 2, v);
    buffer_.add(len + 2);
  }

  return *this;
}

LogStream& LogStream::operator<<(double v) {
  if (buffer_.avail() >= kMaxBufferSize) {
    int len = snprintf(buffer_.current(), kMaxBufferSize, "%.12g", v);
    buffer_.add(len);
  }

  return *this;
}

}  // namespace tinyWeb

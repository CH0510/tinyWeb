#include "Timestamp.h"

#include <sys/time.h>
#include <time.h>
#include <inttypes.h>

#include "Types.h"

namespace tinyWeb {

Timestamp Timestamp::now() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return Timestamp(static_cast<int64_t>(tv.tv_sec) * \
                    kMcroSecondsPerSecond + tv.tv_usec);
}

std::string Timestamp::toString() const {
  char buf[32] = {0};
  int64_t seconds = microSecondsSinceEpoch_ / kMcroSecondsPerSecond;
  int64_t microseconds = microSecondsSinceEpoch_ % kMcroSecondsPerSecond;
  snprintf(buf, sizeof(buf), "%" PRId64 ".%06" PRId64 "", \
        seconds, microseconds);
  return buf;
}

std::string Timestamp::toUTCFormatString(bool showMcroSeconds) const {
  char buf[64] = {0};
  time_t seconds = static_cast<time_t>( \
                    microSecondsSinceEpoch_ / kMcroSecondsPerSecond);
  struct tm tm_time;
  gmtime_r(&seconds, &tm_time);

  if (showMcroSeconds) {
    int microseconds = static_cast<int>( \
                        microSecondsSinceEpoch_ % kMcroSecondsPerSecond);
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d", \
        tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday, \
        tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec, microseconds);
  } else {
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d", \
        tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday, \
        tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
  }

  return buf;
}

std::string Timestamp::toLocalFormatString(bool showMicroseconds) const {
  char buf[64] = {0};
  time_t seconds = static_cast<time_t>( \
                    microSecondsSinceEpoch_ / kMcroSecondsPerSecond);
  struct tm tm_time;
  localtime_r(&seconds, &tm_time);

  if (showMicroseconds) {
    int microseconds = static_cast<int>( \
                        microSecondsSinceEpoch_ % kMcroSecondsPerSecond);
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d", \
                tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday, \
                tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec, microseconds);
  } else {
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d", \
                tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday, \
                tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
  }

  return buf;
}

}  // namespace tinyWeb

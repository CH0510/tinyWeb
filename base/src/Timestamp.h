#pragma once

#include <stdint.h>

#include <utility>
#include <string>

namespace tinyWeb {

class Timestamp {
 public:
  static const int kMcroSecondsPerSecond = 1000 * 1000;

  Timestamp() : microSecondsSinceEpoch_(0) {
  }

  explicit Timestamp(int64_t microSecondsSinceEpoch) \
    : microSecondsSinceEpoch_(microSecondsSinceEpoch) {
  }

  bool valid() const { return microSecondsSinceEpoch_ > 0; }

  int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }

  time_t secondsSinceEpoch() const {
    return microSecondsSinceEpoch_ / kMcroSecondsPerSecond;
  }

  void swap(Timestamp& other) {
    std::swap(microSecondsSinceEpoch_, other.microSecondsSinceEpoch_);
  }

  static Timestamp now();

  static Timestamp fromUnixTime(time_t t) {
    return fromUnixTime(t, 0);
  }

  static Timestamp fromUnixTime(time_t t, int microseconds) {
    return Timestamp(static_cast<int64_t>(t) * \
                    kMcroSecondsPerSecond + microseconds);
  }

  std::string toString() const;
  std::string toUTCFormatString(bool showMicroseconds = true) const;
  std::string toLocalFormatString(bool showMicroseconds = true) const;

 private:
  int64_t microSecondsSinceEpoch_;
};

inline bool operator==(Timestamp lhs, Timestamp rhs) {
  return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

inline bool operator<(Timestamp lhs, Timestamp rhs) {
  return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline double timeDifference(Timestamp end, Timestamp start) {
  int64_t diff = end.microSecondsSinceEpoch() - start.microSecondsSinceEpoch();
  return static_cast<double>(diff) / Timestamp::kMcroSecondsPerSecond;
}

inline Timestamp addTime(Timestamp timestamp, double seconds) {
  int64_t delta = static_cast<int64_t>(seconds * \
                    Timestamp::kMcroSecondsPerSecond);
  return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}

}  // namespace tinyWeb

// @Author Hao Chen
// @Email chenhao980510@163.com
#pragma once

namespace tinyWeb {
class noncopyable {
 protected:
  noncopyable() { }
  noncopyable(const noncopyable&) = delete;
  noncopyable& operator=(const noncopyable&) = delete;
  ~noncopyable() { }
};
}  // namespace tinyWeb

# AsynLogging设计细节
- 采用swap on write技巧缩短临界区的大小
- cond_.waitForSeconds异常唤醒，显示错误EINVAL,经查询错误原因是：
  The abstime argument specified a nanosecond value less than zero or greater than or equal to 1000 million.
而Cond::waitForSeconds的封装存在问题，wait_time += nanosecond % kNonSecondsPerSecond,应为wait_time = nanosecond % kNonSecondsPerSecond
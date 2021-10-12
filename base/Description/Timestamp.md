# Timestamp实现中的细节
- time_t、int64_t和int混淆使用，没有注意溢出问题导致程序出bug。典型的比如Timestamp::now()的实现中，直接将tv.tv_sec直接乘kMicroSecondsPerSecond，应该现将其转化为int64_t防止乘完后数值溢出
- 没有理解gmttime_r、localtime_r的返回值，使用MCHECK进行了检查，当然回core dump
- 没有理解struct timeval结构体中个字段的含义，错把tm_yday当做了tm_year。
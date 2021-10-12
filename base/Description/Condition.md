# Condition实现记录
    对Condition的封装保持与直接使用Cindition一致，即
    - 其依赖于MutexLock，但只是持有引用，而非拥有。因此类中的使用方法是成员函数部分必须将MutexLock声明在前，再声明Condtion（因为C++初始化成员变量顺序只跟声明顺序有关的原因）
    - RAII手法封装初始化与销毁,并且为了防止发生“时间跳跃”，使用CLOCK_MONOTONIC时间
    - 其他调用只是对系统调用的封装，保证实际使用与直接使用条件变量规则相同,比如在调用wait()之前必须现将其所拥有的MutexLock上锁（MutexLockGuard lock(mutex)）
    - pthread_cond_timewait()中的第二参数是绝对时间,并且使用CLOCK_MONOTONIC时间。
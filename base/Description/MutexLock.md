# 实现Mutex.hpp的一些说明

## MutexLock的实现
    提供基础的功能，初始化锁、上锁、解锁、销毁锁。注意的是客户不应直接使用lock() 和 unlock()，而是由MutexLockGuard来管理。所以正确的使用方法是，在需要用锁保护的类中含有一个MutexLock的成员变量，然后在需要锁的地方使用MutexLockGuard进行管理。为了防止使用错误，将MutexLock的lock和unlock设为私有，不提供客户端访问权限

## MutexLockGuard的实现
    在构造函数和析构函数中实现加锁、解锁的操作。真正的加锁解锁应由该类来管理，这样可以避免异常等无法释放锁的情况。

## MutexLock的内部类 UnassignHolder
- 问题：
  其是为了实现在信号量的调用过程中由于调用pthread_cond_wait而导致的虽然释放了锁，但是没有更改对应MutexLock的holder,这样及时从pthread_cond_wait中唤醒，其holder可能已经发生改变，导致当前holder与现在线程的tid不同。

- 解决方法：
  是在调用pthread_cond_wait前将holder置为0（因为pthread_cond_wait会自动释放锁），并在pthread_cond_wait被唤醒之后重新设置holder为当前线程tid。UnassignedHolder就是对这一过程的封装，利用构造/析构函数的自动调用的特性。故只需要在调用pthread_cond_wait前声明一个UnassignHolder实例即可。
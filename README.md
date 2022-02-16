# tinyWeb
- 采用one loop per thread + thread pool 模型，主线程只负责accept请求，并以Round Robin的方式分发给其它IO线程，同一TCP请求只在固定线程中处理，降低锁的争用。
  并且可以创建计算线程池，降低I/O线程阻塞时间，提高响应速度。
- 采用Linux的epoll实现高效IO多路复用技术，采用非阻塞IO，Reactor模式，并统一事件源
- 使用多线程充分利用多核CPU、并使用线程池避免线程频繁创建与销毁的开销
- 使用红黑树管理定时器，方便定时器的删除
- 使用双缓冲区技术实现了简单的异步日志系统
- 使用智能指针防止内存泄露，并实现多线程共享对象的安全析构
- 支持优雅的关闭连接

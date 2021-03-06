# tinyWeb
(注：该项目为熟悉基于 C++ 语言的网络编程所创建，其中参考了 muduo 网络库的实现)，特点如下：
1. 采用one loop per thread + thread pool 模型，主线程只负责accept请求，并以Round Robin的方式分发给其它IO线程，同一TCP请求只在固定线程中处理，降低锁的争用。
  并且可以创建计算线程池，降低I/O线程阻塞时间，提高响应速度。
2. 采用Linux的epoll实现高效IO多路复用技术，采用非阻塞IO，Reactor模式，并统一事件源。
3. 使用多线程充分利用多核CPU、并使用线程池避免线程频繁创建与销毁的开销。
4. 使用红黑树管理定时器，方便定时器的删除。
5. 使用双缓冲区技术实现了简单的异步日志系统。
6. 使用智能指针防止内存泄露，并实现多线程共享对象的安全析构。
7. 支持优雅的关闭连接。

待添加/改进：
- 使用索引优先队列替换红黑树管理定时器
- 优化任务队列，使用linux提供的CAS机制实现无锁队列

注："UML类图" 文件夹中是使用StarUML软件绘制了项目开发过程中各个模块中的UML类图，以及一些Markdown文件记录设计类时的相关信息。

参考书目：
- 《Linux多线程服务器编程-使用muduo C++ 网络库》
- 《Linux高性能服务器编程》
- 《Unix网络编程 卷1》
- 《Unix环境高级编程》
- 《后台开发-核心技术与应用实践》
- 。。。

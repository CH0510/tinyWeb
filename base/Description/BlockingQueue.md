# BlockingQueue实现细节
- 条件变量的通知使用notify()而不能使用notifyAll(),因为这是资源可用的信号，使用后者将导致大量的虚假唤醒
- 注意getTask()中使用std::move()将队列里的值提取出来,否则无法试用与std::unique_ptr类的数据
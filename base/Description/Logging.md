# Logging设计细节
- 存在的类：
  - Appender：日志输出目的地的封装类，默认输出到终端，可以进行设置（在程序的配置期间，运行期间不能改变（线程不安全的））
  - Logger：负责向用户提供接口的类，大部分工作交给Impl类实现，只在析构函数中将已经格式化的日志信息利用Appender类实例进行输出。
  - Impl：Logger的内部实现类，负责格式化时间等日志信息，其内部包含一个LogStream用于保存已格式化的日志信息
  - LogStream：对日志输出流的封装，重载对各种类型的插入运算符已对客户提供类似std::cout等类似的刘插入操作
  - FixedBuffer：对缓存区buffer操作的封装，默认大小64K

- 使用 __restrict__ 修饰指针变量提升效率
- 在格式化事件的函数formatTime()中，对于1s之内的数据可以不再格式化秒级别及以上的数据，因此可以对该部分数据进行缓存，以提高格式化事件的效率，注意这部分数据如果不进行特殊修饰将变成线程共享数据，如果采用所机制将使得效率大大下降，直接用__thread修饰即可。对于errno的值同理

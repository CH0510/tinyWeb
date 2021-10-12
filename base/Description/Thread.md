# Thread封装细节
- 使用一个全局变量ThreadNameInitializer init来初始化主线程的的线程名
- 使用多个bool值提供错误检查
- 在Thread时如果线程没有退出，则detach,让其自行回收资源
- 如果Thread变量过早的结束，可在析构函数中将线程的状态设置为分离属性。（但这必须满足Thread变量生命期独立于线程的生命期，否则Thread变量提前销毁回导致线程崩溃）


## 设置ThreadData的目的:
- 首先如果在Thread中定义runInThread,name就必须保证Thread变量的生命期长于线程的生命期。抽取出一个ThreadData,它存放在堆上，就可以使得Thread变量的生命期独立于线程的生命期。
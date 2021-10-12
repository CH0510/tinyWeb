# ThreadLocal设计细节
- 该设计方式只支持单对象
- 采用 变参模板 + 完美转发 机制使得可以调用目标类型的带参构造函数
- 在destroy()函数中检查T是否是完整类型的方法，完整类型是指具有完整的定义。这是系统函数所要求的【调用destroy时，所保存的数据必须是完整的类型】


# 注意点（辨析）
- 使用Singleton<T>，并只使用Singleton<T>::instance()，则所获得的是同一个T类型的实例。
- 使用ThreadLocal<T>,只需将其看做一个正常的变量类型即可，所以同类型不同名的变量是不一样的。
- Singleton<ThreadLocal<T>>，并使用Single<ThreadLocal<T>>::instance()，则所获的是同一个ThreadLocal<T>类型的实例。
- 不能直接使用ThreadLocal<Singleton<T>>，因为ThreadLocal<T>的设计中需要new T,而Singleton的构造函数是私有的，因此必须新建一个类完成线程局部单例类的需求——即每个线程只能拥有一个T类型的变量
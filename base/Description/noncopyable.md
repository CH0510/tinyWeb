# noncopyable的实现
    构造、析构函数需实现在 protected 下，这样保证继承它的类能够顺利的构造
    拷贝赋值与拷贝构造函数使用 delete 语法禁用

特性,       语法示例,                       适用标准
定义参数包,  typename... Args,              C++11
获取包大小,  sizeof...(args),               C++11
递归展开,    模板函数 + 终止函数,            C++11
折叠表达式,  (... + args),                  C++17
完美转发,    std::forward<Args>(args)...,   C++11
继承展开,    struct D : Base<Args>...,      C++11  // Base<typename T> 和 Base<typename... T> 区别
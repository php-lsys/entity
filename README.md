数据记录实体类
========

> 主要用在数据库orm记录映射实体对象

1. 默认使用 composer require lsys/entity 进行php 源码版本安装

2. 如果追求最高执行效率,可以使用php扩展版本`只支持linux版本`,git clone 后如下安装并在php.ini添加引入:
```
cd ext
phpize && ./configure 
make && sudo make install
```

> 默认请使用composer 方式安装,当编译有扩展时,会优先使用扩展的实现,否则会用php源码版本.
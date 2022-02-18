# SYLAR-基于协程的C++高性能服务器框架



## 模块概述

### 日志模块

支持流式日志风格写日志和格式化风格写日志，支持日志格式自定义，日志级别，多日志分离等等功能。

流式日志使用：

```cpp
SYLAR_LOG_INFO(g_logger) << "this is a log";
```

格式化日志使用：

```cpp
SYLAR_LOG_FMT_INFO(g_logger, "%s", "this is a log"); 
```

日志支持自由配置日期时间，累计运行毫秒数，线程id，线程名称，协程id，日志线别，日志名称，文件名，行号。


### Util与Marco模块

工具接口与工具类，功能宏定义。包括获取时间，日期时间格式转换，栈回溯，文件系统操作接口，类型转换接口，以及SYLAR_ASSERT宏。详细接口参考util.h，macro.h。

### 环境变量模块

提供管理环境变量管理功能，包括系统环境变量，自定义环境变量，命令行参数，帮助信息，exe名称与程序路径相关的信息。环境变量全部以key-value的形式进行存储，key和value都是字符串格式。提供add/get/has/del接口用于操作自定义环境变量和命令行选项与参数，提供setEnv/getEnv用于操作系统环境变量，提供addHelp/removeHelp/printHelp用于操作帮忙信息，提供getExe/getCwd用于获取程序名称及程序路径，提供getAbsolutePath/getAbsoluteWorkPath/getConfigPath用于获取路径相关的信息。


### 配置模块

采用约定优于配置的思想。定义即可使用。支持变更通知功能。使用YAML文件做为配置内容，配置名称大小写不敏感。支持级别格式的数据类型，支持STL容器(vector,list,set,map等等),支持自定义类型的支持（需要实现序列化和反序列化方法)。

使用方式如下：

```cpp
static sylar::ConfigVar<int>::ptr g_tcp_connect_timeout = 
    sylar::Config::Lookup("tcp.connect.timeout", 5000, "tcp connect timeout");
```

定义了一个tcp连接超时参数，可以直接使用`g_tcp_connect_timeout->getValue()`获取参数的值，当配置修改重新加载，该值自动更新（并触发对应的值更新回调函数），上述配置格式如下：

```yaml
tcp:
    connect:
            timeout: 10000
```

### 线程模块

线程模块，封装了pthread里面的一些常用功能，Thread,Semaphore,Mutex,RWMutex等对象，可以方便开发中对线程日常使用。

线程模块相关的类：

`Thread`：线程类，构造函数传入线程入口函数和线程名称，线程入口函数类型为void()，如果带参数，则需要用std::bind进行绑定。线程类构造之后线程即开始运行，构造函数在线程真正开始运行之后返回。


### 协程模块

协程：用户态的线程，相当于线程中的线程，更轻量级。后续配置socket hook，可以把复杂的异步调用，封装成同步操作。降低业务逻辑的编写复杂度。 目前该协程是基于ucontext_t来实现的。


### 协程调度模块

协程调度器，管理协程的调度，内部实现为一个线程池，支持协程在多线程中切换，也可以指定协程在固定的线程中执行。是一个N-M的协程调度模型，N个线程，M个协程。重复利用每一个线程。

限制：  
一个线程只能有一个协程调度器
潜在问题：  
调度器在idle情况下会疯狂占用CPU，所以，创建了几个线程，就一定要有几个类似while(1)这样的协程参与调度。

### IO协程调度模块

继承自协程调度器，封装了epoll（Linux），支持注册socket fd事件回调。只支持读写事件。IO协程调度器解决了协程调度器在idle情况下CPU占用率高的问题，当调度器idle时，调度器会阻塞在epoll_wait上，当IO事件发生或添加了新调度任务时再返回。通过一对pipe fd来实现通知调度协程有新任务。

### 定时器模块

在IO协程调度器之上再增加定时器调度功能，也就是在指定超时时间结束之后执行回调函数。定时的实现机制是idle协程的epoll_wait超时，大体思路是创建定时器时指定超时时间和回调函数，然后以当前时间加上超时时间计算出超时的绝对时间点，然后所有的定时器按这个超时时间点排序，从最早的时间点开始取出超时时间作为idle协程的epoll_wait超时时间，epoll_wait超时结束时把所有已超时的定时器收集起来，执行它们的回调函数。

### Hook模块

hook系统底层和socket相关的API，socket io相关的API，以及sleep系列的API。hook的开启控制是线程粒度的。可以自由选择。通过hook模块，可以使一些不具异步功能的API，展现出异步的性能。如（mysql）

hook实际就是把系统提供的api再进行一层封装，以便于在执行真正的系统调用之前进行一些操作。hook的目的是把socket io相关的api都转成异步，以便于提高性能。hook和io调度是密切相关的，如果不使用IO协程调度器，那hook没有任何意义。

该框架对以下函数进行了hook，并且只对socket fd进行了hook，如果操作的不是socket fd，那会直接调用系统原本的api，而不是hook之后的api：  

```cpp
sleep
usleep
nanosleep
socket
connect
accept
read
readv
recv
recvfrom
recvmsg
write
writev
send
sendto
sendmsg
close
fcntl
ioctl
getsockopt
setsockopt
```


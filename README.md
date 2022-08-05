# CoServer-C++基于协程实现的Tcp/Http服务器



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

## **Example-TinyHttpServer**
---
基于上述高性能框架，实现一个简易**httpServer**——支持访问服务器数据库实现web端用户注册、登录功能，可以请求服务器图片和视频文件。  
服务器启动函数如下所示：  
```
void run() {
    sylar::http::HttpServer::ptr server(new sylar::http::HttpServer);
    sylar::Address::ptr addr = sylar::Address::LookupAnyIPAddress("0.0.0.0:8020");
    while(!server->bind(addr)) {
        sleep(2);
    }
    auto sd = server->getServletDispatch();
    
    sd->addServlet("/sylar", func_1); // func_1返回初始界面
    sd->addServlet("/sylar/register", func_2); // func_2返回注册界面
    sd->addServlet("/sylar/log", func_3); // func_3返回登录界面
    sd->addServlet("/check_register", func_4); // func_4 处理用户注册请求
    sd->addServlet("/check_log", func_5); // func_5 处理用户登录请求
    sd->addServlet("/picture/1", func_6); // func_6处理请求服务器图片请求
    sd->addServlet("/video/1", func_7); // func_7处理请求服务器视频文件请求

    sd->addGlobServlet("/*", func_1);

    server->start();
}
```
### Demo演示

> **注册**  

![Image text](https://github.com/liwentaoCH/sylar-server/blob/main/picture_gif/%E7%99%BB%E5%BD%95.gif)

> **登录**  

![Image text](https://github.com/liwentaoCH/sylar-server/blob/main/picture_gif/%E6%B3%A8%E5%86%8C%E7%94%A8%E6%88%B7.gif)

> **请求图片**
  
![Image text](https://github.com/liwentaoCH/sylar-server/blob/main/picture_gif/%E8%AF%B7%E6%B1%82%E5%9B%BE%E7%89%87%E6%96%87%E4%BB%B6.gif)

> **请求视频**
  
![Image text](https://github.com/liwentaoCH/sylar-server/blob/main/picture_gif/%E8%AF%B7%E6%B1%82%E8%A7%86%E9%A2%91%E6%96%87%E4%BB%B6.gif)


## 运行  
### 生成动态链接库libsylar.so
依据个人文件位置，修改CMakeList.txt文件中的相应输出文件路径，之后进入build文件夹，执行：
```shell
cmake ..
make
```
之后要编写服务器程序只需要新建一个文件，包含头文件**sylar.h**，并在编译时指定链接libsylar.so即可。

## 压力测试
使用jmeter压力测试工具，对xxxx:8020/sylar以及xxxx:8020/video/1两个接口进行压力测试。
### 硬件
处理器：AMD Ryzen7 4800H  四核  
内存： 4GB  
主频：2.9Hz  
### 软件
操作系统：CentOS 7.9(VMware虚拟机)
### 接口一：xxxx:8020/sylar（请求初始界面）
![Image text](https://github.com/liwentaoCH/sylar-server/blob/main/picture_gif/sylar.png)
**吞吐量为1642/sec**  

### 接口二：xxxx:8020/picture/1（请求图片）
![Image text](https://github.com/liwentaoCH/sylar-server/blob/main/picture_gif/video.png)
**吞吐量为1303/sec**  




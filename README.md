### 项目简介：
 - 基于Reactor并发模型开发的非阻塞IO服务器，采用了one loop per thread + 线程池的多线程编程模型，解析了get、head静态请求，支持优雅关闭连接,webbench压力测试,短连接QPS 2W+

### 技术点及工作：
- 使用Epoll水平触发的IO多路复用技术
- 使用多线程充分利用多核CPU，并使用线程池避免线程频繁创建销毁的开销
- 多处使用了智能指针等RAII机制来减少内存泄漏的可能
- 使用基于小根堆的定时器关闭超时请求
- 使用eventfd异步唤醒阻塞的IO线程

#### Webbench压力测试
#### 测试环境: 
- OS:ubuntu16.04
- 内存:8G
- CPU: i7-7700K

#### 测试方法:
- 使用工具Webbench，开启1000客户端进程，时间为1s和60s
- 线程数为４
#### 测试结果
1.  测试时长为１秒

![](https://upload-images.jianshu.io/upload_images/569253-80306c2dae149563.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

2.  测试时长为60秒

![](https://upload-images.jianshu.io/upload_images/569253-9c3fc921702656c8.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

#### cpu负载
- 空闲负载:

![cpu空闲负载.png](https://upload-images.jianshu.io/upload_images/569253-fa2cf21c801e37a4.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

- 连接负载:

![cpu负载.png](https://upload-images.jianshu.io/upload_images/569253-b978ff479a7f2959.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### 项目简介：
 - 基于Reactor并发模型开发的非阻塞IO服务器，采用了one loop per thread + 线程池的多线程编程模型，解析了get、head静态请求，支持优雅关闭连接。

<<<<<<< HEAD
### 技术点及工作：
- 使用Epoll水平触发的IO多路复用技术
- 使用多线程充分利用多核CPU，并使用线程池避免线程频繁创建销毁的开销
- 多处使用了智能指针等RAII机制来减少内存泄漏的可能
- 使用基于小根堆的定时器关闭超时请求
- 使用eventfd异步唤醒阻塞的IO线程

- #### Webbench压力测试
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
=======
- #### 使用了epoll来实现IO复用，并使用了水平触发模式
 1. select在高并发场景下效率过低,因为select是监听所有的待监听连接数，若待监听连接数远大于活跃的连接数，效率会显得额外低 。
 2. epoll维护了一个红黑树，其中保存了所有待监听的连接，另外有一个链表list存储了准备就绪的事件。
 3. epoll_wait(调用次数多)执行时检查一个由活跃列表所组成的链表，相对于select监听所有的待监听连接，效率自然会高出许多。

### LT
 1. level trigger不会丢失数据和消息
 2. 照顾了连接的公平性, 不会因为某个连接上数据量过大而影响其他连接处理.
 3. 会增加耗时
- ### 线程池
  设置固定数目(CPU核心数)的线程池，减少在高并发下线程的频繁创建与摧毁所产生的开销，提高程序的并发能力．
>>>>>>> 42a8a722f65de229aad14092c6a01489601abd91



<<<<<<< HEAD



=======
>>>>>>> 42a8a722f65de229aad14092c6a01489601abd91


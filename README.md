### 项目简介：
 - 基于Reactor并发模型开发的非阻塞IO服务器，采用了one loop per thread + 线程池的多线程编程模型，解析了get、head静态请求，支持优雅关闭连接。

- #### 使用了epoll来实现IO复用，并使用了水平触发模式
 1. select在高并发场景下效率过低,因为select是监听所有的待监听连接数，若待监听连接数远大于活跃的连接数，效率会显得额外低 。

 2. epoll维护了一个红黑树，其中保存了所有待监听的连接，其中节点的增加、减少都是通过epoll_ctl(调用次数少)来完成的，效率相对较高。
 3. epoll_wait(调用次数多)执行时检查一个由活跃列表所组成的链表，相对于select监听所有的待监听连接，效率自然会高出许多。
 4. level trigger不会丢失数据和消息

 5. 对于低延迟的服务器来说, 这样做是高效的, 每次读数据只需要1次系统调用

 6. 照顾了连接的公平性, 不会因为某个连接上数据量过大而影响其他连接处理.

 7. 使用edge trigger 在处理可读事件时, 至少需要read()两次
 8. 会增加耗时
- ### 线程池
  设置固定数目(CPU核心数)的线程池，减少在高并发下线程的频繁创建与摧毁所产生的开销，提高程序的并发能力．

- #### 定时器
1. 基于最小堆实现的定时器
    - 项目中使用了优先队列来实现基于最小堆的定时器
 
    - 指定时间执行删除操作，踢掉超时连接

- ### RAII
  在项目中的类之间的嵌套使用中，使用了__智能指针__做到线程安全的对象回调和析构

- #### Webbench压力测试
- 线程数为４
1.  短连接测试，测试时长为１秒
![shortRequests.png](https://upload-images.jianshu.io/upload_images/569253-fd7f8a42a6c7f183.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


2.  长连接测试，测试时长为１秒
![longRequests.png](https://upload-images.jianshu.io/upload_images/569253-88d9c214169b5624.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)



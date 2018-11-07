### 项目简介：

 - 基于Reactor模型的http服务器,利用了C++11的新特性,使用了多线程，通过创建固定大小的线程池来控制并发数量，并且得以提高并发性能。
 - #### 使用了epoll来实现IO复用
 1. select在高并发场景下效率过低,因为select是监听所有的待监听连接数，若待监听连接数远大于活跃的连接数，效率会显得额外低 。
 2. epoll维护了一个红黑树，其中保存了所有待监听的连接，其中节点的增加、减少都是通过epoll_ctl(调用次数少)来完成的。
 3. epoll_wait(调用次数多)执行时检查一个由活跃列表所组成的链表，相对于select监听所有的待监听连接，效率自然会高出许多。
- #### epoll使用level trigger模式
1. level trigger不会丢失数据和消息
2. 对于低延迟的服务器来说, 这样做是高效的, 每次读数据只需要1次系统调用
3. 照顾了连接的公平性, 不会因为某个连接上数据量过大而影响其他连接处理.
####edge trigger
1. 使用edge trigger 在处理可读事件时, 至少需要read()两次
2. 会增加耗时
- #### 定时器
1. 最小堆实现的定时器
    - 项目中使用了优先队列来实现基于最小堆的定时器（优先队列使用了最大最小堆这种数据结构）
    - 在堆中插入节点（addTimer）
    - 删除超时的节点（handleExpiredEvent）
2. 基于时间轮方式实现的定时器

- #### Channel类
1. 封装了事件I/O处理接口(read,write,connection, error),注册和响应I/O,隶属于一个EventLoop.
2. Channel不负责fd的生命周期,其生命周期由socket决定.
3. 
- #### Epoller类
1. 管理多个事件I/O的分发处理,因此Channel是它的成员变量
2. 封装了epoll的增删改(epoll_ctl)和轮询(epoll_wait --> poll)
- #### EventLoop类
1. mainEventLoop监听, 将事件分发到线程池中的等待队列来处理
2. 跨线程调用的实现(runInLoop)


- #### 多线程日志(TODO)
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;日志用于故障诊断和追踪。日志的消息分为多种级别，在运行时可调。日志LOG库分为**前端**和**后端**。前端负责往缓存当中写，后端负责把缓存中的内容写到文件当中。阅读了muduo的双缓存机制,通过在前端和后端分别设置两个缓冲区(1个接受写入和读出,另一个作为缓冲区不足的后备缓冲区 )

- #### Buffer类设计(TODO)
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;项目采用了非阻塞I/O模型,Buffer类的设计为了避免阻塞在read()和write()系统调用上.

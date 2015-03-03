# libevrpc
基于protobuf，线程池机制，libev封装，实现的RPC框架(现阶段只限于C++)

总体说明：

    1.在tp_rpc基础上进一步改进 去除epoll的裸使用 改用libev作为io_thread的代码核心， protobuf，线程池部分不变
    2.rpc下的connection_manager.h connection_manager.h为原来裸用epoll代码 现已经从rpc中分离开

编译说明：

    需要安装automake，并下载目录文件third-64,pub_util目录并放在libevrpc目录平行，然后在libevrpc下的build目录下 执行build.sh脚本


代码说明：

    1.rpc目录下：
        1）rpc_channel: 主要为提供客户端rpc访问，内部主要实现了socket 客户端实现
        2）rpc_server: 供rpc服务端使用，主要负责接口的启动以及调用逻辑的实现者，注意：内部的单例模式还没有实现线程访问安全模式
        3）io_thread: io线程用来接收客户端请求，接收请求递交到后面的线程池模块中的队列中
        4) libev_connector: libev接口的封装，实现对libev接口的调用，最后被io thread使用调用 
        5）thread_pool:线程池的实现，内部还用接收队列，以及线程池机制的封装实现
        6）rpc_msg.pb：rpc 客户端与服务端内部通信使用，主要定义通信协议 由信息头和信息主体组成，除非代码更改，所以勿要轻易重新生成
        7）socket_util：网络编程基础接口的封装
        8）rpc_util：rpc工具函数
        
    2.log：log日志调用，现阶段使用glog
    3.cs_sample: rpc使用C++实例

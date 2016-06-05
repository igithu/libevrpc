# libevrpc
基于protobuf，线程池机制，libev封装，实现的RPC框架(现阶段只限于C++)


原理说明

    1.现阶段使用memcached线程模型，实现IO接收机制和线程池机制，其中用libev代替libevent，
    2.客户端服务端通信序列化使用protobuf框架进行实现
    3.RPC添加心跳模式 保证客户端在崩溃，并且调用长调用的情况下，RPC服务端能够主动断掉浪费服务的调用


编译说明：

    需要安装automake，并下载目录文件third-64,pub_util目录并放在与libevrpc目录平行处，然后在libevrpc下目录下 执行build.sh脚本


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


设计说明：

        1.目前采用的是reactor + worker ThreadPool 模式，在实现的过程中，reactor(libev)只负责接收io接收转发
        2.注意：当前的实现只适合，插入数据比较短，逻辑比较简单系统使用，例如key value数据的读取（db_engine）
        3.考虑进一步加强功能，来适合大量数据的读写，以及计算密集型的要求: 模拟类似proactor做法，在reactor结构和worker ThreadPool之间增加ReaderPool，WriterPool，ReaderPool负责接收数据，来当数据接收完毕，将数据和fd移交下游的woker ThreadPool进行逻辑计算处理，逻辑计算处理之后门将产生的数据，移交给WritePool，将结果数据返回，这样基本实现了接收数据，产生数据和逻辑计算之间的异步处理实现
        
        

功能修补进展记录：

        1.关于客户端突然断掉连接容错，这里临时会采用TCP keepalive机制来代替应用层的心跳机制
        2.文件描述符重复问题，因为Linux文件描述符是顺序从小到大使用，短时间内是无法重复，如果需要应对，增加时间窗口fd set机制来判断

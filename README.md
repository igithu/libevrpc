# libevrpc


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
    2.cs_sample: rpc使用C++实例


设计说明：

        1.目前采用的是reactor + worker ThreadPool 模式，在实现的过程中，reactor(libev)只负责接收io接收转发
        2.
        
        

功能修补进展记录：

        1.

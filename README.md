# libevrpc


技术简述

    1.现阶段使用memcached线程池模型，实现IO接收机制和线程池机制（固定线程数），其中用libev代替libevent，
    2.客户端服务端通信序列化使用protobuf框架进行实现


编译说明：

    1.预备安装需要automake, gcc(4.8版本以上).
    2.拉去安装相关依赖，确保当前目录有root权限后，执行deps_build.sh，安装需要的依赖编译文件，如果出错，按照出错信息进行修复依赖错误
    3.安装好后，执行build.sh脚本 需要参数，命令示例： sh build.sh --prefix=/home/xxx/lib/libevrpc  (确保目录有权限读写，否则会出现错误)
    4.执行 make install 安装链接文件到指定目录
    5.执行sh build.sh clean 清理编译文件


代码说明：

    1.config_parser：配置文件解析
    2.cs_sample: RPC客户端和RPC服务端实现实例
    3.rpc_client：RPC客户端调相关实现目录，代码文件如下
        1).rpc_client：对外实现RPC客户端接口，RPC客户端使用者实现RPC功能时，需要继承RpcClient后 实现自己的Rpc客户端逻辑
        2).rpc_channel: 主要为提供客户端RPC访问，内部主要实现了socket 客户端实现
        3).rpc_heartbeat_client：RPC心跳客户端，实际为一个后台普通线程，定时没隔一定时间，想RPC服务器发送心跳
        4).client_rpc_controller：RPC客户端内部实现控制对象，现阶段只用于错误信息的输出
    4.rpc_server：RPC服务端调相关实现目录，代码文件如下：
        1).rpc_server: RPC服务端主程序实现，负责RPC服务端所有线程的启动和控制
        2).dispatch_thread:libev IO线程，负责监听和接受网络请求，将网络请求分发给后台的libev线程池当中，期间无任何CPU密集型处理逻辑
        3).libev_thread_pool：libev线程池.线程池中每个线程维护一个epoller，并负责RPC服务端逻辑的运行承载
        4).connection_timer_manager：管理链接计时器，对超时的链接进行销毁处理
        5).rpc_heartbeat_server：RPC服务端心跳线程，接受从客户端发送来的心跳，并将心跳信息更新到connection_timer_manager中
        6).server_rpc_controller：RPC服务端内部实现控制对象，现阶段只用于错误信息的输出
    5.unit_test：单元代码测试
    6.util：工具类和函数


设计说明：

     1.线程池每个线程维护一个epoller, IO每次请求都会向线程池中某个线程发送pipe信息，epoller线程开始处理
     2.RPC添加心跳模式 保证客户端在崩溃，并且调用长调用的情况下，RPC服务端能够主动断掉无用连接，避免浪费服务的资源
     3.因为采用线程池，一般线程数建议最佳线程数目 = ((线程等待时间+线程CPU时间)/线程CPU时间)*CPU数目，具体看RPC服务端的任务是IO密集型还是CPU密集型，IO密集型一般线程数偏小 减少或者避免线程上下文切换
     4.本设计暂时只提供P2P中 不包含负载均衡功能
   
 

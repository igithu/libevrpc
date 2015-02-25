# libevrpc
基于protobuf，线程池机制，libev封装，实现的RPC框架(现阶段只限于C++)

在tp_rpc基础上进一步改进 去除epoll的裸使用 改用libev作为io_thread的代码核心， protobuf，线程池部分不变

注意：rpc下的connection_manager.h connection_manager.h为原来裸用epoll代码 现已经从rpc中分离开

# libevrpc


技术简述

    1.现阶段使用memcached线程池模型，实现IO接收机制和线程池机制（固定线程数），其中用libev代替libevent
    2.客户端与服务端通信之间的框架与序列化， 使用protobuf框架进行实现


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
    5.rpc_center：rpc center功能实现代码 主要实现rpc server与rpc client之间的关联请求规则
    6.unit_test：单元代码测试
    7.util：工具类和函数


设计说明：

     1.线程池每个线程维护一个epoller, IO每次请求都会向线程池中某个线程发送pipe信息，epoller线程开始处理
     2.RPC添加心跳模式 保证客户端在崩溃，并且调用长调用的情况下，RPC服务端能够主动断掉无用连接，避免浪费服务的资源
     3.因为采用线程池，一般线程数建议最佳线程数目 = ((线程等待时间+线程CPU时间)/线程CPU时间)*CPU数目，具体看RPC服务端的任务是IO密集型还是CPU密集型，IO密集型一般线程数偏小 减少或者避免线程上下文切换
     4.本设计暂时只提供P2P中 不包含负载均衡功能


Rpc Center集群选举算法：为了协调Center集群信息数据一致性，实现负载均衡等功能，必须从集群中选举出Leader进行计算，最后同步计算结果到所有机器上，集群运行步骤如下（FastLeaderElection，网络失联情况待补充）

    1.RpcCenter集群中每台机器/tmp/centers_list下都会预先放置好 初始集群的机器列表
    2.集群启动，每个机器启动服务线程准备接受请求
    3.每台机器向其他机器询问信息（主要是启动时间等，同时同步本地Center 服务信息到其他Center服务上）
    4.收集到其他机器Center服务信息后，按照既定规则 选取出本地Center判断出的Leader，然后发起Proposal，同时启动选举线程
    5.每台机器会收到集群其他机器的Proposal，同时根据Proposal的信息进行判断，发起者建议的leader是否可以选举为leader
       Yes：选票记录在案，No：忽略其选票    最后更新当前发起者Center在本地的记录信息
    6.当选票过程中出现选票 超过集群机器数量的 2 / n + 1的时候
       如果当前机器为Leader，进入Leading状态，并广播确认自己的身份
       否则，进入Observering状态，等待Leader机器确认
    7.每台机器收到Leader机器的确认后，按照选举规则复查，是否与本地选取的出的leader机器比较，如本地尚未有选举结果，则直接通过
       通过，进入Following状态，广播宣布自己跟随当前Leader机器，选举线程退出
       未通过，发起新一轮选举，整个集群进入新一轮选举状态 回到步骤4，直到通过
    8.每台机器收到其他机器宣布的结果 。更新本地内存中 所有Center服务器相应信息，标示起跟随的Leader
    9.选举结束

Rpc Center集群LoadBalancer实现：

     1.RpcCenter集群正常启动完成, ConfigServe启动（管理RpcCenter机器列表等信息, 统一向RpcClient或者RpcServer提供全局服务器读取器信息）
     2.RpcServer Cluster机器启动与RpcCenter之间的心跳线程, 初始化过程中从ConfigServer读取RpcCenter机器列表, 然后随机从这些RpcCenter列表中, 选取一个Center地址与之通信
     3.接收到Rpc服务器的Center的根据CenterCluster统一的Hash算法计算出该Rpc服务机器应该向哪个(或哪些)Center汇报, 将结果返回该RpcServer服务器
     4.RpcServer得到对应汇报Center机器，记录到内存和磁盘上，然后周期向对应汇报Center服务器心跳，汇报本地发Load CPU等信息
     5.RpcCenter Cluster会根据所有RpcServer汇总上来的信息进行LoadBalancer计算(例如哪些虚节点与RpcServer对应)
     6.RpcClient启动， 从ConfigServer获取Center列表, 随机选取一个与之进行通信(或者与RpcServer类似，初始化获取Center机器最后确定与之服务的Center服务器)
     6.RpcCenter返回给RpcClient 对应的虚节点以及和虚节点下面对应的RpcServer服务器(选取虚节点遵循改虚节点对应RpcCenter为当时资源消耗最小原则)
     7.RpcClient获取RpcServer列表之后, 随机选取一个RPCServer, 开始运行P2P(点对点)模式 Rpc
     8.RpcClient 30s ~ 1分钟左右 重新执行6, 更新RpcServer信息



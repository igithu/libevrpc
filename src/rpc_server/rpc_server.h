/***************************************************************************
 *
 * Copyright (c) 2014 Aishuyu. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file rpc_server.h
 * @author aishuyu(asy5178@163.com)
 * @date 2014/11/23 16:49:42
 * @brief
 *
 **/




#ifndef  __RPC_SERVER_H_
#define  __RPC_SERVER_H_

#include <ext/hash_map>

#include <google/protobuf/service.h>

#include "dispatch_thread.h"
#include "connection_timer_manager.h"
#include "libev_thread_pool.h"
#include "center_cluster_heartbeat.h"
#include "config_parser/config_parser.h"
#include "util/disallow_copy_and_assign.h"
#include "util/pthread_mutex.h"
#include "util/pthread_rwlock.h"


namespace libevrpc {

using namespace google::protobuf;
using __gnu_cxx::hash_map;

struct RpcMethod {
    RpcMethod(Service* p_service,
              const Message* p_req,
              const Message* p_rep,
              const MethodDescriptor* p_meth)
        : service(p_service),
          request(p_req),
          response(p_rep),
          method(p_meth) {
    }

    Service* service;
    const Message* request;
    const Message* response;
    const MethodDescriptor* method;
};

typedef hash_map<uint32_t, RpcMethod*> HashMap;

// single thread
class RpcServer {
    public:
        ~RpcServer();

        static RpcServer& GetInstance();
        static RpcServer& GetInstance(const std::string& config_file);

        bool RegisteService(Service* reg_service);
        bool Start();
        bool Wait();

        /*
         * in fact, terminate the current thread, and new one thread replace it.
         */
        bool RestartWorkerThread(pthread_t thread_id, long running_version);

        static void RpcCall(int32_t event_fd, void *arg);
        static void* RpcProcessor(void *arg);
        static void* RpcReader(void *arg);
        static void* RpcWriter(void *arg);


    private:
        RpcServer(const std::string& config_file = "./rpc_server.ini");

        bool Initialize(const std::string& config_file);

        bool ErrorSendMsg(int32_t event_fd, const string& error_msg);

        DISALLOW_COPY_AND_ASSIGN(RpcServer);


    private:
        /*
         * RPC函数注册列表读写锁
         */
        RWLock hashmap_rwlock_;

        /*
         * RPC函数注册列表
         */
        HashMap method_hashmap_;

        std::string config_file_;

        DispatchThread*  dispatcher_thread_ptr_;
        LibevThreadPool* worker_threads_ptr_;
        LibevThreadPool* reader_threads_ptr_;
        LibevThreadPool* writer_threads_ptr_;
        RpcController* rpc_controller_ptr_;
        CenterClusterHeartbeat* center_cluster_heartbeat_ptr_;

        ConfigParser& config_parser_instance_;
        ConnectionTimerManager& connection_timer_manager_;

        int32_t active_wtd_num_;
        bool connection_timer_open_;

        struct CallBackParams {
            CallBackParams() :
                rpc_server_ptr(NULL),
                response_ptr(NULL){
            }

            ~CallBackParams() {
                if (NULL != response_ptr) {
                    delete response_ptr;
                }
            }

            int32_t event_fd;
            int32_t call_id;
            /*
             * current rpc server ptr
             */
            RpcServer* rpc_server_ptr;

            std::string recv_info;
            Message* response_ptr;
            RpcController* rpc_controller_ptr;
        };

};

}  // end of namespace libevrpc








#endif  //__RPC_SERVER_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

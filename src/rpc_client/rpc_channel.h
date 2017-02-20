/***************************************************************************
 *
 * Copyright (c) 2014 Aishuyu. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file rpc_channel.h
 * @author aishuyu(asy5178@163.com)
 * @date 2014/11/23 16:37:20
 * @brief
 *
 **/




#ifndef  __RPC_CHANNEL_H_
#define  __RPC_CHANNEL_H_

#include <ext/hash_map>

#include <string>
#include <vector>
#include <list>
#include <memory>
#include <unordered_map>

#include <google/protobuf/service.h>
#include <google/protobuf/message.h>

#include "center_client_heartbeat.h"
#include "util/pthread_rwlock.h"

namespace libevrpc {

using google::protobuf::Message;
using google::protobuf::Closure;
using google::protobuf::RpcChannel;
using google::protobuf::RpcController;
using google::protobuf::MethodDescriptor;

typedef std::list<pthread_t> TidList;
typedef std::shared_ptr<TidList> TidListPtr;
typedef std::unordered_map<uint32_t, TidListPtr> PthreadHashMap;
typedef std::unordered_map<pthread_t, std::string> MsgHashMap;


class Channel;

struct RpcCallParams {
    RpcCallParams(
            const std::string& method_full_name,
            const std::string& method_client_name,
            const Message* request,
            Message* response,
            Channel* channel) :
        method_full_name(method_full_name),
        method_client_name(method_client_name),
        p_request(request),
        p_response(response),
        p_channel(const_cast<Channel*>(channel)) {
        }

    const std::string method_full_name;
    const std::string method_client_name;
    const Message* p_request;

    std::string response_str;
    Message* p_response;
    Channel* p_channel;
};

class Channel : public RpcChannel {
    public:
        Channel(const char* addr, const char* port);
        /**
         * FOR server distribution mode
         */
        Channel(CenterClientHeartbeat* center_client_heartbeat_ptr, const char* port);

        virtual ~Channel();

        virtual void CallMethod(const MethodDescriptor* method,
                                RpcController* controller,
                                const Message* request,
                                Message* response,
                                Closure* done);

        bool OpenRpcAsyncMode();
        bool RpcCommunication(RpcCallParams* rpc_params);
        bool AsyncRpcCall(RpcCallParams* rpc_params_ptr);
        bool GetAsyncResponse(const std::string& method_name, Message* response);

        /*
         * try_time for timeout
         */
        void SetConnectionInfo(int32_t timeout, int32_t try_time = 1);
        void SetCallLimit(int32_t limit);

        static void* RpcProcessor(void *arg);

    private:
        char* addr_;
        char* port_;
        int32_t connect_fd_;

        bool is_channel_async_call_;
        int32_t call_limit_;

        int32_t tcp_conn_timeout_;
        /*
         * only for time out
         */
        int32_t try_time_;
        CenterClientHeartbeat* center_client_heartbeat_ptr_;
        std::string error_info_;

        std::vector<pthread_t> thread_ids_vec_;
        MsgHashMap call_results_map_;
        PthreadHashMap call_tids_map_;

        RWLock tids_map_rwlock_;
        RWLock ret_map_rwlock_;
};

}  // end of namespace libevrpc



#endif  //__RPC_CHANNEL_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

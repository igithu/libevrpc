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

#include <vector>
#include <unordered_map>

#include <google/protobuf/service.h>
#include <google/protobuf/message.h>

#include "pthread_rwlock.h"
#include "thread_pool.h"

namespace libevrpc {

using namespace google::protobuf;

typedef std::unordered_map<uint32_t, pthread_t> PthreadHashMap;
typedef std::unordered_map<pthread_t, Message*> MsgHashMap;


class Channel;

struct RpcCallParams {
    RpcCallParams(const string& method_name, const Message* request, Message* response, Channel* channel) :
        method_name(method_name), p_request(request), p_response(response), p_channel(const_cast<Channel*>(channel)) {
        }


    const std::string method_name;
    const Message* p_request;
    Message* p_response;
    Channel* p_channel;
};

class Channel : public RpcChannel {
    public:
        Channel(const char* addr, const char* port);

        virtual ~Channel();

        virtual void CallMethod(const MethodDescriptor* method,
                                RpcController* controll,
                                const Message* request,
                                Message* response,
                                Closure* done);

        bool OpenRpcAsyncMode(bool is_threadpool = false);

        bool RpcCommunication(RpcCallParams* rpc_params);

        bool AsyncRpcCall(RpcCallParams* rpc_params_ptr);

        static void* RpcProcessor(void *arg);

        bool GetAsyncCall(const std::string& method_name, Message* response);

        void Close();

    private:
        bool AsyncSingleThreadCall(RpcCallParams* rpc_params_ptr);

    private:
        char* addr_;

        char* port_;

        int32_t connect_fd_;

        bool is_channel_async_call_;

        ThreadPool* async_threads_ptr_;

        std::vector<pthread_t> thread_ids_vec_;

        MsgHashMap call_results_map_;

        PthreadHashMap call_tids_map_;

        PUBLIC_UTIL::RWLock tids_map_rwlock_;

};

}  // end of namespace libevrpc



#endif  //__RPC_CHANNEL_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

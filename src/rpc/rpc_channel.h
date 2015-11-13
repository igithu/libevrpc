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

#include <vector>

#include <google/protobuf/service.h>
#include <google/protobuf/message.h>

#include "thread_pool.h"

namespace libevrpc {

using namespace google::protobuf;

class Channel;

struct RpcCallParams {
    RpcCallParams(const MethodDescriptor* method, const Message* request, Message* response, Channel* channel) :
        p_method(method), p_request(request), p_response(response), p_channel(channel) {
        }
    const MethodDescriptor* p_method;
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

        void Close();

        bool OpenRpcAsyncMode(bool is_threadpool = false);

        bool RpcCommunication(RpcCallParams* rpc_params);

        bool AsyncRpcCall(RpcCallParams* rpc_params_ptr);

        static void* RpcProcessor(void *arg);

    private:
        bool AsyncSingleThreadCall(RpcCallParams* rpc_params_ptr);

    private:
        char* addr_;

        char* port_;

        int32_t connect_fd_;

        bool is_channel_async_call_;

        ThreadPool* async_threads_ptr_;

        std::vector<pthread_t> thread_ids_vec_;

};

}  // end of namespace libevrpc



#endif  //__RPC_CHANNEL_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

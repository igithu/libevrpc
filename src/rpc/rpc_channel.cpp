/***************************************************************************
 * 
 * Copyright (c) 2014 Aishuyu. All Rights Reserved
 * 
 **************************************************************************/



/**
 * @file rpc_channel.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2014/11/23 16:25:13
 * @brief 
 *
 **/


#include "rpc_channel.h"

#include <string>

#include "rpc_util.h"
#include "rpc_communication.h"

namespace libevrpc {

using std::string;
using std::vector;

Channel::Channel(const char* addr, const char* port) :
    is_channel_async_call_(false), async_threads_ptr_(NULL) {
    strcpy(addr_ = (char*)malloc(strlen(addr) + 1), addr);
    strcpy(port_ = (char*)malloc(strlen(port) + 1), port);

}

Channel::~Channel() {
    free(addr_);
    free(port_);

    if (NULL != async_threads_ptr_) {
        delete async_threads_ptr_;
    }

    if (!thread_ids_vec_.empty()) {
        for (int i = 0; i < thread_ids_vec_.size(); ++i) {
            pthread_join(thread_ids_vec_[i], NULL);
        }
    }
}

void Channel::CallMethod(const MethodDescriptor* method,
                         RpcController* control,
                         const Message* request,
                         Message* response,
                         Closure* done) {

    connect_fd_ = TcpConnect(addr_, port_);
    if (connect_fd_ < 0) {
        perror("Rpc connect server failed!");
        return;
    }

    RpcCallParams* rpc_params_ptr = new RpcCallParams(method, request, response, this);

    if (is_channel_async_call_) {
        AsyncRpcCall(rpc_params_ptr);
    } else {
        RpcCommunication(rpc_params_ptr);
        delete rpc_params_ptr;
    }
}

bool Channel::RpcCommunication(RpcCallParams* rpc_params) {
    const MethodDescriptor* method = rpc_params->p_method;
    const Message* request = rpc_params->p_request;
    Message* response = rpc_params->p_response;

    string send_str;
    if (!request->SerializeToString(&send_str)) {
        perror("SerializeToString request failed!");
        return false;
    }
    uint32_t hash_code = BKDRHash(method->full_name().c_str());
    if (RpcSend(connect_fd_, hash_code, send_str, false) < 0) {
        return false;
    }

    string recv_str;
    int32_t ret_id = RpcRecv(connect_fd_, recv_str, true);
    if (ERROR_RECV == ret_id) {
        perror("Recv data error in rpc channel");
        return false;
    }

    if (!response->ParseFromString(recv_str)) {
        perror("SerializeToString response error in RpcChannel!");
        // TODO
    }

    return true;
}

bool Channel::OpenRpcAsyncMode(bool is_threadpool) {
    is_channel_async_call_ = true;
    if (is_threadpool) {
        async_threads_ptr_ = new ThreadPool(5);
        async_threads_ptr_->Start();
    }
    return true;
}

bool Channel::AsyncRpcCall(RpcCallParams* rpc_params_ptr) {
    if (NULL == async_threads_ptr_) {
        // Single thread mode
        AsyncSingleThreadCall(rpc_params_ptr);
    } else {
        // Thread pool mode
        async_threads_ptr_->Processor(Channel::RpcProcessor, rpc_params_ptr);
    }
    return true;
}

bool Channel::AsyncSingleThreadCall(RpcCallParams* rpc_params_ptr) {
    pthread_t tid;
    pthread_create(&tid, NULL, Channel::RpcProcessor, rpc_params_ptr);
    thread_ids_vec_.push_back(tid);
    return true;
}

void* Channel::RpcProcessor(void *arg) {
    if (NULL == arg) {
        return NULL;
    }
    RpcCallParams* rpc_params_ptr = (RpcCallParams*) arg;
    RpcCommunication(rpc_params_ptr);
    delete rpc_params_ptr;
}

void Channel::Close() {
}





}  // end of namespace libevrpc



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

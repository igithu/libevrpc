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


    RpcCallParams* rpc_params_ptr = new RpcCallParams(method->full_name(), request, response, this);
    if (is_channel_async_call_) {
        AsyncRpcCall(rpc_params_ptr);
    } else {
        RpcCommunication(rpc_params_ptr);
        delete rpc_params_ptr;
    }
}

bool Channel::RpcCommunication(RpcCallParams* rpc_params) {
    const string& method_name = rpc_params->method_name;
    const Message* request = rpc_params->p_request;
    Message* response = rpc_params->p_response;

    string send_str;
    if (!request->SerializeToString(&send_str)) {
        perror("SerializeToString request failed!");
        return false;
    }
    uint32_t hash_code = BKDRHash(method_name.c_str());
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
    is_threadpool = false; // shutdown the thread pool
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
    uint32_t hash_code = BKDRHash(rpc_params_ptr->method_name.c_str());
    call_tids_map_.insert(std::make_pair(hash_code, tid));
    thread_ids_vec_.push_back(tid);
    return true;
}

void* Channel::RpcProcessor(void *arg) {
    if (NULL == arg) {
        return NULL;
    }
    RpcCallParams* rpc_params_ptr = (RpcCallParams*) arg;
    Channel* channel_ptr = rpc_params_ptr->p_channel;
    Message* response_ptr = rpc_params_ptr->p_response->New();
    channel_ptr->RpcCommunication(rpc_params_ptr);
    if (NULL != response_ptr) {
        MsgHashMap& ret_map = channel_ptr->call_results_map_;
        uint32_t hash_code = BKDRHash(rpc_params_ptr->method_name.c_str());
        MsgHashMap::iterator ret_iter = ret_map.find(hash_code);
        // pthread_t cur_tid = pthread_self();
        if (ret_iter == ret_map.end()) {
            ret_map.insert(std::make_pair(hash_code, response_ptr));
        } else {
            // if conflict, replace old one
            delete ret_iter->second;
            ret_map[hash_code] = response_ptr;
        }
    }
    delete rpc_params_ptr;
}

bool Channel::GetAsyncCall(const string& method_name, Message* response) {
    uint32_t method_code = BKDRHash(method_name.c_str());
    PthreadHashMap::iterator ret_iter = call_tids_map_.find(method_code);
    if (ret_iter == call_tids_map_.end()) {
        return false;
    }
    pthread_t& cur_tid = ret_iter->second;
    pthread_join(cur_tid, NULL);

    return true;
}

void Channel::Close() {
}





}  // end of namespace libevrpc



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

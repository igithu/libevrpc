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
    is_channel_async_call_(false) {
    strcpy(addr_ = (char*)malloc(strlen(addr) + 1), addr);
    strcpy(port_ = (char*)malloc(strlen(port) + 1), port);

    call_limit_ = 100;

}

Channel::~Channel() {
    free(addr_);
    free(port_);

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

bool Channel::OpenRpcAsyncMode() {
    is_channel_async_call_ = true;
    return true;
}

bool Channel::RpcCommunication(RpcCallParams* rpc_params) {
    const string& method_name = rpc_params->method_name;
    const Message* request = rpc_params->p_request;
    Message* response = rpc_params->p_response;
    if (NULL == response) {
        response->New();
    }

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

bool Channel::AsyncRpcCall(RpcCallParams* rpc_params_ptr) {
    AsyncSingleThreadCall(rpc_params_ptr);
    return true;
}

bool Channel::AsyncSingleThreadCall(RpcCallParams* rpc_params_ptr) {
    {
        ReadLockGuard rguard(tids_map_rwlock_);
        if (call_tids_map_.size() >= call_limit_) {
            return false;
        }
    }
    pthread_t tid;
    pthread_create(&tid, NULL, Channel::RpcProcessor, rpc_params_ptr);
    uint32_t hash_code = BKDRHash(rpc_params_ptr->method_name.c_str());
    {
        WriteLockGuard wguard(tids_map_rwlock_);
        PthreadHashMap::iterator ret_iter = call_tids_map_.find(hash_code);
        if (ret_iter == call_tids_map_.end()) {
            TidListPtr tids_ptr(new TidList());
            tids_ptr->push_back(tid);
            call_tids_map_.insert(std::make_pair(hash_code, tids_ptr));
        } else {
            ret_iter->second->push_back(tid);
        }
    }
    thread_ids_vec_.push_back(tid);
    return true;
}

bool Channel::GetAsyncResponse(const string& method_name, Message* response) {
    uint32_t method_code = BKDRHash(method_name.c_str());
    PthreadHashMap::iterator ret_iter;
    pthread_t cur_tid;
    {
        ReadLockGuard rguard(tids_map_rwlock_);
        ret_iter = call_tids_map_.find(method_code);
        if (call_tids_map_.end() == ret_iter || ret_iter->second->size() == 0) {
            return false;
        }
        cur_tid = ret_iter->second->front();
        ret_iter->second->pop_front();
    }
    pthread_join(cur_tid, NULL);
    Message* response_ptr = NULL;
    {
        WriteLockGuard wguard(ret_map_rwlock_);
        MsgHashMap::iterator msg_iter = call_results_map_.find(cur_tid);
        if (call_results_map_.end() == msg_iter) {
            return false;
        }
        response_ptr = msg_iter->second;
        call_results_map_.erase(msg_iter);
    }
    if (NULL == response_ptr) {
        return false;
    }
    response->CopyFrom(*response_ptr);
    delete response_ptr;

    return true;
}

void Channel::SetCallLimit(int32_t limit) {
    call_limit_ = limit;
}

void* Channel::RpcProcessor(void *arg) {
    if (NULL == arg) {
        return NULL;
    }
    RpcCallParams* rpc_params_ptr = (RpcCallParams*) arg;
    Channel* channel_ptr = rpc_params_ptr->p_channel;
    if (!channel_ptr->RpcCommunication(rpc_params_ptr)) {
        return NULL;
    }
    Message* response_ptr = rpc_params_ptr->p_response->New();
    pthread_t cur_tid = pthread_self();
    if (NULL != response_ptr) {
        MsgHashMap& ret_map = channel_ptr->call_results_map_;
        WriteLockGuard wguard(channel_ptr->ret_map_rwlock_);
        MsgHashMap::iterator ret_iter = ret_map.find(cur_tid);
        if (ret_map.end() == ret_iter) {
            ret_map.insert(std::make_pair(cur_tid, response_ptr));
        } else {
            // if conflict, replace old one
            delete ret_iter->second;
            ret_map[cur_tid] = response_ptr;
        }
    }
    delete rpc_params_ptr;
}

}  // end of namespace libevrpc



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

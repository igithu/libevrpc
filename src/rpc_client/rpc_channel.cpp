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

#include "util/rpc_util.h"
#include "util/rpc_communication.h"

namespace libevrpc {

using std::string;
using std::vector;

Channel::Channel(CenterClientHeartbeat* center_client_heartbeat_ptr, const char* port) :
    addr_(NULL),
    port_(NULL),
    is_channel_async_call_(false),
    call_limit_(100),
    tcp_conn_timeout_(1000),
    try_time_(1),
    center_client_heartbeat_ptr_(center_client_heartbeat_ptr) {

    strcpy(port_ = (char*)malloc(strlen(port) + 1), port);
}


Channel::Channel(const char* addr, const char* port) :
    is_channel_async_call_(false),
    call_limit_(100),
    tcp_conn_timeout_(1000),
    try_time_(1) {
    /**
     * copy the server addr and port when in single server
     */
    strcpy(addr_ = (char*)malloc(strlen(addr) + 1), addr);
    strcpy(port_ = (char*)malloc(strlen(port) + 1), port);
}

Channel::~Channel() {
    if (NULL != addr_) {
        free(addr_);
    }

    if (NULL != port_) {
        free(port_);
    }

    if (!thread_ids_vec_.empty()) {
        for (int i = 0; i < thread_ids_vec_.size(); ++i) {
            pthread_join(thread_ids_vec_[i], NULL);
        }
    }
}

void Channel::CallMethod(const MethodDescriptor* method,
                         RpcController* controller,
                         const Message* request,
                         Message* response,
                         Closure* done) {

    if (NULL == center_client_heartbeat_ptr_ &&
       (NULL == addr_ || NULL == port_)) {
        return;
    }

    int32_t try_times = try_time_;
    char* local_addr = NULL;
    if (NULL != center_client_heartbeat_ptr_) {
        local_addr = center_client_heartbeat_ptr_->RandomGetRpcServerAddr();
    } else {
        strcpy(local_addr = (char*)malloc(strlen(addr_) + 1), addr_);
    }
    do {
        connect_fd_ = TcpConnect(local_addr, port_, tcp_conn_timeout_);
        if (TCP_CONN_TIMEOUT != connect_fd_) {
            break;
        }
        --try_times;
        PrintErrorInfo("TcpConnect timeout! try again!");
    } while (try_times <= 0);

    if (NULL != local_addr) {
        free(local_addr);
    }

    if (connect_fd_ < 0) {
        if (NULL != controller) {
            controller->SetFailed("Rpc connect server failed!");
        }
        return;
    }

    RpcCallParams* rpc_params_ptr = new RpcCallParams(method->full_name(), method->name(), request, response, this);
    if (is_channel_async_call_) {
        AsyncRpcCall(rpc_params_ptr);
    } else {
        if (!RpcCommunication(rpc_params_ptr)) {
            if (NULL != controller) {
                controller->SetFailed(error_info_);
            }
            delete rpc_params_ptr;
            return;
        }
        if (!response->ParseFromString(rpc_params_ptr->response_str) && NULL != controller) {
            controller->SetFailed("SerializeToString response error in RpcChannel!");
            // TODO
        }
        delete rpc_params_ptr;
    }
}

bool Channel::OpenRpcAsyncMode() {
    is_channel_async_call_ = true;
    return true;
}

bool Channel::RpcCommunication(RpcCallParams* rpc_params) {
    const string& method_name = rpc_params->method_full_name;
    const Message* request = rpc_params->p_request;
    string& recv_str = rpc_params->response_str;
    Message* response = rpc_params->p_response;
    if (NULL == response) {
        response->New();
    }

    string send_str;
    if (!request->SerializeToString(&send_str)) {
        error_info_ = "SerializeToString request failed!";
        return false;
    }
    uint32_t hash_code = BKDRHash(method_name.c_str());
    if (RpcSend(connect_fd_, hash_code, send_str, false) < 0) {
        error_info_ = "Send data error in rpc channel.";
        return false;
    }

    int32_t ret_id = RpcRecv(connect_fd_, recv_str, true);
    if (ERROR_RECV == ret_id) {
        error_info_ = "Recv data error in rpc channel.";
        return false;
    }
    return true;
}

bool Channel::AsyncRpcCall(RpcCallParams* rpc_params_ptr) {
    {
        ReadLockGuard rguard(tids_map_rwlock_);
        if (call_tids_map_.size() >= call_limit_) {
            return false;
        }
    }
    pthread_t tid;
    pthread_create(&tid, NULL, Channel::RpcProcessor, rpc_params_ptr);
    uint32_t hash_code = BKDRHash(rpc_params_ptr->method_client_name.c_str());
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
            error_info_ = "Get the response list failed";
            return false;
        }
        cur_tid = ret_iter->second->front();
        ret_iter->second->pop_front();
    }
    pthread_join(cur_tid, NULL);
    {
        WriteLockGuard wguard(ret_map_rwlock_);
        MsgHashMap::iterator msg_iter = call_results_map_.find(cur_tid);
        if (call_results_map_.end() == msg_iter) {
            return false;
        }
        response->ParseFromString(msg_iter->second);
        call_results_map_.erase(msg_iter);
    }
    return true;
}

void Channel::SetConnectionInfo(int32_t timeout, int32_t try_time) {
    tcp_conn_timeout_ = timeout;
    try_time_ = try_time;
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
    string& response_str = rpc_params_ptr->response_str;
    pthread_t cur_tid = pthread_self();
    MsgHashMap& ret_map = channel_ptr->call_results_map_;
    {
        WriteLockGuard wguard(channel_ptr->ret_map_rwlock_);
        MsgHashMap::iterator ret_iter = ret_map.find(cur_tid);
        if (ret_map.end() == ret_iter) {
            ret_map.insert(std::make_pair(cur_tid, std::move(response_str)));
        } else {
            /*
             * if conflict, replace old one
             */
            ret_map[cur_tid] = std::move(response_str);
        }
    }
    delete rpc_params_ptr;
}

}  // end of namespace libevrpc



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

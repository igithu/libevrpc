/***************************************************************************
 *
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file rpc_client.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2015/09/10 20:08:23
 * @brief
 *
 **/

#include <rpc_client.h>

#include "client_rpc_controller.h"

namespace libevrpc {

using std::string;

RpcClient::RpcClient() :
    rpc_channel_ptr_(NULL),
    rpc_controller_ptr_(NULL) {
}

RpcClient::~RpcClient() {
    if (NULL != rpc_channel_ptr_) {
        delete rpc_channel_ptr_;
    }

    if (NULL != rpc_controller_ptr_) {
        delete rpc_controller_ptr_;
    }
}

bool RpcClient::InitClient(const char* addr, const char* port) {
    rpc_channel_ptr_ = new Channel(addr, port);
    rpc_controller_ptr_ = new ClientRpcController();
    SetRpcConnectionInfo(1000, 1);
    return true;
}

RpcController* RpcClient::Status() {
    return rpc_controller_ptr_;
}

bool RpcClient::IsCallOk() {
    if (NULL == rpc_controller_ptr_) {
        return true;
    }
    return !rpc_controller_ptr_->Failed();
}

string RpcClient::GetErrorInfo() const {
    if (NULL == rpc_controller_ptr_) {
        return "";
    }
    return rpc_controller_ptr_->ErrorText();
}

Channel* RpcClient::GetRpcChannel() {
    return rpc_channel_ptr_;
}

bool RpcClient::OpenRpcAsyncMode() {
    if (NULL == rpc_channel_ptr_) {
        perror("Maybe YOU DIDNOT call InitClient first! open the async failed!");
        exit(0);
    }
    rpc_channel_ptr_->OpenRpcAsyncMode();
    return true;
}

bool RpcClient::GetAsyncResponse(const string& method_name, Message* response) {
    if (NULL == rpc_channel_ptr_) {
        return false;
    }
    return rpc_channel_ptr_->GetAsyncResponse(method_name, response);
}

bool RpcClient::SetRpcConnectionInfo(int32_t rpc_timeout, int32_t try_time) {
    if (NULL == rpc_channel_ptr_) {
        return false;
    }
    rpc_channel_ptr_->SetConnectionInfo(rpc_timeout, try_time);
    return true;
}


}  // end of namespace libevrpc












/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

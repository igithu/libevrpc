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

namespace libevrpc {

RpcClient::RpcClient() :
    rpc_channel_ptr_(NULL), service_call_ptr_(NULL), is_async_(false) {
}

RpcClient::~RpcClient() {
    if (NULL != service_call_ptr_) {
        delete service_call_ptr_;
    }

    if (NULL != rpc_channel_ptr_) {
        delete rpc_channel_ptr_;
    }
}

bool RpcClient::InitClient(const char* addr, const char* port) {
    rpc_channel_ptr_ = new Channel(addr, port);
    // async mode?
    if (is_async_) {
        rpc_channel_ptr_->OpenRpcAsyncMode();
    }
    return true;
}

Channel* RpcClient::GetRpcChannel() {
    return rpc_channel_ptr_;
}

bool RpcClient::OpenRpcAsyncMode() {
    if (NULL == rpc_channel_ptr_ || NULL == service_call_ptr_) {
        perror("Maybe YOU DIDNOT call InitClient first! open the async failed!");
        return false;
    }
    is_async_ = true;
    return true;
}

void* RpcClient::ServerCall() {
    if (NULL == rpc_channel_ptr_ || NULL == service_call_ptr_) {
        perror("InitClient failed! Maybe YOU DIDNOT call InitClient first!");
        exit(0);
    }
    return service_call_ptr_;
}


}  // end of namespace libevrpc












/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

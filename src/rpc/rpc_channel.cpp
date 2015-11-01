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

Channel::Channel(const char* addr, const char* port) :
    is_channel_async_call_(false) {
    strcpy(addr_ = (char*)malloc(strlen(addr) + 1), addr);
    strcpy(port_ = (char*)malloc(strlen(port) + 1), port);
}

Channel::~Channel() {
    free(addr_);
    free(port_);
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

    string send_str;
    if (!request->SerializeToString(&send_str)) {
        perror("SerializeToString request failed!");
        return;
    }
    uint32_t hash_code = BKDRHash(method->full_name().c_str());
    if (RpcSend(connect_fd_, hash_code, send_str, false) < 0) {
        return;
    }

    string recv_str;
    int32_t ret_id = RpcRecv(connect_fd_, recv_str, true);
    if (ERROR_RECV == ret_id) {
        perror("Recv data error in rpc channel");
        return;
    }

    if (!response->ParseFromString(recv_str)) {
        perror("SerializeToString response error in RpcChannel!");
        // TODO
    }

}

void Channel::Close() {
}

bool Channel::OpenRpcAsyncMode() {
    is_channel_async_call_ = true;
    return true;
}

bool Channel::RpcCommunication() {
    return true;
}





}  // end of namespace libevrpc



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

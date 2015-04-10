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

#include "logger.h"
#include "rpc_util.h"
#include "rpc_msg.pb.h"
#include "pub_define.h"
#include "socket_util.h"

namespace libevrpc {

using std::string;

Channel::Channel(const char* addr, const char* port) {
    strcpy(addr_ = (char*)malloc(strlen(addr) + 1), addr);
    strcpy(port_ = (char*)malloc(strlen(port) + 1), port);
}

Channel::~Channel() {
    free(addr_);
    free(port_);
}

// used by CallMethod
bool FormatSendMsg(
        const MethodDescriptor* method, const Message* request, string& send_str);
// used by CallMethod
bool FormatRecvMsg(
        const string& recv_str, Message* response);

void Channel::CallMethod(const MethodDescriptor* method,
                         RpcController* control,
                         const Message* request,
                         Message* response,
                         Closure* done) {

    connect_fd_ = TcpConnect(addr_, port_);
    if (connect_fd_ < 0) {
        LOGGING(ERROR, "rpc connect server failed!");
        return;
    }

    string send_str;
    if (!FormatSendMsg(method, request, send_str)) {
        return;
    }

    if (SendMsg(connect_fd_, send_str) < 0) {
        LOGGING(ERROR, "send msg error!");
    }

    string recv_str;
    if (RecvMsg(connect_fd_, recv_str) < 0) {
        LOGGING(ERROR, "rcv msg error!");
    }

    if (!FormatRecvMsg(recv_str, response)) {
        LOGGING(ERROR, "format recv msg failed!");
    }
    close(connect_fd_);
}

void Channel::Close() {
}

bool FormatSendMsg(
        const MethodDescriptor* method, const Message* request, string& send_str) {

    uint32_t hash_code = BKDRHash(method->full_name().c_str());

    if (NULL == request) {
        LOGGING(ERROR, "request is null ptr!");
        return false;
    }

    string request_str;
    if (!request->SerializeToString(&request_str)) {
        LOGGING(ERROR, "request SerializeToString has failed!");
        return false;
    }

    if (0 == request_str.size()) {
        request_str = "0";
    }

    RpcMessage rpc_msg;
    rpc_msg.set_head_code(hash_code);
    rpc_msg.set_body_msg(request_str);


    if (!rpc_msg.SerializeToString(&send_str)) {
        LOGGING(ERROR, "request SerializeToString has failed!");
        return false;
    }
    return true;
}

bool FormatRecvMsg(const string& recv_str, Message* response) {
    RpcMessage recv_rpc_msg;
    if (!recv_rpc_msg.ParseFromString(recv_str)) {
        LOGGING(ERROR, "parse recv msg error! %s", recv_str.c_str());
        return false;
    }

    if (SER_INTERNAL_ERROR == recv_rpc_msg.head_code()) {
        LOGGING(ERROR, "server internal error!");
    }

    if ("0" != recv_rpc_msg.body_msg() &&
        !response->ParseFromString(recv_rpc_msg.body_msg())) {
        LOGGING(ERROR, "parse recv body msg error!");
        return false;
    }
    return true;
}


}  // end of namespace libevrpc



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

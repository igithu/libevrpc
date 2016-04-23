/***************************************************************************
 *
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file rpc_heartbeat_server.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2016/04/04 22:10:45
 * @brief
 *
 **/

#include "rpc_heartbeat_server.h"

#include "util/rpc_communication.h"


namespace libevrpc {

RpcHeartbeatServer::RpcHeartbeatServer(const char* hb_host, const char* hb_port) :
    dispatcher_thread_ptr_(NULL), hb_host_(NULL), hb_port_(NULL) {

    hb_host_ = (char*)malloc(strlen(hb_host));
    hb_port_ = (char*)malloc(strlen(hb_port));
    strcpy(hb_host_, hb_host);
    strcpy(hb_port_, hb_port);
}

RpcHeartbeatServer::~RpcHeartbeatServer() {
    if (NULL != hb_host_) {
        free(hb_host_);
    }
    if (NULL != hb_port_) {
        free(hb_port_);
    }
    if (NULL != dispatcher_thread_ptr_) {
        delete dispatcher_thread_ptr_;
    }
}

bool RpcHeartbeatServer::InitHeartbeatServer() {
    int32_t listenfd_ = TcpListen(hb_host_, hb_port_);
    if (listenfd_ < 0) {
        PrintErrorInfo("Rpc server listen current port failed\n");
        return false;
    }
    return true;
}


bool RpcHeartbeatServer::HeartBeatStart() {
    return true;
}

}  // end of namespace libevrpc









/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

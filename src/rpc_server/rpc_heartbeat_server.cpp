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

RpcHeartbeatServer::RpcHeartbeatServer() {
}

RpcHeartbeatServer::~RpcHeartbeatServer() {
}

void RpcHeartbeatServer::Run() {
    int32_t listenfd = TcpListen(hb_host_, hb_port_);
}


}  // end of namespace libevrpc









/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

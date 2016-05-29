/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
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

#include <string.h>
#include <string>

#include "connection_timer_manager.h"
#include "util/rpc_communication.h"
#include "util/rpc_util.h"


namespace libevrpc {

using std::string;


RpcHeartbeatServer::RpcHeartbeatServer(
        const char* hb_host,
        const char* hb_port,
        const char* config_file) :
    hb_host_(NULL),
    hb_port_(NULL),
    config_file_(NULL),
    hb_running_(true) {

    hb_host_ = (char*)malloc(strlen(hb_host));
    hb_port_ = (char*)malloc(strlen(hb_port));
    config_file_ = (char*)malloc(strlen(config_file));
    strcpy(hb_host_, hb_host);
    strcpy(hb_port_, hb_port);
    strcpy(config_file_, config_file);
}

RpcHeartbeatServer::~RpcHeartbeatServer() {
    if (NULL != config_file_) {
        free(config_file_);
    }
    if (NULL != hb_host_) {
        free(hb_host_);
    }
    if (NULL != hb_port_) {
        free(hb_port_);
    }
}

bool RpcHeartbeatServer::InitHeartbeatServer() {
    int32_t server_fd = UdpServerInit(hb_host_, hb_port_);

    // epoller_ = ev_loop_new(EVBACKEND_EPOLL | EVFLAG_NOENV);
    epoller_ = ev_loop_new(0);
    socket_watcher_.data = this;
    ev_io_init(&socket_watcher_, RpcHeartbeatServer::HeartBeatProcessor, server_fd, EV_READ);
    ev_io_start(epoller_, &socket_watcher_);
    return true;
}

void RpcHeartbeatServer::Run() {
    // InitHeartbeatServer();
    // if (NULL == epoller_) {
    //     PrintErrorInfo("The epoller ptr is null!\n");
    //     return;
    // }

    // ev_run(epoller_, 0);
    // ev_loop_destroy(epoller_);
    // epoller_ = NULL;

    int32_t server_fd = UdpServerInit(hb_host_, hb_port_);
    while(hb_running_) {
        string clien_addr;
        if (RpcRecvFrom(server_fd, clien_addr, false) < 0) {
            PrintErrorInfo("HeartBeatSeerver recv info error!");
        }
        /*
        if (GetPeerAddr(server_fd, clien_addr) < 0) {
            PrintErrorInfo("Get client address error!");
            break;
        }
        */
        ConnectionTimerManager& ctm_instance = ConnectionTimerManager::GetInstance(config_file_);
        ctm_instance.InsertRefreshConnectionInfo(clien_addr);
    }
    close(server_fd);
}

void RpcHeartbeatServer::HeartBeatProcessor(struct ev_loop *loop, struct ev_io *watcher, int revents) {
    if (NULL == loop) {
        return;
    }
    if (EV_ERROR & revents) {
        return;
    }
    RpcHeartbeatServer* rhs = (RpcHeartbeatServer*)(watcher->data);
    string recv_info;
    if (RpcRecvFrom(watcher->fd, recv_info, false) < 0) {
        PrintErrorInfo("HeartBeatSeerver recv info error!");
    }
    string clien_addr;
    if (GetPeerAddr(watcher->fd, clien_addr) < 0) {
        PrintErrorInfo("Get client address error!");
        return;
    }
    ConnectionTimerManager& ctm_instance = ConnectionTimerManager::GetInstance(rhs->config_file_);
    ctm_instance.InsertRefreshConnectionInfo(clien_addr);
}


}  // end of namespace libevrpc









/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

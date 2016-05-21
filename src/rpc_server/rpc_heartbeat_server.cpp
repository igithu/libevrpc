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
    dispatcher_thread_ptr_(NULL),
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
    if (NULL != dispatcher_thread_ptr_) {
        delete dispatcher_thread_ptr_;
    }
}

bool RpcHeartbeatServer::InitHeartbeatServer() {
    return true;
}

bool RpcHeartbeatServer::Start() {
    dispatcher_thread_ptr_ = new DispatchThread();
    dispatcher_thread_ptr_->InitializeService(hb_host_, hb_port_, &RpcHeartbeatServer::HeartBeatProcessor, (void*)this);
    if (NULL == dispatcher_thread_ptr_) {
        return false;
    }
    dispatcher_thread_ptr_->Start();
}

bool RpcHeartbeatServer::Wait() {
    if (NULL == dispatcher_thread_ptr_) {
        return false;
    }
    dispatcher_thread_ptr_->Wait();
    return true;
}

bool RpcHeartbeatServer::Stop() {
    if (NULL == dispatcher_thread_ptr_) {
        return false;
    }
    hb_running_ = false;
    dispatcher_thread_ptr_->Stop();
    return true;
}

void RpcHeartbeatServer::HeartBeatProcessor(int32_t fd, void *arg) {
    RpcHeartbeatServer* rhs = (RpcHeartbeatServer*)arg;
    if (NULL == rhs) {
        return;
    }
    string clien_addr;
    if (GetPeerAddr(fd, clien_addr) < 0) {
        PrintErrorInfo("Get client address error!");
        return;
    }
    while (rhs->hb_running_) {
        string recv_info;
        if (RpcRecv(fd, recv_info, false) < 0) {
            PrintErrorInfo("HeartBeatSeerver recv info error!");
            break;
        }
        if (recv_info.empty()) {
            break;
        }
        ConnectionTimerManager& ctm_instance = ConnectionTimerManager::GetInstance(rhs->config_file_);
        ctm_instance.InsertRefreshConnectionInfo(clien_addr);
    }
}


}  // end of namespace libevrpc









/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

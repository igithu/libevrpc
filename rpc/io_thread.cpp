/***************************************************************************
 * 
 * Copyright (c) 2014 aishuyu, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file io_thread.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2014/12/05 16:02:39
 * @brief 
 *  
 **/

#include "io_thread.h"

//#include <sys/epoll.h>
#include <sys/socket.h>

#include "rpc_server.h"
#include "../log/ds_log.h"

namespace libevrpc {

#define MAXEVENTS 100

IOThread::IOThread(const char* addr, const char* port) {
    strcpy(addr_ = (char*)malloc(strlen(addr) + 1), addr);
    strcpy(port_ = (char*)malloc(strlen(port) + 1), port);
}

IOThread::~IOThread() {
    delete addr_;
    delete port_;
}

void IOThread::Run() {
    RpcServer& rpc_server = RpcServer::GetInstance();
    LibevConnector* libev_connector_ptr = rpc_server.GetLibevConnector();
    if (NULL == libev_connector_ptr) {
        return;
    }
    libev_connector_ptr->Initialize(addr_, port_);
    libev_connector_ptr->LibevLoop();


    struct epoll_event events[MAXEVENTS];
    while (true) {
        int32_t ready = connection_manager_ptr->EpollWait(MAXEVENTS, events);
        for (int32_t i = 0; i < ready; ++i) {
            int32_t event_fd = events[i].data.fd;
            uint32_t cur_events = events[i].events;
            if (event_fd == listenfd) {
                DS_LOG(INFO, "new connection!");
                connection_manager_ptr->EpollNewConnect(listenfd);
            } else if (cur_events & EPOLLIN) {
                rpc_server.RpcCall(event_fd);
            } 
        }
    }

}

}  // end of namespace libevrpc




/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

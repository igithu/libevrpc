/***************************************************************************
 *
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file dispatch_thread.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2015/12/31 00:15:08
 * @brief
 *
 **/

#include "dispatch_thread.h"

DispatchThread::DispatchThread() {
}

DispatchThread::~DispatchThread() {
}

bool DispatchThread::InitializeService(const char *host, const char *port) {
int32_t listenfd = TcpListen(host, port);
    if (listenfd < 0) {
        perror("Rpc server listen current port failed\n");
        return false;
    }

    epoller_ = ev_loop_new(EVBACKEND_EPOLL | EVFLAG_NOENV);
    epoller_ = ev_loop_new(0);

    if (NULL == epoller_) {
        perror("Call ev_loop_new failed!\n");
        return false;
    }

    /*
     * set callback AcceptCb, Note the function AcceptCb must be static function
     */
    ev_io_init(&socket_watcher_, LibevConnector::AcceptCb, listenfd, EV_READ);
    ev_io_start(epoller_, &socket_watcher_);

    return true;
}

void DispatchThread::Run() {
}

void DispatchThread::AcceptCb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
}

void DispatchThread::ProcessCb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
}








/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

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

DispatchThread::DispatchThread() : epoller_(NULL) {
}

DispatchThread::~DispatchThread() {
    if (NULL != epoller_) {
        delete epoller_;
    }
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
    socket_watcher_.data = this;
    ev_io_init(&socket_watcher_, LibevConnector::AcceptCb, listenfd, EV_READ);
    ev_io_start(epoller_, &socket_watcher_);

    return true;
}

/*
 * start to run
 */
void DispatchThread::Run() {
    if (NULL == epoller_) {
        perror("The epoller ptr is null!\n");
        return;
    }
    ev_run(epoller_, 0);
    ev_loop_destroy(epoller_);
    epoller_ = NULL:
}

void DispatchThread::AcceptCb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
    if (NULL == loop) {
        perror("Ev loop ptr is null!\n");
        return;
    }

    if (EV_ERROR & revents) {
        perror("EV_ERROR in AcceptCb callback!\n");
        return;
    }
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    int32_t cfd = Accept(watcher->fd, client_addr, len);
    if (cfd < 0) {
        return;
    }

    // struct ev_io *client_eio = (struct ev_io*)malloc(sizeof(struct ev_io));
    DispatchThread* dt = (DispatchThread*)(watcher->data);
    struct ev_io *client_eio = dt->NewEIO();
    ev_io_init(client_eio, LibevConnector::ProcessCb, cfd, EV_READ);
    ev_io_start(loop, client_eio);
}

void DispatchThread::ProcessCb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
}

ev_io* DispatchThread::NewEIO() {
}

ev_io* DispatchThread::PopEIO() {
}

void DispatchThread::PushEIO(ev_io* eio) {
}

void DispatchThread::FreeEIO(ev_io* eio) {
}









/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

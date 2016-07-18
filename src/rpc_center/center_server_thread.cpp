/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file center_server_thread.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2016/07/17 02:31:37
 * @brief
 *
 **/

#include "center_server_thread.h"

#include "rpc_center.h"


namespace libevrpc {

static RpcServer& g_rpc_server = RpcServer::GetInstance();

int32_t CenterServerThread::ei_per_alloc_ = 4;
CenterEioQueue CenterServerThread::eio_uselist_ = {NULL, NULL};

CenterServerThread::CenterServerThread(
        const char* local_center,
        const char* server_port) :
    local_center_(NULL),
    server_port_(NULL),
    epoller_(NULL) {

    local_center_ =  (char*)malloc(strlen(local_center));
    server_port_ = (char*)malloc(strlen(server_port));

    strcpy(local_center_, local_center);
    strcpy(server_port_, server_port);
}

CenterServerThread::~CenterServerThread() {
    if (NULL != local_center_) {
        free(local_center_);
    }
    if (NULL != server_port_) {
        free(server_port_);
    }
    if (NULL != epoller_) {
        ev_loop_destroy(epoller_);
    }
}

bool CenterServerThread::InitCenterServer() {
    int32_t listenfd = TcpListen(local_center_, server_port_);
    if (listenfd < 0) {
        fprintf(stderr, "Center Listen Server Init failed!\n");
        return false;
    }
    epoller_ = ev_loop_new(EVBACKEND_EPOLL | EVFLAG_NOENV);
    if (NULL == epoller_) {
        fprintf(stderr,"Center new epoller failed!\n");
        return false;
    }

    socket_watcher_.data = this;
    /*
     * 初始启动 Accept epoller事件 负责接收请求
     */
    ev_io_init(&socket_watcher_, DispatchThread::AcceptCb, listenfd, EV_READ);
    ev_io_start(epoller_, &socket_watcher_);

    return true;
}

void CenterServerThread::Run() {
    if (!InitCenterServer()) {
        fprintf(stderr, "Center epoller is NULL\n");
        return;
    }
    if (NULL == epoller_) {
        fprintf(stderr, "Center epoller is NULL\n");
        return;
    }
    ev_run(epoller_, 0);
    ev_loop_destroy(epoller_);
    epoller_ = NULL;
}

void CenterServerThread::AcceptCb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
    if (NULL == loop) {
        fprintf(stderr, "Center Ev loop ptr is null!\n");
        return;
    }

    if (EV_ERROR & revents) {
        fprintf(stderr, "Center EV_ERROR in AcceptCb\n");
        return;
    }

    struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    int32_t cfd = Accept(watcher->fd, client_addr, len);
    if (cfd < 0) {
        fprintf(stderr, "Center Accept failed!\n");
        return;
    }

    /*
     * 初始化启动处理epoller事件
     */
    CenterServerThread* cst = (CenterServerThread*)(watcher->data);
    CEIO_ITEM *eio_item = dt->NewCEIO();
    struct ev_io& client_eio = eio_item->eio;
    client_eio.data = watcher->data;
    ev_io_init(&client_eio, CenterServerThread::Processor, cfd, EV_READ);
    ev_io_start(loop, &client_eio);
    PushCEIO(eio_uselist_, eio_item);
}

void CenterServerThread::Processor(struct ev_loop *loop, struct ev_io *watcher, int revents) {
}

CEIO_ITEM* CenterServerThread::NewCEIO() {
}

void CenterServerThread::FreeCEIO(CEIO_ITEM* eio_item) {
}

void CenterServerThread::PushCEIO(CenterEioQueue& eq, CEIO_ITEM* eio_item) {
}

CEIO_ITEM* CenterServerThread::PopCEIO(CenterEioQueue& eq) {
}


}  // end of namespace libevrpc








/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

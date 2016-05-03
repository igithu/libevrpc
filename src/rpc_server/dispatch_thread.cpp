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

#include "util/rpc_communication.h"
#include "util/rpc_util.h"

namespace libevrpc {

int32_t DispatchThread::ei_per_alloc_ = 4;
EioQueue DispatchThread::eio_uselist_ = {NULL, NULL};

DispatchThread::DispatchThread() : epoller_(NULL), eio_freelist_(NULL) {
}

DispatchThread::~DispatchThread() {
    Stop();
    Wait();
    if (NULL != epoller_) {
        delete epoller_;
    }
    if (NULL != eio_freelist_) {
        for (EIO_ITEM* eif = eio_freelist_->next; eio_freelist_ != NULL; ) {
            free(eio_freelist_);
            eio_freelist_ = NULL;
            eio_freelist_ = eif;
            if (NULL != eif) {
                eif = eif->next;
            }
        }
    }
    EioQueue& eul = DispatchThread::eio_uselist_;
    for (EIO_ITEM* eiptr = eul.head ; eiptr != eul.tail && eiptr != NULL; ) {
        eul.head = eiptr->next;
        free(eiptr);
        eiptr = eul.head;
    }
}

bool DispatchThread::InitializeService(
        const char *host,
        const char *port,
        RpcCallBackPtr cb,
        void *arg) {
    /*
     * copy the callback function to mem callback_
     */
    cb_ptr_ = cb;
    cb_arg_ = arg;

    int32_t listenfd = TcpListen(host, port);
    if (listenfd < 0) {
        PrintErrorInfo("Rpc server listen current port failed\n");
        return false;
    }

    epoller_ = ev_loop_new(EVBACKEND_EPOLL | EVFLAG_NOENV);
    epoller_ = ev_loop_new(0);

    if (NULL == epoller_) {
        PrintErrorInfo("Call ev_loop_new failed!\n");
        return false;
    }

    /*
     * set callback AcceptCb, Note the function AcceptCb must be static function
     */
    socket_watcher_.data = this;
    ev_io_init(&socket_watcher_, DispatchThread::AcceptCb, listenfd, EV_READ);
    ev_io_start(epoller_, &socket_watcher_);

    return true;
}

/*
 * start to run
 */
void DispatchThread::Run() {
    if (NULL == epoller_) {
        PrintErrorInfo("The epoller ptr is null!\n");
        return;
    }
    ev_run(epoller_, 0);
    ev_loop_destroy(epoller_);
    epoller_ = NULL;
}

void DispatchThread::AcceptCb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
    if (NULL == loop) {
        PrintErrorInfo("Ev loop ptr is null!\n");
        return;
    }

    if (EV_ERROR & revents) {
        PrintErrorInfo("EV_ERROR in AcceptCb callback!\n");
        return;
    }
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    int32_t cfd = Accept(watcher->fd, client_addr, len);
    if (cfd < 0) {
        return;
    }

    DispatchThread* dt = (DispatchThread*)(watcher->data);
    EIO_ITEM *eio_item = dt->NewEIO();
    struct ev_io& client_eio = eio_item->eio;
    client_eio.data = watcher->data;
    ev_io_init(&client_eio, DispatchThread::ProcessCb, cfd, EV_READ);
    ev_io_start(loop, &client_eio);
    PushEIO(eio_uselist_, eio_item);
}

/*
 * handle the connection and callback
 */
void DispatchThread::ProcessCb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
    if (EV_ERROR & revents) {
        PrintErrorInfo("EV_ERROR in AcceptCb callback!\n");
        return;
    }
    DispatchThread* dt = (DispatchThread*)(watcher->data);
    dt->cb_ptr_(watcher->fd, dt->cb_arg_);
    ev_io_stop(loop, watcher);
    EIO_ITEM* ei = PopEIO(eio_uselist_);
    dt->FreeEIO(ei);
}

EIO_ITEM* DispatchThread::NewEIO() {
    EIO_ITEM* ei = NULL;
    if (NULL != eio_freelist_) {
        ei = eio_freelist_;
        eio_freelist_ = eio_freelist_->next;
    }
    if (NULL == ei) {
         ei = (EIO_ITEM*)malloc(sizeof(EIO_ITEM) * ei_per_alloc_);
         if (NULL == ei) {
             PrintErrorInfo("Alloc the ei item mem failed!");
             return NULL;
         }
         for (int i = 0; i < ei_per_alloc_; ++i) {
             ei[i - 1].next = &ei[i];
         }
         ei[ei_per_alloc_ - 1].next = NULL;
         ei[ei_per_alloc_ - 1].next = eio_freelist_;
         eio_freelist_ = &ei[1];
    }
    return ei;
}

void DispatchThread::FreeEIO(EIO_ITEM* eio_item) {
    eio_item->next = eio_freelist_;
    eio_freelist_ = eio_item;
}

void DispatchThread::PushEIO(EioQueue& eq,  EIO_ITEM* eio_item) {
    eio_item->next = NULL;
    if (NULL == eq.tail) {
        eq.head = eio_item;
    } else {
        eq.tail->next = eio_item;
    }
    eq.tail = eio_item;
}

EIO_ITEM* DispatchThread::PopEIO(EioQueue& eq) {
    EIO_ITEM* ei = NULL;
    ei = eq.head;
    if (NULL != ei) {
        eq.head = ei->next;
        if (NULL == eq.head) {
            eq.tail = NULL;
        }
    }
    return ei;
}


}  // end of namespace libevrpc






/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

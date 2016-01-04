/***************************************************************************
 *
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file dispatch_thread.h
 * @author aishuyu(asy5178@163.com)
 * @date 2015/12/31 00:04:24
 * @brief
 *
 **/




#ifndef __DISPATCH_THREAD_H
#define __DISPATCH_THREAD_H

#include <ev.h>

#include "util/thread.h"

namespace libevrpc {

using namespace PUBLIC_UTIL;

/*
 * callbancl in dispatch_thread
 */
typedef struct {
    void *(*callback)(void * arg);
    void *params;
} DCallBack;

/*
 * ev_io item to link all ev_io
 */
typedef struct EIOItem EIO_ITEM;
struct EIOItem {
    ev_io eio;
    EIO_ITEM* prev;
    EIO_ITEM* next;
};

typedef void (*RpcCallBack)(int32_t, void *arg);
class DispatchThread : public Thread {
    public:
        DispatchThread();

        ~DispatchThread();

        bool InitializeService(const char *host, const char *port, RpcCallBack* cb, void *arg);

        virtual void Run();

    private:
        /*
         * libev callback
         */
        static void AcceptCb(struct ev_loop *loop, struct ev_io *watcher, int revents);
        static void ProcessCb(struct ev_loop *loop, struct ev_io *watcher, int revents);

        /*
         * eio new and free
         */
        EIO_ITEM* NewEIO();
        void FreeEIO(EIO_ITEM* eio_item);

    private:
        struct ev_loop *epoller_;
        struct ev_io socket_watcher_;
        ev_io* eio_freelist_;

        RpcCallBack* cb_;
        void* cb_arg_;
};

}


#endif // __DISPATCH_THREAD_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

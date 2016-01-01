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

typedef struct {
    void *(*callback)(void * arg);
    void *params
} DCallBack;

class DispatchThread : public Thread {
    public:
        DispatchThread();

        ~DispatchThread();

        bool InitializeService(const char *host, const char *port, DCallBack* d_callback);

        virtual void Run()

    private:
        static void AcceptCb(struct ev_loop *loop, struct ev_io *watcher, int revents);

        static void ProcessCb(struct ev_loop *loop, struct ev_io *watcher, int revents);

    private:
        struct ev_loop *epoller_;

        struct ev_io socket_watcher_;
};

}


#endif // __DISPATCH_THREAD_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file center_server_thread.h
 * @author aishuyu(asy5178@163.com)
 * @date 2016/07/17 02:29:13
 * @brief
 *
 **/




#ifndef __CENTER_SERVER_THREAD_H
#define __CENTER_SERVER_THREAD_H

#include <ev.h>

#include "util/thread.h"

namespace libevrpc {

typedef struct CenterEIOItem CEIO_ITEM;
struct CenterEIOItem {
    ev_io eio;
    CEIO_ITEM* prev;
    CEIO_ITEM* next;
};

struct CenterEioQueue {
    CEIO_ITEM* head;
    CEIO_ITEM* tail;
};

class CenterServerThread : public Thread {
    public:
        CenterServerThread(const char* local_center, const char* server_port);
        ~CenterServerThread();

        bool InitCenterServer();

        virtual void Run();

    private:
        static void AcceptCb(struct ev_loop *loop, struct ev_io *watcher, int revents);
        static void Processor(struct ev_loop *loop, struct ev_io *watcher, int revents);

        CEIO_ITEM* NewCEIO();
        void FreeCEIO(CEIO_ITEM* eio_item);
        static void PushCEIO(CenterEioQueue& eq, CEIO_ITEM* eio_item);
        static CEIO_ITEM* PopCEIO(CenterEioQueue& eq);

    private:
        char* local_center_;
        char* center_port_;

        struct ev_loop *epoller_;
        struct ev_io socket_watcher_;

        CEIO_ITEM* eio_freelist_;
        static int32_t ei_per_alloc_;
        static CenterEioQueue eio_uselist_;

};

}  // end of namespace libevrpc




#endif // __CENTER_SERVER_THREAD_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

/***************************************************************************A
 * 
 * Copyright (c) 2014 aishuyu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file libev_thread_pool.h
 * @author aishuyu(asy5178@163.com)
 * @date 2014/11/07 15:32:07
 * @brief 
 *  
 **/




#ifndef  __LIBEV_THREAD_POOL_H_
#define  __LIBEV_THREAD_POOL_H_


//#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
// #include <atomic.h>

#include <ev.h>

#include "pthread_mutex.h"

namespace libevrpc {

/*
 * rpc call info item
 */
typedef struct RequestQueueItem RQ_ITEM;
struct RequestQueueItem {
    // callback funtion
    void *(*processor)(void * arg);
    void *param;
    RQ_ITEM *prev;
    RQ_ITEM *next;
};

/*
 * rpc call queue
 */
typedef struct RequestQueue RQ;
struct RequestQueue {
    RQ_ITEM* head;
    RQ_ITEM* tail;
    PUBLIC_UTIL::Mutex q_mutex;
};

/*
 * thread with libev info
 */
class LibevThreadPool;
typedef struct {
    pthread_t thread_id;
    struct ev_loop* epoller;
    struct ev_io libev_watcher;
    int32_t notify_receive_fd;
    int32_t notify_send_fd;
    RQ* new_request_queue;
    LibevThreadPool* lt_pool;
} LIBEV_THREAD;

class LibevThreadPool {
    public:
        LibevThreadPool();

        virtual ~LibevThreadPool();

        bool Start();

        bool Wait();

        /*
         * nonblock call the processor and return shortly
         */
        bool DispatchRpcCall(void *(*process) (void *arg), void *arg);

    private:
        bool LibevThreadInitialization(int num_threads);

        /*
         * request itesm op
         */
        RQ_ITEM* RQItemNew();
        RQ_ITEM* RQItemPop(RQ* req_queue);
        bool RQItemPush(RQ* req_queue, RQ_ITEM* req_item);
        bool RQItemFree(RQ_ITEM* req_item);

        /*
         * static fuction
         */
        static void LibevProcessor(struct ev_loop *loop, struct ev_io *watcher, int revents);
        static void *LibevWorker(void *arg);

        bool Destroy();

    private:
        /*
         * threads
         */
        int32_t current_thread_;
        int32_t num_threads_;

        LIBEV_THREAD* libev_threads_;
        RQ_ITEM*  rqi_freelist_;

        PUBLIC_UTIL::Mutex rqi_freelist_mutex_;

        static int32_t item_per_alloc_;
        //static atomic_bool running_;
};


} // end of namespace libevrpc

#endif  //__LIBEV_THREAD_POOL_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
